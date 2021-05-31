#include "ccbot.h"

#include <QSettings>
#include <QApplication>
#include <QDir>
#include <QUrl>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>
#include <QStandardPaths>

#include "misc.h"

CCBot::CCBot(Properties *params, QObject *parent) : CCBotEngine(parent),
    m_player(new QMediaPlayer)
{    
    m_params = params;
    loadSettings();
}

CCBot::~CCBot()
{
    delete m_player;
}

void CCBot::start()
{
    initConnections();
    initTasks();
    initTimers();

    if (!QSqlDatabase::drivers().contains("QSQLITE")) {
        emit showMessage("Ошибка", "Unable to load database.\nNeeds the SQLITE driver!", true);
    } else {
        initDB();
    }
}

void CCBot::initDB()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
}

void CCBot::loadSettings()
{
    QSettings cfg;

    cfg.beginGroup("Window");
    m_params->setWindowX(cfg.value("X", 0).toInt());
    m_params->setWindowY(cfg.value("Y", 0).toInt());
    m_params->setWindowHeight(cfg.value("HEIGHT", 900).toInt());
    m_params->setWindowWidth(cfg.value("WIDTH", 1000).toInt());
    cfg.endGroup();

    cfg.beginGroup("View");
    m_params->setFontNameForChat(cfg.value("FontNameForChat", "Noto Sans").toString());
    m_params->setFontPointSizeForChat(
                cfg.value("FontPointSizeForChat", 12.0).toFloat());
    m_params->setTextColorForChat(cfg.value("TextColorForChat", QColor("#bfc7d0")).value<QColor>());
    cfg.endGroup();

    cfg.beginGroup("Server");
    m_params->setMaxTimestampDiff(cfg.value("MaxTDiff", 5).toInt());
    m_params->setListenHost(cfg.value("Host", "127.0.0.1").toString());
    m_params->setListenPort(cfg.value("Port", 3000U).toUInt());
    cfg.endGroup();

    cfg.beginGroup("SpeechKit");
    m_params->setSpeechkitFolderId(cfg.value("FolderID", "").toString());
    m_params->setSpeechkitOAuthToken(cfg.value("OAuthToken", "").toString());
    m_params->setSpeechkitHost(
                cfg.value("Host", defaultSpeechkitHost).toString());
    m_params->setSpeechkitGetIamTokenHost(
                cfg.value("IamTokenHost",
                          defaultSpeechkitGetIamTokenHost).toString());
    m_params->setSpeechkitLang(
                cfg.value("Lang", defaultSpeechkitLang).toString());
    m_params->setSpeechkitFormat(
                cfg.value("Format", defaultSpeechkitFormat).toString());
    m_params->setSpeechkitVoice(
                cfg.value("Voice", defaultSpeechkitVoice).toString());
    m_params->setSpeechkitEmotion(
                cfg.value("Emotion", defaultSpeechkitEmotion).toString());
    m_params->setSpeechkitSpeed(
                cfg.value("Speed", defaultSpeechkitSpeed).toString());
    m_params->setSpeechkitSampleRateHertz(
                cfg.value("SampleRateHertz",
                          defaultSpeechkitSampleRateHertz).toString());
    cfg.endGroup();
}

void CCBot::saveSettings()
{
    QSettings cfg;

    cfg.beginGroup("Window");
    cfg.setValue("X", m_params->windowX());
    cfg.setValue("Y", m_params->windowY());
    cfg.setValue("HEIGHT", m_params->windowHeight());
    cfg.setValue("WIDTH", m_params->windowWidth());
    cfg.endGroup();

    cfg.beginGroup("View");
    cfg.setValue("FontNameForChat", m_params->fontNameForChat());
    cfg.setValue("FontPointSizeForChat", m_params->fontPointSizeForChat());
    cfg.setValue("TextColorForChat", m_params->textColorForChat());
    cfg.endGroup();

    cfg.beginGroup("Server");
    cfg.setValue("MaxTDiff", m_params->maxTimestampDiff());
    cfg.setValue("Host", m_params->listenHost());
    cfg.setValue("Port", m_params->listenPort());
    cfg.endGroup();

    cfg.beginGroup("SpeechKit");
    cfg.setValue("FolderID", m_params->speechkitFolderId());
    cfg.setValue("OAuthToken", m_params->speechkitOAuthToken());
    cfg.setValue("Host", m_params->speechkitHost());
    cfg.setValue("IamTokenHost", m_params->speechkitGetIamTokenHost());
    cfg.setValue("Lang", m_params->speechkitLang());
    cfg.setValue("Format", m_params->speechkitFormat());
    cfg.setValue("Voice", m_params->speechkitVoice());
    cfg.setValue("Emotion", m_params->speechkitEmotion());
    cfg.setValue("Speed", m_params->speechkitSpeed());
    cfg.setValue("SampleRateHertz", m_params->speechkitSampleRateHertz());
    cfg.endGroup();
    //...
}

void CCBot::initTimers()
{
    //
}

