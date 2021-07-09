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
    m_player = new QMediaPlayer;

    loadSettings();
}

CCBot::~CCBot()
{
    delete m_player;
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

    initSysCommands();
    initConnections();
    initTasks();
    initTimers();

    if (!QSqlDatabase::drivers().contains("QSQLITE")) {
        if (m_params->flagLogging()) {
            addToLog("ERROR! Unable to load database, needs the SQLITE driver!");
        }
        emit showMessage("Ошибка", "Unable to load database.\nNeeds the SQLITE driver!", true);
    } else {
        initDB();
    }

//    qDebug() << getModelAvaibleHistoryNiknames().join(",");
//    qDebug() << getModelAvaibleHistoryStreamsByNikname("TANIST").join(",");
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

    // load saved command buffer stack
    m_consoleInput->loadCommandBufferStackSaves();
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
    cfg.setValue("PriceSymbol", m_params->speechKitPriceBySymbol());
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
    cfg.setValue("OptionSpeakReasonType", m_params->speakOptionReasonType());
    cfg.endGroup();

    cfg.beginGroup("ToReplaceForVoice");
    cfg.setValue("data", m_dataToReplaceTextForVoice.toJson(QJsonDocument::Compact));
    cfg.endGroup();

    cfg.beginGroup("Box");
    cfg.setValue("UserStartingBalance", m_params->boxUserStartingBalance());
    cfg.setValue("DefaultOnFlag0", m_params->boxDefaultOnFlag0());
    cfg.setValue("NotificationChatByEmptyUserBalanceForVoice",
                 m_params->boxNotificationChatByEmptyUserBalanceForVoice());
    cfg.endGroup();

    //...

    if (m_params->flagLogging()) {
        m_log.endLogSession();
    }

    // save command buffer stack
    m_consoleInput->saveCommandBufferStack();
}

void CCBot::initTimers()
{
    //
}

void CCBot::initConnections()
{
    // соединение: конец проигрывания файла
    connect(m_player,
            &QMediaPlayer::stateChanged,
            [this](QMediaPlayer::State state)
    {
        if (state == QMediaPlayer::StoppedState) {
            emit completePlayFile();
        }
    });
    connect(m_player,
            QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error),
            [=](QMediaPlayer::Error error) {
        const QString errStr = QString("Media Player error(%1): ").arg(error)
                + m_player->errorString();
        qDebug() << errStr;
        if (m_params->flagLogging()) {
            addToLog(errStr);
        }
    });
    // соединение: запуск комманд через консоль
    connect(m_consoleInput, &Console::runCommand, [=](QString sender, QString command, QStringList args) {
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
                    //qDebug() << sender << option << "=" << value;
                    break;
                }
            }
        } else {
            target = sender;
        }

        if (!target.isEmpty()) {
            switch (type) {
            case CCBotTaskEnums::SysCmdDrink:
                break;
            case CCBotTaskEnums::SysCmdSetVoice:
                for (const auto &arg : args) {
                    QString option = arg.section('=', 0, 0);
                    QString value = arg.section('=', -1, -1);
                    if (option == "on") {
                        boxSetFlag(target, BoxFlagsEnums::FLAG_SPEECH_ON, 1);
                        isValidCommand = true;
                    } else if (option == "off") {
                        boxSetFlag(target, BoxFlagsEnums::FLAG_SPEECH_ON, 0);
                        isValidCommand = true;
                    } else if (option == "name") {
                        if (isValidVoiceName(value)) {
                            boxSetUserVoice(target, value);
                            isValidCommand = true;
                        }
                    } else if (option == "emotion") {
                        if (value == "evil"
                                || value == "neutral"
                                || value == "normal"
                                || value == "good"
                                || value.isEmpty())
                        {
                            if (value == "normal") {
                                value = "neutral";
                            }
                            boxSetUserEmotionVoice(target, value);
                            isValidCommand = true;
                        }
                    } else if (option == "speed") {
                        if (isStreamer) {
                            double fvalue = value.toDouble();
                            if (fvalue >= 0.4 && fvalue <= 3.0) {
                                boxGetUserSpeedVoice(target, value);
                                isValidCommand = true;
                            }
                        }
                    }
                }
                break;
            default:
                break;
            }
        }
        if (isValidCommand)
            emit showChatNotification(_clr_(fullCommand, "green"));
        else
            emit showChatNotification(_clr_(fullCommand + " no valid command!", "red"));
    });
}

