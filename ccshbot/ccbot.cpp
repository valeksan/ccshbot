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

CCBot::CCBot(Properties *params, QObject *parent) : CCBotPrivate(parent)
{    
    m_params = params;

    m_pSpeechKitTTS = new SpeechkitTTS(this);
    m_pVoicePlayer = new QMediaPlayer(this);
    m_pManagerTTS = new TTSManager(this);

    loadSettings();

    m_params->setIsActivated(Cicero::verifyActivation(m_params->actKey().toLatin1()));
}

void CCBot::start()
{
    const QStringList args = qApp->arguments();
    m_params->setFlagLogging(args.contains("--log") || args.contains("-l"));

    if (m_params->flagLogging()) {
        if (!startLog()) {
            emit showMessage("Ошибка", "failed to create log file!", true);
            m_params->setFlagLogging(false);
        }
    }

    initComponents();
    initSysCommands();
    initConnections();
    initTasks();

    if (!m_params->isActivated()) {
        emit showTrialDlg();
    }
}

void CCBot::initDatabase()
{
    if (!QSqlDatabase::drivers().contains("QSQLITE")) {
        if (m_params->flagLogging()) {
            addToLog("ERROR! Unable to load database, needs the SQLITE driver!");
        }
        emit showMessage("Ошибка", "Unable to load database.\nNeeds the SQLITE driver!", true);
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE");
    }
}

void CCBot::loadSettings()
{
    QSettings cfg;

    cfg.beginGroup("App");
    m_params->setActKey(cfg.value("ActivationKey","").toString());
    cfg.endGroup();

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
    m_params->setTextColorForChat(cfg.value("TextColorForChat", QColor(0xBF,0xC7,0xD0)).value<QColor>());
    cfg.endGroup();

    cfg.beginGroup("Server");
    m_params->setMaxTimestampDiff(cfg.value("MaxTDiff", 5U).toUInt());
    m_params->setListenHost(cfg.value("Host", "127.0.0.1").toString());
    m_params->setListenPort(cfg.value("Port", 3000U).toUInt());
    cfg.endGroup();

    cfg.setValue("OptionSpeakReasonType", m_params->speakOptionReasonType());
    cfg.beginGroup("SpeechKit");
    m_params->setSpeechKitPriceBySymbol(cfg.value("PriceSymbol", 0.0).toDouble());
    m_params->setSpeechkitFolderId(cfg.value("FolderID", "").toString());
    m_params->setSpeechkitOAuthToken(cfg.value("OAuthToken", "").toString());
    m_params->setSpeechkitHost(
                cfg.value("ExtHost", "").toString());
    m_params->setSpeechkitGetIamTokenHost(
                cfg.value("ExtIamTokenHost", "").toString());
    m_params->setSpeechkitLang(
                cfg.value("DefLang", "").toString());
    m_params->setSpeechkitFormat(
                cfg.value("DefFormat", "").toString());
    m_params->setSpeechkitVoice(
                cfg.value("DefVoice", "").toString());
    m_params->setSpeechkitEmotion(
                cfg.value("DefEmotion", "").toString());
    m_params->setSpeechkitSpeed(
                cfg.value("DefSpeed", "").toString());
    m_params->setSpeechkitSampleRateHertz(
                cfg.value("DefSampleRateHertz", "").toString());
    m_params->setSpeakOptionReasonType(
                cfg.value("OptionSpeakReasonType",
                          SpeakReasonEnums::DisableAll).toInt());
    cfg.endGroup();

    cfg.beginGroup("ToReplaceForVoice");
    m_dataToReplaceTextForVoice = QJsonDocument::fromJson(cfg.value("data","[]").toByteArray());
    cfg.endGroup();

    cfg.beginGroup("Box");
    m_params->setBoxUserStartingBalance(cfg.value("UserStartingBalance", 0.0).toDouble());
    m_params->setBoxDefaultOnFlag0(cfg.value("DefaultOnFlag0", false).toBool());
    m_params->setBoxNotificationChatByEmptyUserBalanceForVoice(
                cfg.value("NotificationChatByEmptyUserBalanceForVoice", false).toBool());
    cfg.endGroup();

    cfg.beginGroup("Optimise");
    {
        // list Type3 Senders fo remove
        QJsonObject jsonSenders = QJsonDocument::fromJson(cfg.value("listType3Senders").toByteArray()).object();
        QJsonObject jsonExpire = QJsonDocument::fromJson(cfg.value("listType3SendersExpire").toByteArray()).object();
        QStringList keys = jsonSenders.keys();
        QDateTime currentDT = QDateTime::currentDateTime();
        for (int i = 0; i < keys.size(); i++) {
            QString key = keys.at(i);
            QDateTime expire = QDateTime::fromString(jsonExpire.value(key).toString(), Qt::ISODate);
            if (expire > currentDT) {
                m_mapListType3SendersOld.insert(key, jsonArrToStringList(jsonSenders.value(key).toArray()));
                m_mapListType3SendersOldExpire.insert(key, jsonExpire.value(key).toString());
            }
        }
    }
    cfg.endGroup();

    // load saved command buffer stack
    m_consoleInput->loadCommandBufferStackSaves();
}