void CCBot::initConnections()
{
    // соединение: завершение программы
    connect(this, &CCBotEngine::signQuit, [=]() {
        QApplication::quit();
    });

    connect(m_player,
            &QMediaPlayer::stateChanged,
            [this](QMediaPlayer::State state)
    {
        if (state == QMediaPlayer::StoppedState) {
            emit completePlayFile();
        }
    });

//    connect(m_params, &Properties::listenClientsChanged, [this]() {
//        if (m_params->listenClients() == true) {
//            //openDB();
//        } else {
//            closeDB();
//        }
//    });
}

void CCBot::initTasks()
{
    m_pCore->registerTask(CCBotTaskEnums::MergeChat,
                          [this](
                          QString streamId,
                          QString messagesJsonStr,
                          bool loading) -> TaskResult
    {
        TaskResult result;
        QString errInfo = "";
        int state = 0;

        QMetaObject::invokeMethod(
                    this,
                    "insertNewMessagesInTable",
                    Qt::BlockingQueuedConnection,
                    Q_RETURN_ARG(int, state),
                    Q_ARG(QString, streamId),
                    Q_ARG(QByteArray, messagesJsonStr.toUtf8()),
                    Q_ARG(bool, !loading),
                    Q_ARG(QString*, &errInfo)
                    );

        if(state != CCBotErrEnums::Ok) {
            return TaskResult(state, errInfo);
        }
        return TaskResult();
    });

    m_pCore->registerTask(CCBotTaskEnums::VoiceLoad,
                          [this](QString text) -> TaskResult {
        TaskResult result;
        QNetworkAccessManager *manager = new QNetworkAccessManager();

        // 1. Проверка что токен истек
        bool tokenExpiry =
                QDateTime::currentDateTime() >= m_params->speechkitIamTokenExpiryDate();
        if (tokenExpiry) {
            // 1.1 Если токен уже не действителен то:
            // получаем новый и обновляем дату

            // * формируем запрос
            QNetworkRequest requestGetIamToken;
            requestGetIamToken.setUrl(
                        QUrl(m_params->speechkitGetIamTokenHost()));
            requestGetIamToken.setHeader(
                        QNetworkRequest::ContentTypeHeader,
                        "application/json");
            QJsonObject obj;
            obj["yandexPassportOauthToken"] = m_params->speechkitOAuthToken();
            QJsonDocument doc(obj);
            QByteArray data = doc.toJson();

            // * делаем запрос
            manager->setTransferTimeout(constTimeoutGetIamToken);
            QNetworkReply *reply = manager->post(requestGetIamToken, data);
            QNetworkReply::NetworkError errType = QNetworkReply::NoError;
            QList<QSslError> errorsSsl;

            connect(reply,
                    &QNetworkReply::finished,
                    this,
                    [&]() {
                QByteArray response = reply->readAll();
                QJsonObject responseObj =
                        QJsonDocument::fromJson(response).object();
                m_params->setSpeechkitIamToken(
                            responseObj.value("iamToken").toString());
                m_params->setSpeechkitIamTokenExpiryDate(
                            QDateTime::fromString(
                                responseObj.value("expiresAt").toString(),
                                Qt::ISODateWithMs)
                            );
                //qDebug() << "IamToken: " << m_params->speechkitIamToken();
                //qDebug() << "expiresAt: " << m_params->speechkitIamTokenExpiryDate().toString();
                emit completeRequestGetIamToken();
            });

            connect(reply,
                    &QNetworkReply::errorOccurred,
                    this,
                    [&](QNetworkReply::NetworkError error) {
                errType = error;
                //qDebug() << "err" << error;
                emit completeRequestGetIamToken();
            });

            connect(reply,
                    &QNetworkReply::sslErrors,
                    this,
                    [&](const QList<QSslError> &errors) {
                errorsSsl = errors;
                //qDebug() << "TEST_2";
                emit completeRequestGetIamToken();
            });

            bool timeout = !waitSignal(this,
                                       &CCBot::completeRequestGetIamToken,
                                       constTimeoutGetIamToken + 500);
            reply->disconnect();

            if(timeout) {
                result = TaskResult(CCBotErrEnums::NetworkRequest,
                                    "Timeout request, not get iam-token!");
                delete manager;
                return result;
            }

            if(errType != QNetworkReply::NoError || !errorsSsl.isEmpty()) {
                QString mainInfo = QString("Error request (")
                        + QString::number(static_cast<int>(errType))
                        + "), ";
                QString secondInfo =
                        errorsSsl.isEmpty() ?
                            QString("no ssl errors") :
                            QString("%1 ssl errors").arg(errorsSsl.size());
                result = TaskResult(CCBotErrEnums::NetworkRequest,
                                    mainInfo + secondInfo);
                delete manager;
                return result;
            }
        }

        // 2. Запрос звукового файла с сервера на текст
        // повторная проверка даты токена
        tokenExpiry = QDateTime::currentDateTime() >= m_params->speechkitIamTokenExpiryDate();
        if (!tokenExpiry) {
            QNetworkRequest requestGetAudio;
            QUrl url(m_params->speechkitHost());
            QUrlQuery postDataEncoded;

            // * add header
            requestGetAudio.setHeader(QNetworkRequest::ContentTypeHeader,
                                      "application/x-www-form-urlencoded");
            requestGetAudio.setRawHeader("Authorization",
                                         QString("Bearer %1")
                                         .arg(m_params->speechkitIamToken())
                                         .toUtf8());
            // * add data
            postDataEncoded.addQueryItem("text", text);

            postDataEncoded.addQueryItem("folderId", m_params->speechkitFolderId());

            if (!m_params->speechkitLang().isEmpty()) {
                postDataEncoded.addQueryItem("lang", m_params->speechkitLang());
            }
            if (!m_params->speechkitVoice().isEmpty()) {
                postDataEncoded.addQueryItem("voice", m_params->speechkitVoice());
            }
            if (!m_params->speechkitEmotion().isEmpty()) {
                postDataEncoded.addQueryItem("emotion", m_params->speechkitEmotion());
            }
            if (!m_params->speechkitSpeed().isEmpty()) {
                postDataEncoded.addQueryItem("speed", m_params->speechkitSpeed());
            }
            if (!m_params->speechkitFormat().isEmpty()) {
                postDataEncoded.addQueryItem("format", m_params->speechkitFormat());
            }
            if (!m_params->speechkitSampleRateHertz().isEmpty()) {
                postDataEncoded.addQueryItem("sampleRateHertz",
                                      m_params->speechkitSampleRateHertz());
            }

            // * запрос
            // * делаем запрос
            manager->setTransferTimeout(constTimeoutGetAudio);
            requestGetAudio.setUrl(url);
            QNetworkReply *reply = manager->post(requestGetAudio, postDataEncoded.toString(QUrl::FullyEncoded).toUtf8());
            QNetworkReply::NetworkError errType = QNetworkReply::NoError;
            QList<QSslError> errorsSsl;
            connect(reply, &QNetworkReply::finished, this, [&reply,this]() {
                QByteArray response = reply->readAll();
                if (QJsonDocument::fromJson(response).isObject()) {
                    //qDebug() << "no_audio: " << response;
                } else {
                    //qDebug() << "with audio";
                    // сохраняем файл на диске
                    // и передаем новой задачи имя файла
                    QTemporaryFile tmpfile;
                    if (tmpfile.open()) {
                        tmpfile.setAutoRemove(false);
                        QDataStream ostream(&tmpfile);
                        ostream.writeRawData(response, response.size());
                        tmpfile.close();
                        m_pCore->addTask(CCBotTaskEnums::VoiceSpeech,
                                         tmpfile.fileName());
                    }
                }
                emit completeRequestGetAudio();
            });
            connect(reply,
                    &QNetworkReply::errorOccurred,
                    this,
                    [&](QNetworkReply::NetworkError error) {
                errType = error;
                //qDebug() << "err" << error;
                emit completeRequestGetAudio();
            });
            connect(reply,
                    &QNetworkReply::sslErrors,
                    this,
                    [&](const QList<QSslError> &errors) {
                errorsSsl = errors;
                //qDebug() << "TEST_2";
                emit completeRequestGetAudio();
            });

            bool timeout = !waitSignal(this,
                                       &CCBot::completeRequestGetAudio,
                                       constTimeoutGetAudio + 500);
            reply->disconnect();

            if(timeout) {
                result =
                        TaskResult(CCBotErrEnums::NetworkRequest,
                                   "Timeout request, not get audio!");
                delete manager;
                return result;
            }
            if(errType != QNetworkReply::NoError ||
                    !errorsSsl.isEmpty())
            {
                QString mainInfo =
                        QString("Error request (")
                        + QString::number(static_cast<int>(errType))
                        + "), ";
                QString secondInfo =
                        errorsSsl.isEmpty() ?
                            QString("no ssl errors") :
                            QString("%1 ssl errors").arg(errorsSsl.size());
                result = TaskResult(CCBotErrEnums::NetworkRequest,
                                    mainInfo + secondInfo);
                delete manager;
                return result;
            }
        }
        // 3. Завершение
        delete manager;
        return result;
    }, 1);

    m_pCore->registerTask(CCBotTaskEnums::VoiceSpeech,
                          [this](QString filename) -> TaskResult
    {
        TaskResult result;
        //qDebug() << "speek_filename: " << filename;
        QMetaObject::invokeMethod(this,
                                  "speechFile",
                                  Qt::BlockingQueuedConnection,
                                  Q_ARG(QString, filename)
                                  );
        // очистка
        QFile::remove(filename);
        return TaskResult();
    }, 2);
}