void CCBot::initSysCommands()
{
    m_consoleInput->registerTaskTypeAlias("!voice", CCBotTaskEnums::SysCmdSetVoice); // установка голоса
    m_consoleInput->registerTaskTypeAlias("!drink", CCBotTaskEnums::SysCmdDrink);    // установка темпа голоса на время
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
                          [this](QString text, SpeakOptions options) -> TaskResult {
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
                    [&]()
            {
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
                    [&](QNetworkReply::NetworkError error)
            {
                errType = error;
                if (m_params->flagLogging()) {
                    const QString errStr = QString("Error network reply(%1) - ")
                            .arg(errType) + reply->errorString()
                            + "\nUrl_reply: " + reply->url().toString()
                            + "\nRawHeaders_reply: " + reply->rawHeaderList().join(";")
                            + "\nUrl_request: " + reply->request().url().toString()
                            + "\nRawHeaders_request: " + reply->request().rawHeaderList().join(";");
                    addToLog(errStr);
                }
                emit completeRequestGetIamToken();
            });

            connect(reply,
                    &QNetworkReply::sslErrors,
                    this,
                    [&](const QList<QSslError> &errors)
            {
                errorsSsl = errors;
                if (m_params->flagLogging()) {
                    QString errorsText = "";
                    for (const auto &err: errors) {
                        errorsText.append(err.errorString()) + "; ";
                    }
                    QString errStr = QString("Error network reply ssl.")
                            + "\nErrors: " + errorsText
                            + "\nUrl_reply: " + reply->url().toString()
                            + "\nRawHeaders_reply: " + reply->rawHeaderList().join(";")
                            + "\nUrl_request: " + reply->request().url().toString()
                            + "\nRawHeaders_request: " + reply->request().rawHeaderList().join(";");
                    addToLog(errStr);
                }
                emit completeRequestGetIamToken();
            });

            bool timeout = !waitSignal(this,
                                       &CCBot::completeRequestGetIamToken,
                                       constTimeoutGetIamToken + 500);
            reply->disconnect();

            if (timeout) {
                result = TaskResult(CCBotErrEnums::NetworkRequest,
                                    "Timeout request, not get iam-token!");
                delete manager;
                return result;
            }

            if (errType != QNetworkReply::NoError || !errorsSsl.isEmpty()) {
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

            postDataEncoded.addQueryItem("folderId",
                                         m_params->speechkitFolderId());

            QString voice = options.voice.isEmpty() ? m_params->speechkitVoice() : options.voice;
            QString emotion = options.emotion.isEmpty() ? m_params->speechkitEmotion() : options.emotion;
            QString speed = options.speed.isEmpty() ? m_params->speechkitSpeed() : options.speed;

            if (!voice.isEmpty()) {
                postDataEncoded.addQueryItem("lang", getLangByVoiceName(voice));
                postDataEncoded.addQueryItem("voice", voice);
            }

            if (!emotion.isEmpty()) {
                postDataEncoded.addQueryItem("emotion", emotion);
            }
            if (!speed.isEmpty()) {
                postDataEncoded.addQueryItem("speed", speed);
            }
            if (!m_params->speechkitFormat().isEmpty()) {
                postDataEncoded.addQueryItem("format",
                                             m_params->speechkitFormat());
            }
            if (!m_params->speechkitSampleRateHertz().isEmpty()) {
                postDataEncoded.addQueryItem("sampleRateHertz",
                                      m_params->speechkitSampleRateHertz());
            }

            // * запрос
            // * делаем запрос
            manager->setTransferTimeout(constTimeoutGetAudio);
            requestGetAudio.setUrl(url);
            QNetworkReply *reply = manager->post(requestGetAudio,
                                                 postDataEncoded
                                                 .toString(QUrl::FullyEncoded)
                                                 .toUtf8());
            QNetworkReply::NetworkError errType = QNetworkReply::NoError;
            QList<QSslError> errorsSsl;
            connect(reply, &QNetworkReply::finished, this, [&reply,this]() {
                QByteArray response = reply->readAll();
                if (QJsonDocument::fromJson(response).isObject()) {
                    if (m_params->flagLogging()) {
                        const QString errStr = QString("Error speechkit service.")
                                + "\nResponse: " + QString::fromUtf8(response);
                        addToLog(errStr);
                    }
                } else {
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
                    [&](QNetworkReply::NetworkError error)
            {
                errType = error;
                if (m_params->flagLogging()) {
                    const QString errStr = QString("Error network reply(%1) - ")
                            .arg(errType) + reply->errorString()
                            + "\nUrl_reply: " + reply->url().toString()
                            + "\nRawHeaders_reply: " + reply->rawHeaderList().join(";")
                            + "\nUrl_request: " + reply->request().url().toString()
                            + "\nRawHeaders_request: " + reply->request().rawHeaderList().join(";");
                    addToLog(errStr);
                }
                emit completeRequestGetAudio();
            });
            connect(reply,
                    &QNetworkReply::sslErrors,
                    this,
                    [&](const QList<QSslError> &errors)
            {
                errorsSsl = errors;
                if (m_params->flagLogging()) {
                    QString errorsText = "";
                    for (const auto &err: errors) {
                        errorsText.append(err.errorString()) + "; ";
                    }
                    QString errStr = QString("Error network reply ssl.")
                            + "\nErrors: " + errorsText
                            + "\nUrl_reply: " + reply->url().toString()
                            + "\nRawHeaders_reply: " + reply->rawHeaderList().join(";")
                            + "\nUrl_request: " + reply->request().url().toString()
                            + "\nRawHeaders_request: " + reply->request().rawHeaderList().join(";");
                    addToLog(errStr);
                }
                emit completeRequestGetAudio();
            });

            bool timeout = !waitSignal(this,
                                       &CCBot::completeRequestGetAudio,
                                       constTimeoutGetAudio + 500);
            reply->disconnect();

            if (timeout) {
                result =
                        TaskResult(CCBotErrEnums::NetworkRequest,
                                   "Timeout request, not get audio!");
                delete manager;
                return result;
            }
            if (errType != QNetworkReply::NoError ||
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

void CCBot::openLogDir()
{
    m_log.openLogDir();
}

bool CCBot::openDB(QString name)
{
    QString path = QStandardPaths::writableLocation(
                QStandardPaths::AppDataLocation);

//    qDebug() << path;

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
        //qDebug() << "Error, missing database or opened from another program!";
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
                .arg(m_db.lastError().nativeErrorCode())
                .arg(baseName);
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
                .arg(m_db.lastError().nativeErrorCode())
                .arg(baseName);
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
    case CCBotTaskEnums::VoiceLoad:
        {
            QString text = args.value(0,"").toString();
            if(!text.isEmpty()) {
                SpeakOptions options;
                m_pCore->addTask(type, text, options);
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
//    qDebug() << "###";
    m_consoleInput->exec(m_params->currentStreamerNikname(), command);
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