void CCBot::saveSettings(quint32 section, bool beforeExit)
{
    QSettings cfg;

    if ((section & SaveSectionEnums::App) == SaveSectionEnums::App) {
        cfg.beginGroup("App");
        cfg.setValue("ActivationKey", m_params->actKey());
        cfg.endGroup();
    }
    if ((section & SaveSectionEnums::Window) == SaveSectionEnums::Window) {
        cfg.beginGroup("Window");
        cfg.setValue("X", m_params->windowX());
        cfg.setValue("Y", m_params->windowY());
        cfg.setValue("HEIGHT", m_params->windowHeight());
        cfg.setValue("WIDTH", m_params->windowWidth());
        cfg.endGroup();
    }
    if ((section & SaveSectionEnums::View) == SaveSectionEnums::View) {
        cfg.beginGroup("View");
        cfg.setValue("FontNameForChat", m_params->fontNameForChat());
        cfg.setValue("FontPointSizeForChat", m_params->fontPointSizeForChat());
        cfg.setValue("TextColorForChat", m_params->textColorForChat());
        cfg.endGroup();
    }
    if ((section & SaveSectionEnums::Server) == SaveSectionEnums::Server) {
        cfg.beginGroup("Server");
        cfg.setValue("MaxTDiff", m_params->maxTimestampDiff());
        cfg.setValue("Host", m_params->listenHost());
        cfg.setValue("Port", m_params->listenPort());
        cfg.endGroup();
    }
    if ((section & SaveSectionEnums::SpeechKit) == SaveSectionEnums::SpeechKit) {
        cfg.beginGroup("SpeechKit");
        cfg.setValue("PriceSymbol", m_params->speechKitPriceBySymbol());
        cfg.setValue("FolderID", m_params->speechkitFolderId());
        cfg.setValue("OAuthToken", m_params->speechkitOAuthToken());
        cfg.setValue("ExtHost", m_params->speechkitHost());
        cfg.setValue("ExtIamTokenHost", m_params->speechkitGetIamTokenHost());
        cfg.setValue("DefLang", m_params->speechkitLang());
        cfg.setValue("DefFormat", m_params->speechkitFormat());
        cfg.setValue("DefVoice", m_params->speechkitVoice());
        cfg.setValue("DefEmotion", m_params->speechkitEmotion());
        cfg.setValue("DefSpeed", m_params->speechkitSpeed());
        cfg.setValue("DefSampleRateHertz", m_params->speechkitSampleRateHertz());
        cfg.setValue("OptionSpeakReasonType", m_params->speakOptionReasonType());
        cfg.endGroup();
        if (!beforeExit) {
            initSpeechkitTts();
        }
    }
    if ((section & SaveSectionEnums::ToReplaceForVoice) == SaveSectionEnums::ToReplaceForVoice) {
        cfg.beginGroup("ToReplaceForVoice");
        cfg.setValue("data", m_dataToReplaceTextForVoice.toJson(QJsonDocument::Compact));
        cfg.endGroup();
    }
    if ((section & SaveSectionEnums::Box) == SaveSectionEnums::Box) {
        cfg.beginGroup("Box");
        cfg.setValue("UserStartingBalance", m_params->boxUserStartingBalance());
        cfg.setValue("DefaultOnFlag0", m_params->boxDefaultOnFlag0());
        cfg.setValue("NotificationChatByEmptyUserBalanceForVoice",
                     m_params->boxNotificationChatByEmptyUserBalanceForVoice());
        cfg.endGroup();
    }
    if ((section & SaveSectionEnums::Optimise) == SaveSectionEnums::Optimise) {
        cfg.beginGroup("Optimise");
        {
            // list Type3 Senders fo remove
            QJsonObject json;
            QJsonObject jsonExpire;
            QJsonObject jsonExpireOld = QJsonDocument::fromJson(cfg.value("listType3SendersExpire").toByteArray()).object();
            QString expireStrDate = QDateTime::currentDateTime().addDays(2).toString(Qt::ISODate);
            QMapIterator<QString, QStringList> i(m_mapListType3SendersOld);
            while (i.hasNext()) {
                i.next();
                json.insert(i.key(), QJsonArray::fromStringList(i.value()));
                if (!jsonExpireOld.contains(i.key()))
                        jsonExpire.insert(i.key(), expireStrDate);
            }
            QJsonDocument doc;
            QJsonDocument docExpire;
            doc.setObject(json);
            docExpire.setObject(jsonExpire);
            cfg.setValue("listType3Senders", doc.toJson());
            cfg.setValue("listType3SendersExpire", docExpire.toJson());
        }
        cfg.endGroup();
    }

    //...

    if (m_params->flagLogging()) {
        m_log.endLogSession();
    }

    if ((section & SaveSectionEnums::CommandBuffer) == SaveSectionEnums::CommandBuffer) {
        // save command buffer stack
        m_consoleInput->saveCommandBufferStack();
    }
}