void CCBot::speechFile(QString filename)
{
    waitSignalAfterFunction(this,
                            &CCBot::completePlayFile,
                            [&filename, this]()
    {
        m_player->setMedia(QUrl::fromLocalFile(filename));
        m_player->setVolume(100);
        m_player->play();
    });
}

QString CCBot::generateErrMsg(int type, int errCode, QString info)
{
    Q_UNUSED(type)

    if(errCode == CCBotErrEnums::Ok)
        return "";
    if(errCode == CCBotErrEnums::NoInit)
        return QString("Задача не выполнялась, результат не инициализирован.");
    if(errCode == CCBotErrEnums::ParseJson)
        return QString("Фатальная ошибка, не удалось распарсить JSON-данные.");
    if(errCode == CCBotErrEnums::Sql)
        return QString("Ошибка SQL: %1").arg(info);
    if(errCode == CCBotErrEnums::NetworkRequest)
        return QString("%1").arg(info);

    return QString("Неизвестная ошибка, нет описания.");
}

QString CCBot::modifyMsg(const QString &text)
{
    // init emotion power counters
    qint32 funnyPower = 0;       // (-) грусть .. радость (+)
    qint32 godnessPower = 0;     // (-) злоба .. доброта (+)
    qint32 likingPower = 0;      // (-) отвращение .. симпатия (+)
    qint32 sicklinessPower = 0;  // болезненный (+)
    qint32 nicelyPower = 0;      // (-) неприятно .. приятно (+)
    qint32 sleepyPower = 0;      // сонный (+)
    qint32 fearPower = 0;        // страх (+)
    qint32 amazePower = 0;       // удивление (+)
    qint32 laughPower = 0;       // ржач (+)
    qint32 pokePower = 0;        // прикалываться, наезд, задираться (+)

    // set counters
    std::wstring studiedText = text.toStdWString();
    for (size_t i = 0; i < studiedText.length(); i++) {
        wchar_t symbol = studiedText.at(i);
        switch (symbol) {
        case 0x1F62C: // 😬 - grimacing face
            godnessPower -= 1;
        case 0x1F638: // 😸 - grinning cat face with smiling eyes
        case 0x1F601: // 😁 - grinning face with smiling eyes
        case 0x1F600: // 😀 - grinning face
            funnyPower += 1;
            laughPower += 1;
            break;
        case 0x1F639: // 😹 - cat face with tears of joy
        case 0x1F602: // 😂 - face with tears of joy
            laughPower += 2;
            break;
        case 0x1F63A: // 😺 - smiling cat face with open mouth
        case 0x1F603: // 😃 - smiling face with open mouth
            funnyPower += 1;
            break;
        case 0x1F604: // 😄 - smiling face with open mouth and smiling eyes
            funnyPower += 2;
            break;
        case 0x1F605: // 😅 - smiling face with open mouth and cold sweat
            funnyPower += 1;
            amazePower += 1;
            fearPower += 1;
            break;
        case 0x1F606: // 😆 - smiling face with open mouth and tightly-closed eyes
            funnyPower += 3;
            laughPower += 2;
            break;
        case 0x1F609: // 😉 - winking face
            likingPower += 1;
            break;
        case 0x1F60A: // 😊 - smiling face with smiling eyes
            funnyPower += 4;
            nicelyPower += 2;
            break;
        case 0x1F60B: // 😋 - face savouring delicious food
            funnyPower += 2;
            nicelyPower += 1;
            break;
        case 0x1F60C: // 😌 - relieved face
            fearPower = 0;
            break;
        case 0x1F63B: // 😻 - smiling cat face with heart-shaped eyes
        case 0x1F60D: // 😍 - smiling face with heart-shaped eyes
            likingPower += 4;
            break;
        case 0x1F63C: // 😼 - cat face with wry smile
        case 0x1F60F: // 😏 - smirking face
            funnyPower += 1;
            godnessPower -= 1;
            break;
        case 0x1F612: // 😒 - unamused face
            funnyPower -= 1;
            break;
        case 0x1F613: // 😓 - face with cold sweat
            funnyPower -= 2;
            fearPower += 1;
            break;
        case 0x1F614: // 😔 - pensive face
        case 0x1F61F: // 😟 - worried face
            funnyPower -= 1;
            break;
        case 0x1F626: // 😦 - frowning face with open mouth
            funnyPower -= 1;
            amazePower += 1;
            break;
        case 0x1F627: // 😧 - anguished face
            funnyPower -= 2;
            amazePower += 2;
            break;
        case 0x1F616: // 😖 - confounded face
            funnyPower -= 2;
            break;
        case 0x1F618: // 😘 - face throwing a kiss
            likingPower += 3;
            break;
        case 0x1F63D: // 😽 - kissing cat face with closed eyes
        case 0x1F61A: // 😚 - kissing face with closed eyes
        case 0x1F617: // 😗 - kissing face
        case 0x1F619: // 😙 - kissing face with smiling eyes
            likingPower += 2;
            break;
        case 0x1F61C: // 😜 - face with stuck-out tongue and winking eye
        case 0x1F61B: // 😛 - face with stuck-out tongue
            likingPower += 1;
            pokePower += 2;
            break;
        case 0x1F61D: // 😝 - face with stuck-out tongue and tightly-closed eyes
            likingPower += 1;
            pokePower += 3;
            break;
        case 0x1F61E: // 😞 - disappointed face
            funnyPower -= 3;
            break;
        case 0x1F620: // 😠 - angry face
            godnessPower -= 2;
            funnyPower -= 2;
            break;
        case 0x1F63E: // 😾 - pouting cat face
        case 0x1F621: // 😡 - pouting face
            funnyPower -= 2;
            nicelyPower -= 2;
            break;
        case 0x1F63F: // 😿 - crying cat face
        case 0x1F622: // 😢 - crying face
            funnyPower -= 2;
            nicelyPower -= 2;
            break;
        case 0x1F623: // 😣 - persevering face
            funnyPower -= 1;
            nicelyPower -= 1;
            break;
        case 0x1F624: // 😤 - face with look of triumph
            funnyPower += 1;
            pokePower += 1;
            godnessPower -= 1;
            break;
        case 0x1F625: // 😥 - disappointed but relieved face
            funnyPower -= 1;
            nicelyPower -= 2;
            break;
        case 0x1F628: // 😨 - fearful face
            fearPower += 2;
            break;
        case 0x1F640: // 🙀 - weary cat face
        case 0x1F629: // 😩 - weary face
        case 0x1F62B: // 😪 - tired face
            funnyPower -= 1;
            sleepyPower += 1;
            break;
        case 0x1F611: // 😑 - expressionless face
            sleepyPower += 1;
            break;
        case 0x1F62A: // 😪 - sleepy face
        case 0x1F4A4: // 💤 - sleeping symbol
        case 0x1F634: // 😴 - sleeping face
            sleepyPower += 2;
            break;
        case 0x1F62D: // 😭 - loudly crying face
            funnyPower -= 4;
            laughPower -= 4;
            break;
        case 0x1F630: // 😰 - face with open mouth and cold sweat
            fearPower += 3;
            break;
        case 0x1F631: // 😰 - face screaming in fear
            fearPower += 4;
            break;
        case 0x1F632: // 😲 - astonished face
            amazePower += 2;
            break;
        case 0x1F633: // 😳 - flushed face
            amazePower += 2;
            nicelyPower += 2;
            break;
        case 0x1F635: // 😵 - dizzy face
            amazePower += 4;
            sicklinessPower += 1;
            break;
        case 0x1F637: // 😷 - face with medical mask
            sicklinessPower += 2;
            break;
        case 0x1F912: // 🤒 - face with thermometer
            sicklinessPower += 3;
            break;
        case 0x1F915: // 🤕 - face with head-bandage
            sicklinessPower += 4;
            break;
        case 0x1F647: // 🙇 - person bowing deeply
            pokePower -= 2;
            likingPower += 4;
            break;
        case 0x1F64F: // 🙏 - person with folded hands
            godnessPower += 2;
            break;
        case 0x263A: // ☺ - white smiling face
            funnyPower += 1;
            nicelyPower += 2;
            break;
        case 0x1F479: // 👹 - japanese ogre
            godnessPower -= 4;
            funnyPower += 1;
            break;
        case 0x1F47A: // 👺 - japanese goblin
            godnessPower -= 3;
            funnyPower += 2;
            break;
        case 0x1F47B: // 👻 - ghost
            godnessPower -= 1;
            funnyPower += 1;
            pokePower += 1;
            break;
        case 0x1F47C: // 👼 - baby angel
            godnessPower += 2;
            funnyPower += 1;
            break;
        case 0x1F47F: // 👿 - imp
            godnessPower -= 2;
            funnyPower -= 2;
            break;
        case 0x1F480: // 💀 - skull
        case 0x2620:
            sicklinessPower += 100;
            break;
        case 0x1F48B: // 💋 - kiss mark
        case 0x1F48F: // 💏 - kiss
            likingPower += 5;
            break;
        case 0x1F48C: // 💌 - love letter
        case 0x1F491: // 💑 - couple with heart
        case 0x1F493: // 💓 - beating heart
        case 0x1F495: // 💕 - two hearts
        case 0x1F496: // 💖 - sparkling heart
        case 0x1F497: // 💗 - growing heart
        case 0x1F498: // 💘 - heart with arrow
        case 0x1F49D: // 💝 - heart with ribbon
        case 0x1F49E: // 💞 - revolving hearts
            likingPower += 10;
            break;
        case 0x1F490: // 💐 - bouquet
            likingPower += 2;
            break;
        case 0x1F494: // 💔 - broken heart
            likingPower -= 10;
            funnyPower -= 50;
            break;
        case 0x1F4A2: // 💢 - anger symbol
            godnessPower -= 2;
            nicelyPower -= 2;
            break;
        case 0x1F607: // 😇 - smiling face with halo
            godnessPower += 2;
            break;
        case 0x1F608: // 😈 - smiling face with horns
            godnessPower -= 2;
            laughPower += 2;
            break;
        case 0x1F615: // 😕 - confused face
            nicelyPower -= 1;
            funnyPower -= 1;
            break;
        case 0x1F62E: // 😮 - face with open mouth
        case 0x1F62F: // 😯 - hushed face
        case 0x1F636: // 😶 - face without mouth
            amazePower += 1;
            break;
        case 0x1F922: // 🤢 - nauseated face
            likingPower -= 3;
            break;
        case 0x1F92E: // 🤮 - face vomiting
            likingPower -= 4;
            break;
        case 0x1F973: // 🥳	- partying face
            funnyPower += 4;
            break;
        case 0x1F4A9: // 💩 - pile of poo
            pokePower += 10;
            break;
        default:
            break;
        }
    }

    return "";
}

