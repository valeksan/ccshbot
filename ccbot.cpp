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

    if (m_params->flagLogging()) {
        m_log.endLogSession();
    }
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

            if (!m_params->speechkitLang().isEmpty()) {
                postDataEncoded.addQueryItem("lang",
                                             m_params->speechkitLang());
            }
            if (!m_params->speechkitVoice().isEmpty()) {
                postDataEncoded.addQueryItem("voice",
                                             m_params->speechkitVoice());
            }
            if (!m_params->speechkitEmotion().isEmpty()) {
                postDataEncoded.addQueryItem("emotion",
                                             m_params->speechkitEmotion());
            }
            if (!m_params->speechkitSpeed().isEmpty()) {
                postDataEncoded.addQueryItem("speed",
                                             m_params->speechkitSpeed());
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
                    //qDebug() << "no_audio: " << response;
                    if (m_params->flagLogging()) {
                        const QString errStr = QString("Error speechkit service.")
                                + "\nResponse: " + QString::fromUtf8(response);
                        addToLog(errStr);
                    }
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

    // Fix. remove type 4 and check on have type 3
    int removeCount = 0;
    QStringList listType3Senders;
    for (int i = 0; i < rowsFromServer.size(); i++) {
        if (rowsFromServer.at(i).type == 4) {
            rowsFromServer.removeAt(i--);
            ++removeCount;
        } else if (rowsFromServer.at(i).type == 3) {
            const QString bannedUser = rowsFromServer.at(i).sender;
            listType3Senders.append(bannedUser);
            if (m_params->flagLogging()) {
                addToLog(QString("Notification. "
                    "Ban user - %1!").arg(bannedUser));
            }
        }
    }

    // 2. Запрос сообщений с таблицы (100, либо все)
    if(!tableNotExist) {
        if (mergeOnly) {
            selectMsgsFromTableDB(streamId, rowsFromDB, 100);
        } else {
            selectMsgsFromTableDB(streamId, rowsFromDB, -1);
        }
    }

    // 2.1. Fix. remove senders checks in listType3Senders
    if (!listType3Senders.isEmpty()) {
        for (int i = 0; i < rowsFromDB.size(); i++) {
            if (listType3Senders.contains(rowsFromDB.at(i).sender)) {
                rowsFromDB.removeAt(i--);
            }
        }
    }

    // 3. Слияние
    if (tableNotExist) {
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
    updateChat(rowsForInsert);
//    if (mergeOnly) {
//        updateChat(rowsForInsert);
//    } else {
//        updateChat(rowsFromDB + rowsForInsert);
//    }

    // 6. Анализ сообщений на комманды -> выполнение комманд (добавление задач)
    if (mergeOnly) {
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