const QString CCBot::getRegistrationCode()
{
    return Cicero::makeRegistrationKey();
}

const QString CCBot::getActivationCode()
{
    return m_params->actKey();
}

void CCBot::setActivationCode(QString keyFmt)
{
    m_params->setActKey(keyFmt);
    saveSettings(SaveSectionEnums::App);
}

bool CCBot::verifyActivation()
{
    return Cicero::verifyActivation(m_params->actKey().toLatin1());
}

void CCBot::initSpeechkitTts()
{
    TTSManager::SpeechKitConfig speechkitCfg = getSpeechkitConfig();
    m_pManagerTTS->initTTS(TTSManager::SpeechKit, QVariant::fromValue(speechkitCfg));
}

void CCBot::initComponents()
{
    // Database
    initDatabase();
    // Players:
    //...
    // TTSs:
    // - speechkit (Yandex)
    initSpeechkitTts();
}

void CCBot::initConnections()
{
    // номер стрима был изменен
    connect(m_params, &Properties::currentStreamIdChanged, [=]() {
        m_mapSubscribeUserNotified.clear();
    });

    // components
    // -- tts speechkit
//    connect(m_pSpeechKitTTS, &SpeechkitTTS::voiceComplete, [this](QString filename) {
//        m_pCore->addTask(CCBotTaskEnums::VoiceSpeech, filename);
//    });
//    connect(m_pSpeechKitTTS, &SpeechkitTTS::voiceFail, [this](int type, const QString info) {
//        qDebug() << type << info;
//        QString title;
//        switch (type) {
//        case SpeechkitTTS::NetworkError:
//            title = tr("Ошибка сети");
//            break;
//        case SpeechkitTTS::ServiceError:
//            title = tr("Ошибка сервиса SpeechKit TTS");
//            break;
//        case SpeechkitTTS::SystemError:
//            title = tr("Ошибка доступа");
//            break;
//        default:
//            break;
//        }
//        emit showMessage(title, info, true);
//    });
    // -- voice player
//    connect(m_pVoicePlayer,
//            QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error),
//            [=](QMediaPlayer::Error error) {
//        const QString errStr = QString("Media Player error(%1): ").arg(error)
//                + m_pVoicePlayer->errorString();
//        qDebug() << errStr;
//        if (m_params->flagLogging()) {
//            addToLog(errStr);
//        }
//    });
//    connect(m_pVoicePlayer,
//            &QMediaPlayer::stateChanged,
//            [this](QMediaPlayer::State state)
//    {
//        // соединение: конец проигрывания файла
//        if (state == QMediaPlayer::StoppedState) {
//            emit completePlayFile();
//        }
//    });

    // соединение: запуск комманд через консоль
    connect(m_consoleInput, &Console::runCommand, [&, this](QString sender, QString command, QStringList args) {
        int type = m_consoleInput->getType(command);
        QString fullCommand = command + (args.isEmpty() ? "" : QString(" ") + args.join(" "));
        bool isValidCommand = false;
        bool isStreamer = sender.toUpper() == m_params->currentStreamerNikname();
        QString target = "";
        qDebug() << "type cmd:" << type;

        if (isStreamer) {
            for (int i = 0; i < args.size(); i++) {
                QString option = args.at(i).section('=', 0, 0);
                if (option == "target") {
                    QString value = args.at(i).section('=', -1, -1);
                    target = value;
                    break;
                }
            }
        } else {
            target = sender;
        }

        switch (type) {
        case CCBotTaskEnums::SysCmdDrink:
            isValidCommand = commandDrink(args, target, isStreamer);
            break;
        case CCBotTaskEnums::SysCmdSetVoice:
            isValidCommand = commandVoice(args, target, isStreamer);
            break;
        default:
            break;
        }
        if (!isValidCommand) {
            QString notifyMsg = QString("#") + fullCommand + " - no valid command!";
            if (isStreamer) {
                emit showChatNotification(_clr_(notifyMsg, "red"));
            } else {
                emit sendChatMessage(notifyMsg);
            }
        }
    });
}