bool CCBot::readMessagesFromJsonStr(QByteArray jsonData,
                                    QList<MessageData> &msgList,
                                    QString *errInfo)
{
    QJsonDocument jsonDoc;
    QJsonParseError parseError;

    jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        QString info = QString("Parse error at %1:%2")
                .arg(parseError.offset)
                .arg(parseError.errorString());
        //qDebug() << info;
        if (errInfo) {
            *errInfo = info;
        }
        return false;
    }

    QJsonArray jsonArr = jsonDoc.array();

    for (int i = 0; i < jsonArr.size(); i++) {
        MessageData msg;
        QJsonObject obj = jsonArr.at(i).toObject();
        msg.id = 0;
        msg.type = obj["type"].toInt();
        msg.sender = obj["sender"].toString();
        msg.nik_color = obj["nik_color"].toString();
        msg.msg = obj["msg"].toString();
        msg.pay = static_cast<float>(obj["pay"].toDouble());
        msg.flagIsNewMsg = true;
        msgList.append(msg);
    }

    return true;
}

bool CCBot::openDB(QString name)
{
    QString path =
            QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

//    qDebug() << path;

    if (path.isEmpty()) {
        qDebug() << "Cannot determine settings storage location";
        path = QDir::homePath() + QDir::separator() + ".ccbot";
    }

    QDir d{path};

    if (!d.exists()) {
        if (!d.mkpath(d.absolutePath())) {
            //qDebug() << QString("Cannot create path: %1").arg(d.absolutePath());
            return false;
        }
    }

    QString file_path = path + QDir::separator() + (name.isEmpty() ? constNameBaseStr : name);

    m_db.setDatabaseName(file_path);
    if (!m_db.open()) {
        emit showMessage("Ошибка", QString("Не удалось открыть базу.\n") + m_db.lastError().text(), true);
        //qDebug() << "Error, missing database or opened from another program!";
        return false;
    }

    return true;
}

bool CCBot::isOpenedDB()
{
    return m_db.isOpen();
}

void CCBot::closeDB()
{
    m_db.close();
}

bool CCBot::createTableDB(QString streamId)
{
    QSqlQuery qry;

    const QString values =  "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                            "type INTEGER NOT NULL, "
                            "sender TEXT, "
                            "nik_color TEXT, "
                            "msg TEXT, "
                            "pay REAL, "
                            "timestamp DATETIME";
    const QString sql = QString("CREATE TABLE IF NOT EXISTS 't_%1' (%2)")
            .arg(streamId, values);

    bool state = qry.exec(sql);

    return state;
}

bool CCBot::existsTableDB(QString streamId)
{
    QString tableName = "t_" + streamId;
    bool state = m_db.tables().contains(tableName);

    return state;
}

bool CCBot::selectMsgsFromTableDB(QString streamId,
                                  QList<MessageData> &msgList,
                                  int limit)
{
    QSqlQuery qry;
    QString sql;

    if(limit > 0) {
        sql = QString("SELECT * FROM ( "
                    "SELECT * FROM t_%1 ORDER BY id DESC LIMIT %2) "
                    "ORDER BY id ASC;")
                .arg(streamId).arg(limit);
    } else {
        sql = QString("SELECT * FROM t_%1 ORDER BY id ASC;").arg(streamId);
    }

    bool state = qry.exec(sql);

    if(state) {
        while(qry.next()) {
            MessageData msg;
            msg.id = qry.value("id").toULongLong();
            msg.type = qry.value("type").toInt();
            msg.sender = qry.value("sender").toString();
            msg.nik_color = qry.value("nik_color").toString();
            msg.msg = qry.value("msg").toString();
            msg.pay = qry.value("pay").toFloat();
            msg.timestamp = qry.value("timestamp").toDateTime();
            msgList.append(msg);
        }
    }

    return state;
}