void CCBot::initSysCommands()
{
    m_consoleInput->registerTaskTypeAlias("!voice", CCBotTaskEnums::SysCmdSetVoice); // установка голоса
    m_consoleInput->registerTaskTypeAlias("!drink", CCBotTaskEnums::SysCmdDrink);    // установка темпа голоса на время
    m_consoleInput->registerTaskTypeAlias("!balance", CCBotTaskEnums::SysCmdBalance); // вывести баланс собеседника
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

//    m_pCore->registerTask(CCBotTaskEnums::VoiceLoad,
//                          [this](QString text, SpeakOptions options) -> TaskResult {
//        TaskResult result;
//        QNetworkAccessManager *manager = new QNetworkAccessManager();
//        m_errorsSsl.clear();
//        m_errType = QNetworkReply::NoError;

//        // 1. Проверка что токен истек
//        bool tokenExpiry = (QDateTime::currentDateTime() >= m_params->speechkitIamTokenExpiryDate());
//        if (tokenExpiry) {
//            // 1.1 Если токен уже не действителен то:
//            // получаем новый и обновляем дату
//            // * формируем запрос
//            QNetworkRequest requestGetIamToken;
//            requestGetIamToken.setUrl(
//                        QUrl(m_params->speechkitGetIamTokenHost()));
//            requestGetIamToken.setHeader(
//                        QNetworkRequest::ContentTypeHeader,
//                        "application/json");
//            QJsonObject obj;
//            obj["yandexPassportOauthToken"] = m_params->speechkitOAuthToken();
//            QJsonDocument doc(obj);
//            QByteArray data = doc.toJson(QJsonDocument::Compact);

//            // * делаем запрос
//            manager->setTransferTimeout(5000);
//            QNetworkReply *reply = manager->post(requestGetIamToken, data);

//            connect(reply,
//                    &QNetworkReply::finished,
//                    this,
//                    [&]()
//            {
//                QByteArray response = reply->readAll();
//                QJsonObject responseObj =
//                        QJsonDocument::fromJson(response).object();
//                m_params->setSpeechkitIamToken(
//                            responseObj.value("iamToken").toString());
//                m_params->setSpeechkitIamTokenExpiryDate(
//                            QDateTime::fromString(
//                                responseObj.value("expiresAt").toString(),
//                                Qt::ISODateWithMs)
//                            );
//                emit completeRequestGetIamToken();
//            });

//            connect(reply,
//                    &QNetworkReply::errorOccurred,
//                    this,
//                    [this](QNetworkReply::NetworkError error)
//            {
//                m_errType = error;
//                if (m_params->flagLogging()) {
//                    const QString errStr = QString("Error network reply(%1) - ")
//                            .arg(error);
//                    addToLog(errStr);
//                }
//                emit completeRequestGetIamToken();
//            });

//            connect(reply,
//                    &QNetworkReply::sslErrors,
//                    this,
//                    [this](const QList<QSslError> &errors)
//            {
//                m_errorsSsl = errors;
//                if (m_params->flagLogging()) {
//                    QString errorsText = "";
//                    for (const auto &err: errors) {
//                        errorsText.append(err.errorString()) + "; ";
//                    }
//                    QString errStr = QString("Error network reply ssl.")
//                            + "\nErrors: " + errorsText;
//                    addToLog(errStr);
//                }
//                emit completeRequestGetIamToken();
//            });

//            bool timeout = !waitSignal(this,
//                                       &CCBot::completeRequestGetIamToken,
//                                       5000 + 500);
//            reply->disconnect();

//            if (timeout) {
//                result = TaskResult(CCBotErrEnums::NetworkRequest,
//                                    "Timeout request, not get iam-token!");
//                delete manager;
//                return result;
//            }

//            if (m_errType != QNetworkReply::NoError || !m_errorsSsl.isEmpty()) {
//                QString mainInfo = QString("Error request (")
//                        + QString::number(static_cast<int>(m_errType))
//                        + "), ";
//                QString secondInfo =
//                        m_errorsSsl.isEmpty() ?
//                            QString("no ssl errors") :
//                            QString("%1 ssl errors").arg(m_errorsSsl.size());
//                result = TaskResult(CCBotErrEnums::NetworkRequest,
//                                    mainInfo + secondInfo);
//                delete manager;
//                return result;
//            }
//        }

//        // 2. Запрос звукового файла с сервера на текст
//        // повторная проверка даты токена
//        tokenExpiry = (QDateTime::currentDateTime() >= m_params->speechkitIamTokenExpiryDate());
//        if (!tokenExpiry) {
//            QNetworkRequest requestGetAudio;
//            QUrl url(m_params->speechkitHost());
//            QUrlQuery postDataEncoded;

//            // * add header
//            requestGetAudio.setHeader(QNetworkRequest::ContentTypeHeader,
//                                      "application/x-www-form-urlencoded");
//            requestGetAudio.setRawHeader("Authorization",
//                                         QString("Bearer %1")
//                                         .arg(m_params->speechkitIamToken())
//                                         .toUtf8());
//            // * add data
//            text.replace(';', "%3B");
//            postDataEncoded.addQueryItem("text", text.replace(' ', '+'));

//            postDataEncoded.addQueryItem("folderId",
//                                         m_params->speechkitFolderId());

//            QString voice = options.voice.isEmpty() ?
//                        m_params->speechkitVoice() : options.voice;
//            QString emotion = options.emotion.isEmpty() ?
//                        m_params->speechkitEmotion() : options.emotion;
//            QString speed = options.speed.isEmpty() ?
//                        m_params->speechkitSpeed() : options.speed;

//            if (!voice.isEmpty()) {
//                postDataEncoded.addQueryItem("lang", getLangByVoiceName(voice));
//                postDataEncoded.addQueryItem("voice", voice);
//            }

//            if (!emotion.isEmpty()) {
//                postDataEncoded.addQueryItem("emotion", emotion);
//            }
//            if (!speed.isEmpty()) {
//                postDataEncoded.addQueryItem("speed", speed);
//            }
//            if (!m_params->speechkitFormat().isEmpty()) {
//                postDataEncoded.addQueryItem("format",
//                                             m_params->speechkitFormat());
//            }
//            if (!m_params->speechkitSampleRateHertz().isEmpty()) {
//                postDataEncoded.addQueryItem("sampleRateHertz",
//                                      m_params->speechkitSampleRateHertz());
//            }

//            // * запрос
//            // * делаем запрос
//            manager->setTransferTimeout(25000);
//            requestGetAudio.setUrl(url);
//            QNetworkReply *reply = manager->post(requestGetAudio,
//                                                 postDataEncoded
//                                                  .toString(QUrl::FullyEncoded)
//                                                  .toUtf8());

//            m_errType = QNetworkReply::NoError;
//            m_errorsSsl.clear();

//            connect(reply, &QNetworkReply::finished, this, [this, reply]() {
//                QByteArray response = reply->readAll();
//                if (QJsonDocument::fromJson(response).isObject()) {
//                    if (m_params->flagLogging()) {
//                        const QString errStr = QString("Error speechkit service.")
//                                + "\nResponse: " + QString::fromUtf8(response);
//                        addToLog(errStr);
//                    }
//                } else {
//                    // сохраняем файл на диске
//                    // и передаем новой задачи имя файла
//                    QTemporaryFile tmpfile;
//                    if (tmpfile.open()) {
//                        tmpfile.setAutoRemove(false);
//                        QDataStream ostream(&tmpfile);
//                        ostream.writeRawData(response, response.size());
//                        tmpfile.close();
//                        m_pCore->addTask(CCBotTaskEnums::VoiceSpeech,
//                                         tmpfile.fileName());
//                    }
//                }
//                emit completeRequestGetAudio();
//            });

//            connect(reply,
//                    &QNetworkReply::errorOccurred,
//                    this,
//                    [this](QNetworkReply::NetworkError error)
//            {
//                m_errType = error;
//                if (m_params->flagLogging()) {
//                    const QString errStr = QString("Error network reply(%1) - ")
//                            .arg(m_errType);
//                    addToLog(errStr);
//                }
//                emit completeRequestGetAudio();
//            });

//            connect(reply,
//                    &QNetworkReply::sslErrors,
//                    this,
//                    [this](const QList<QSslError> &errors)
//            {
//                m_errorsSsl = errors;
//                if (m_params->flagLogging()) {
//                    QString errorsText = "";
//                    for (const auto &err: errors) {
//                        errorsText.append(err.errorString()) + "; ";
//                    }
//                    QString errStr = QString("Error network reply ssl.")
//                            + "\nErrors: " + errorsText;
//                    addToLog(errStr);
//                }
//                emit completeRequestGetAudio();
//            });

//            bool timeout = !waitSignal(this,
//                                       &CCBot::completeRequestGetAudio,
//                                       25000 + 500);
//            reply->disconnect();

//            if (timeout) {
//                result =
//                        TaskResult(CCBotErrEnums::NetworkRequest,
//                                   "Timeout request, not get audio!");
//                delete manager;
//                return result;
//            }
//            if (m_errType != QNetworkReply::NoError ||
//                    !m_errorsSsl.isEmpty())
//            {
//                QString mainInfo =
//                        QString("Error request (")
//                        + QString::number(static_cast<int>(m_errType))
//                        + "), ";
//                QString secondInfo =
//                        m_errorsSsl.isEmpty() ?
//                            QString("no ssl errors") :
//                            QString("%1 ssl errors").arg(m_errorsSsl.size());
//                result = TaskResult(CCBotErrEnums::NetworkRequest,
//                                    mainInfo + secondInfo);
//                delete manager;
//                return result;
//            }
//        }
//        // 3. Завершение
//        delete manager;
//        return result;
//    }, 1);

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

TTSManager::SpeechKitConfig CCBot::getSpeechkitConfig()
{
    TTSManager::SpeechKitConfig config;
    config.extHost = m_params->speechkitHost();
    config.extHostGetImToken = m_params->speechkitGetIamTokenHost();
    config.folderId = m_params->speechkitFolderId();
    config.format = m_params->speechkitFormat();
    config.sampleRateHertz = m_params->speechkitSampleRateHertz();
    config.tokenOauth = m_params->speechkitOAuthToken();
    return config;
}

void CCBot::speechFile(QString filename)
{
    waitSignalAfterFunction(this,
                            &CCBot::completePlayFile,
                            [&filename, this]()
    {
        m_pVoicePlayer->setMedia(QUrl::fromLocalFile(filename));
        m_pVoicePlayer->play();
    });
}

void CCBot::openLogDir()
{
    m_log.openLogDir();
}

bool CCBot::openDB(QString name)
{
    QString path = QStandardPaths::writableLocation(
                QStandardPaths::AppDataLocation);

    if (path.isEmpty()) {
        qDebug() << "Cannot determine settings storage location";
        path = QDir::homePath() + QDir::separator() + ".ccbot";
    }

    QDir d{path};

    if (!d.exists()) {
        if (!d.mkpath(d.absolutePath())) {
            qDebug() << QString("Can't to create path: %1").arg(d.absolutePath());
            return false;
        }
    }

    QString filePath = path + QDir::separator() + (name.isEmpty() ? constNameBaseStr : name);

    m_db.setDatabaseName(filePath);
    if (!m_db.open()) {
        emit showMessage("Ошибка", QString("Не удалось открыть базу.\n") + m_db.lastError().text(), true);
        return false;
    }

    return true;
}

QStringList CCBot::getModelAvaibleHistoryNiknames()
{
    QDir dirAppData{getAppDataDirPath()};
    auto listNiknamesWithSuffix = dirAppData.entryInfoList({"*.db"}, QDir::Files, QDir::Name);
    QStringList result{""};

    for (const auto &name : listNiknamesWithSuffix) {
        result.append(name.completeBaseName());
    }

    return result;
}

QStringList CCBot::getModelAvaibleHistoryStreamsByNikname(QString nikname)
{
    QStringList tmpList;
    QStringList result{""};

    QString baseName = nikname + ".db";
    QString prevOpennedBase = isOpenedDB() ? m_db.databaseName() : "";

    m_mutex.lock();

    if (!prevOpennedBase.isEmpty()) {
        m_db.close();
    }

    bool state = openDB(baseName);

    if (m_params->flagLogging() && !state) {
        QString info = QString("Error open history base (%1). Can't open %2 database!")
                .arg(m_db.lastError().nativeErrorCode(), baseName);
        addToLog(info);
    }

    if (state) {
        tmpList.append(m_db.tables().filter("t_"));
        m_db.close();
    }

    if (!prevOpennedBase.isEmpty()) {
        openDB(prevOpennedBase);
    }

    m_mutex.unlock();

    for (auto& id : tmpList) {
        result.append(id.remove("t_"));
    }

    result.sort();

    return result;
}

void CCBot::displayChatHistory(QString nikname, QString streamId)
{
    QString baseName = nikname + ".db";
    QString prevOpennedBase = isOpenedDB() ? m_db.databaseName() : "";

    m_mutex.lock();

    if (!prevOpennedBase.isEmpty()) {
        m_db.close();
    }

    bool state = openDB(baseName);

    if (m_params->flagLogging() && !state) {
        QString info = QString("Error open history base (%1). Can't open %2 database!")
                .arg(m_db.lastError().nativeErrorCode(), baseName);
        addToLog(info);
    }

    if (state) {
        QList<MessageData> messages{};
        state = selectMsgsFromTableDB(streamId, messages);
        if (state) {
            updateChat(messages, true, "hh:mm", true);
        }
        m_db.close();
    }

    if (!prevOpennedBase.isEmpty()) {
        openDB(prevOpennedBase);
    }

    m_mutex.unlock();
}

bool CCBot::isOpenedDB()
{
    return m_db.isOpen();
}

void CCBot::closeDB()
{
    m_db.close();
}

int CCBot::insertNewMessagesInTable(QString streamId, QByteArray jsonData, bool mergeOnly, QString *errInfo)
{
    QList<MessageData> rowsFromDB;
    QList<MessageData> rowsFromServer;
    QList<MessageData> rowsForInsert;

    bool state = false;

    // 0. Упаковка данных с CrazyCash
    state = readMessagesFromJsonStr(jsonData, rowsFromServer, errInfo);
    if(!state) {
        return CCBotErrEnums::ParseJson;
    }

    // 1. Проверка что таблица есть, иначе создать ее
    bool newTableCreated = false;
    if (!existsTableDB(streamId)) {
        if (!createTableDB(streamId)) {
            if (errInfo) {
                *errInfo = m_db.lastError().text();
            }
            return CCBotErrEnums::Sql;
        }
        newTableCreated = true;
    }

    // Fix. remove type 4 and check on have type 3
    QStringList listType3Senders;
    for (int i = 0; i < rowsFromServer.size(); i++) {
        if (rowsFromServer.at(i).type == 4) {
            if (!m_mapSubscribeUserNotified.contains(rowsFromServer.at(i).sender)) {
                m_mapSubscribeUserNotified.insert(rowsFromServer.at(i).sender, true);
                emit showChatNotification(_clr_(rowsFromServer.at(i).msg, "gray"));
            }
            rowsFromServer.removeAt(i--);
        } else if (rowsFromServer.at(i).type == 3) {
            const QString bannedUser = rowsFromServer.at(i).sender;
            bool isOld = m_mapListType3SendersOld.value(streamId).contains(bannedUser);
            if (!isOld)
                listType3Senders.append(bannedUser);
            if (!isOld && m_params->flagLogging()) {
                addToLog(QString("Notification. "
                    "Ban user - %1!").arg(bannedUser));
            }
            rowsFromServer.removeAt(i--);
        }
    }

    // Fix. remove banned user from table
    for (int i = 0; i < listType3Senders.size(); i++) {
        removeMessagesBannedUserFromTableDB(streamId, listType3Senders.at(i));
    }
    QStringList newOldList = m_mapListType3SendersOld[streamId] + listType3Senders;
    m_mapListType3SendersOld.insert(streamId, newOldList);

    // 2. Запрос сообщений с таблицы (100, либо все)
    if (!newTableCreated) {
        if (mergeOnly) {
            selectMsgsFromTableDB(streamId, rowsFromDB, 100);
        } else {
            selectMsgsFromTableDB(streamId, rowsFromDB, -1);
        }
    }

    // 3. Слияние
    if (newTableCreated) {
        rowsForInsert.append(rowsFromServer);
    } else {
        if (mergeOnly) {
            mergeMessages(rowsFromDB,
                          rowsFromServer, rowsForInsert);
        } else {
            mergeMessages(listRight<MessageData>(rowsFromDB, 100),
                          rowsFromServer, rowsForInsert);
        }
    }

    // 4. Вставка новых сообщений в БД
    appendMsgIntoTableDB(streamId, rowsForInsert);

    // 5. Обновление чата
    if (mergeOnly) {
        updateChat(rowsForInsert);
    } else {
        updateChat(rowsFromDB + rowsForInsert);
    }

    // 6. Анализ сообщений на комманды -> выполнение комманд (добавление задач)
    if (mergeOnly || newTableCreated) {
        boxUpdate(rowsForInsert);
        analyseNewMessages(rowsForInsert);
    }

    return CCBotErrEnums::Ok;
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
    case CCBotTaskEnums::SpeechkitVoice:
        {
            QString text = args.value(0,"").toString();
            if(!text.isEmpty()) {
                SpeechkitTTS::Options options = SpeechkitTTS::makeOptions(m_params->speechkitVoice(),
                                                                          m_params->speechkitLang(),
                                                                          m_params->speechkitSpeed(),
                                                                          m_params->speechkitEmotion());
                TTSManager::Task task = TTSManager::makeTask(TTSManager::TypeTTS::SpeechKit, text, QVariant::fromValue(options));
                m_pManagerTTS->addTask(task);
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

void CCBot::exec(QString command)
{
    m_consoleInput->exec(m_params->currentStreamerNikname(), command);
}

QString CCBot::keyUpCommand()
{
    return m_consoleInput->upCommandInBuffer();
}

QString CCBot::keyDownCommand()
{
    return m_consoleInput->downCommandInBuffer();
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
    } else {
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
}