bool CCBot::appendMsgIntoTableDB(QString streamId, QList<MessageData> &msgList)
{
    if (msgList.isEmpty()) {
        return true;
    }

    QDateTime timestamp = QDateTime::currentDateTime();

    for (int i = 0; i < msgList.size(); i++) {
        QSqlQuery qry;
        MessageData msg = msgList.at(i);
        msgList[i].timestamp = timestamp;
        QString sql = QString("INSERT INTO t_%1 (type, sender, nik_color, msg, pay, timestamp) VALUES (:type, :sender, :nik_color, :msg, :pay, :timestamp);").arg(streamId);
        qry.prepare(sql);
        qry.bindValue(":type", msg.type);
        qry.bindValue(":sender", msg.sender);
        qry.bindValue(":nik_color", msg.type == 1 ? "#fff200" : msg.nik_color);
        qry.bindValue(":msg", msg.msg);
        qry.bindValue(":pay", msg.pay);
        qry.bindValue(":timestamp", timestamp.toString("yyyy-MM-dd hh:mm:ss"));
        qry.exec();
    }

    return false;
}

int CCBot::insertNewMessagesInTable(QString streamId, QByteArray jsonData, bool merge, QString *errInfo)
{
    QList<MessageData> rowsFromDB;
    QList<MessageData> rowsFromServer;
    QList<MessageData> rowsForInsert;

    bool tableNotExist = false;
    bool state = false;

    // 0. Упаковка данных с CrazyCash
    state = readMessagesFromJsonStr(jsonData, rowsFromServer, errInfo);
    if(!state) {
        return CCBotErrEnums::ParseJson;
    }

    // 1. Проверка что таблица есть, иначе создать ее
    if (!existsTableDB(streamId)) {
        tableNotExist = true;
        if (!createTableDB(streamId)) {
            if (errInfo) {
                *errInfo = m_db.lastError().text();
            }
            return CCBotErrEnums::Sql;
        }
    }

    // Fix remove type 4
    int removeCount = 0;
    for (int i = 0; i < rowsFromServer.size(); i++) {
        if (rowsFromServer.at(i).type == 4) {
            rowsFromServer.removeAt(i--);
            ++removeCount;
        }
    }

    // 2. Запрос 100 сообщений с таблицы
    if(!tableNotExist) {
        state = selectMsgsFromTableDB(streamId, rowsFromDB, merge ? 100 : -1);
//    if (!state) {
//        if (errInfo) {
//            *errInfo = m_db.lastError().text();
//        }
//        return CCBotErrEnums::Sql;
//    }
    }

    // 3. Слияние
    if(!tableNotExist) {
        mergeMessages(!merge ? rowsFromDB : listRight<MessageData>(rowsFromDB, 100), rowsFromServer, rowsForInsert);
    } else {
        rowsForInsert.append(rowsFromServer);
    }

    // 4. Вставка новых сообщений в БД
    state = appendMsgIntoTableDB(streamId, rowsForInsert);
//    if(!state) {
//        if (errInfo) {
//            *errInfo = m_db.lastError().text();
//        }
//        return CCBotErrEnums::Sql;
//    }

    // 5. Обновление чата
    updateChat(merge ? rowsForInsert : rowsFromDB + rowsForInsert);

    // 6. Анализ сообщений на комманды -> выполнение комманд (добавление задач)
    if(merge) {
        analyseNewMessages(rowsForInsert);
    }

    return CCBotErrEnums::Ok;
}

void CCBot::mergeMessages(QList<MessageData> oldMsgList, QList<MessageData> newMsgList, QList<MessageData> &mergedMsgList)
{
    if (oldMsgList.isEmpty()) {
        mergedMsgList.append(newMsgList);
        return;
    }

    QStringList type3NikNames;

    // Поиск ников помеченных как тип 3 для игнорирования в сравнении списков
    foreach (const MessageData &msg, newMsgList) {
        if(msg.type == 3 && !type3NikNames.contains(msg.sender)) {
            type3NikNames.append(msg.sender);
        }
    }

    // Удаление из сравнения сообщений с ником из списка
    for (const auto &nik : type3NikNames) {
        for (int i = 0; i < oldMsgList.size(); i++) {
            if (oldMsgList.at(i).sender == nik) {
                oldMsgList.removeAt(i--);
            }
        }
        for (int i = 0; i < newMsgList.size(); i++) {
            if (newMsgList.at(i).sender == nik) {
                newMsgList.removeAt(i--);
            }
        }
    }

    // Поиск интервалов похожести для выбора наилучшего слияния списка (выбирается интервал с большим весом - weight)
    QList<MessageData>::const_reverse_iterator iOldMsgs;
    QList<MessageData>::const_reverse_iterator iNewMsgs;
    QList<QPair<int,int> > intervals;
    int start = -1;
    int weight = 0;
    int spaceMsgCount = 0;
    bool flagEnterInterval = false;
    for (iNewMsgs = newMsgList.crbegin(), iOldMsgs = oldMsgList.crbegin(); iNewMsgs != newMsgList.crend() && iOldMsgs != oldMsgList.crend(); ) {
        if (equalMessages(*iNewMsgs, *iOldMsgs)) {
            if (!flagEnterInterval) {
                start = iNewMsgs - newMsgList.crbegin();
                weight = 1;
                flagEnterInterval = true;
            } else {
                weight += 1;
            }
            ++iOldMsgs;
            ++iNewMsgs;
            if (iOldMsgs == oldMsgList.crend()) {
                intervals.append(QPair<int,int>(start, weight));
                break;
            }
            if (iNewMsgs == newMsgList.crend()) {
                intervals.append(QPair<int,int>(start, weight));
                iOldMsgs -= (weight + spaceMsgCount);
            }
        } else {
            if (flagEnterInterval) {
                intervals.append(QPair<int,int>(start, weight));
                flagEnterInterval = false;
                iOldMsgs -= (weight + spaceMsgCount);
                spaceMsgCount = 0;
            }
            ++iNewMsgs;
        }
    }

    // Выбор интервала
    QPair<int,int> maxInterval = QPair<int,int>(-1, 0);
    for (const auto &interval : intervals) {
        if (interval.second > maxInterval.second) {
            maxInterval = interval;
        }
    }

    // Спам-пакет из пачки сообщений т.к. не найдено совпадений вообще! (если такое возможно) -> передаем его сразу в запись
    if (maxInterval.first == -1) {
        mergedMsgList = newMsgList;
        //qDebug() << "spam!";
        return;
    }

    // Отсутствуют новые сообщения!
    if (maxInterval.first == 0) {
        //qDebug() << "empty new";
        return;
    }

    // Записываем новые сообщения в список слияния
    int startIndex = newMsgList.size() - maxInterval.first;
    //qDebug() << "start_index: " << startIndex;
    mergedMsgList.append(newMsgList.mid(startIndex));
}

bool CCBot::equalMessages(const MessageData &msg1, const MessageData &msg2)
{
    if (msg1.type == msg2.type
            && msg1.sender == msg2.sender
            && qFuzzyCompare(msg1.pay, msg2.pay)
            && msg1.msg == msg2.msg
            ) {
        return true;
    }
    return false;
}

void CCBot::updateChat(const QList<MessageData> &msgsl, bool withTime, QString timeFormat)
{
    for (int i = 0; i < msgsl.size(); i++) {
        MessageData msg = msgsl.value(i);
        QString timeStr = msg.timestamp.toString(timeFormat);
        QString fragment0 = withTime ? timeStr + ": " : "";
        QString nikStr = msg.sender;
        QString fragment1 = nikStr.isEmpty() ? "" : msg.sender + ": ";
        QString fmtFragment1 = msg.nik_color.isEmpty() ? fragment1 : _clr_(fragment1, msg.nik_color);
        QString fragment2 = msg.msg;
        QString fmtFragment2 = fragment2.isEmpty() ?
                    (msg.type == 2 ? _bclr_((QString("($") + QString::number(msg.pay, 'f', 2) + ")"), "#fff200") : "")
                    :
                    (msg.type == 2 ?
                         _bclr_((fragment2 + " ($" + QString::number(msg.pay, 'f', 2) + ")"), "#fff200") :
                         fragment2);
        QString msgStr = fragment0 + fmtFragment1 + fmtFragment2;

        emit showChatMessage(msgStr);
    }
}

void CCBot::analyseNewMessages(const QList<MessageData> &msgsl)
{
    for (int i = 0; i < msgsl.size(); i++) {
        MessageData msg = msgsl.at(i);
        QString text = "";
        //clearUselessSymbols(msg.msg);
        if (checkAutoVoiceMessage(msg, text)) {
            m_pCore->addTask(CCBotTaskEnums::VoiceLoad, text);
        }
    }
}

bool CCBot::checkAutoVoiceMessage(const MessageData &msg, QString &text)
{
    if (msg.msg.isEmpty()) {
        return false;
    }
    if ((m_params->flagAnalyseVoiceAllMsgType2() && msg.type == 2)
            || (m_params->flagAnalyseVoiceAllMsgType0() && msg.type == 0)
            ) {
        QString analyseText = msg.msg;
        analyseText = analyseText.remove(QRegularExpression("[\\x{1F600}-\\x{1F7FF}]+"));
        analyseText.replace("Zhivana", "Джиганна");
        // проверка на комманду
//        if (analyseText.at(0) == QChar('!')) {
//            return false;
//        }
        // проверка на пустое сообщение
        bool emptyMsg = true;
        for (int i = 0; i < analyseText.length(); i++) {
            if(analyseText.at(i).isLetter()) {
                emptyMsg = false;
                text = analyseText;
                break;
            }
        }
        return !emptyMsg;
    }
    return false;
}

bool CCBot::checkCmdMessage(const MessageData &msg, QString &cmd, QStringList &args)
{
    Q_UNUSED(msg)
    Q_UNUSED(cmd)
    Q_UNUSED(args)
    //
    return false;
}

void CCBot::action(int type, QVariantList args)
{
    switch (type) {
    case CCBotTaskEnums::MergeChat:
        {
            QString streamId = args.value(0,"").toString();
            QString messagesJsonStr = args.value(1,"").toString();
            bool loading = args.value(2, false).toBool();
            m_pCore->addTask(type, streamId, messagesJsonStr, loading);
        }
        break;
    case CCBotTaskEnums::VoiceLoad:
        {
            QString text = args.value(0,"").toString();
            if(!text.isEmpty()) {
                m_pCore->addTask(type, text);
            }
        }
        break;
    case CCBotTaskEnums::OpenBase:
        {
            openDB();
        }
        break;
    case CCBotTaskEnums::CloseBase:
        {
            closeDB();
        }
        break;
    default:
        break;
    }
}

void CCBot::slotFinishedTask(long id, int type, QVariantList argsList, QVariant result)
{
    Q_UNUSED(id)
    Q_UNUSED(argsList)

    auto taskResult = result.value<TaskResult>();
    int errCode = taskResult.errCode();
    QString errInfo = taskResult.errInfo();

    if (errCode != CCBotErrEnums::Ok) {
        emit showMessage(tr("Ошибка"), generateErrMsg(type, errCode, errInfo), true);
    }
    else
        switch (type) {
        case CCBotTaskEnums::MergeChat:
            {
                bool loading = argsList.value(2, false).toBool();
                if(loading) {
                    m_params->setFlagLoadingChat(false);
                }
            }
            break;
        default:
            break;
        }
}
