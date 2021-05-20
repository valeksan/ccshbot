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
    /*, m_speechkitMgr(new QNetworkAccessManager(this)),*/ m_player(new QMediaPlayer)
{    
    m_params = params;

    loadSettings();

    initConnections();
    initTasks();
    initTimers();
}

CCBot::~CCBot()
{
    delete m_player;
}

void CCBot::loadSettings()
{
    QSettings cfg;
    cfg.beginGroup("SpeechKit");
    m_params->setSpeechkitFolderId(cfg.value("FolderID", "").toString());
    m_params->setSpeechkitOAuthToken(cfg.value("OAuthToken", "").toString());
    cfg.endGroup();
}

void CCBot::saveSettings()
{
    QSettings cfg;
    cfg.beginGroup("SpeechKit");
    if(!m_params->speechkitFolderId().isEmpty())
        cfg.setValue("FolderID", m_params->speechkitFolderId());
    if(!m_params->speechkitOAuthToken().isEmpty())
        cfg.setValue("OAuthToken", m_params->speechkitOAuthToken());
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

    connect(m_player, &QMediaPlayer::stateChanged, [this](QMediaPlayer::State state) {
        if (state == QMediaPlayer::StoppedState) {
            emit completePlayFile();
        }
    });

    connect(m_params, &Properties::listenClientsChanged, [this]() {
        if (m_params->listenClients() == true) {
            openDB();
        } else {
            closeDB();
        }
    });

    // соедиение загрузки с сервиса speechkit
    /*
    connect(m_speechkitMgr, &QNetworkAccessManager::finished, this, [&](QNetworkReply *reply) {
        if (reply->error() == QNetworkReply::NoError) {
            if(reply->request().attribute(QNetworkRequest::User, "").toString() == "getIamToken") {
                QByteArray response = reply->readAll();
                qDebug() << "getIamToken complete";
            }
        } else {
            //
        }
    });
    */
}

void CCBot::initTasks()
{
    m_pCore->registerTask(CCBotTaskEnums::MergeChat, [this](QString streamId, QString messagesJsonStr, bool loading) -> TaskResult {
        TaskResult result;
        QString errInfo = "";
        int state = 0;

        QMetaObject::invokeMethod(this, "insertNewMessagesInTable", Qt::BlockingQueuedConnection,
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

    m_pCore->registerTask(CCBotTaskEnums::VoiceLoad, [this](QString text) -> TaskResult {
        TaskResult result;
        QNetworkAccessManager *manager = new QNetworkAccessManager();
        // 1. Проверка что токен истек
        bool tokenExpiry = QDateTime::currentDateTime() >= m_params->speechkitIamTokenExpiryDate();
        if (tokenExpiry) {
            // 1.1 Если токен уже не действителен -> получаем новый и обновляем дату
            // - формируем запрос
            QNetworkRequest requestGetIamToken;
            requestGetIamToken.setUrl(QUrl(m_params->speechkitGetIamTokenHost()));
            //requestGetIamToken.setRawHeader("Authorization", QString("Bearer %1").arg(m_params->speechkitOAuthToken()).toUtf8());
            //requestGetIamToken.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
            requestGetIamToken.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
            //requestGetIamToken.setRawHeader("User-Agent", "Mozilla/5.0 (X11; Fedora; Linux x86_64; rv:88.0) Gecko/20100101 Firefox/88.0");
            //requestGetIamToken.setAttribute(QNetworkRequest::User, "getIamToken" );
            QJsonObject obj;
            obj["yandexPassportOauthToken"] = m_params->speechkitOAuthToken();
            QJsonDocument doc(obj);
            QByteArray data = doc.toJson();
            // - делаем запрос
            manager->setTransferTimeout(constTimeoutGetIamToken);
            QNetworkReply *reply = manager->post(requestGetIamToken, data);
            QNetworkReply::NetworkError errType = QNetworkReply::NoError;
            QList<QSslError> errorsSsl;
            connect(reply, &QNetworkReply::finished, this, [&]() {
                QByteArray response = reply->readAll();
                QJsonObject responseObj = QJsonDocument::fromJson(response).object();
                m_params->setSpeechkitIamToken(responseObj.value("iamToken").toString());
                m_params->setSpeechkitIamTokenExpiryDate(QDateTime::fromString(responseObj.value("expiresAt").toString(), Qt::ISODateWithMs));
                qDebug() << "IamToken: " << m_params->speechkitIamToken();
                qDebug() << "expiresAt: " << m_params->speechkitIamTokenExpiryDate().toString();
                emit completeRequestGetIamToken();
            });
            connect(reply, &QNetworkReply::errorOccurred, this, [&](QNetworkReply::NetworkError error) {
                errType = error;
                qDebug() << "err" << error;
                emit completeRequestGetIamToken();
            });
            connect(reply, &QNetworkReply::sslErrors, this, [&](const QList<QSslError> &errors) {
                errorsSsl = errors;
                qDebug() << "TEST_2";
                emit completeRequestGetIamToken();
            });
            bool timeout = !waitSignal(this, &CCBot::completeRequestGetIamToken, constTimeoutGetIamToken + 500);
            reply->disconnect();

            if(timeout) {
                result = TaskResult(CCBotErrEnums::NetworkRequest, "Timeout request, not get iam-token!");
                delete manager;
                return result;
            }
            if(errType != QNetworkReply::NoError || !errorsSsl.isEmpty()) {
                QString mainInfo = QString("Error request (") + QString::number(static_cast<int>(errType)) + "), ";
                QString secondInfo = errorsSsl.isEmpty() ? QString("no ssl errors") : QString("%1 ssl errors").arg(errorsSsl.size());
                result = TaskResult(CCBotErrEnums::NetworkRequest, mainInfo + secondInfo);
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
            QUrlQuery urlQuery;
            QStringList requestDataList;
            // add --data-urlencode
            urlQuery.addQueryItem("text", text);
            url.setQuery(urlQuery.query());
            // add header
            requestGetAudio.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
            requestGetAudio.setRawHeader("Authorization", QString("Bearer %1").arg(m_params->speechkitIamToken()).toUtf8());
            // add data
            requestDataList.append(QString("folderId=%1").arg(m_params->speechkitFolderId()));
            if(!m_params->speechkitLang().isEmpty()) {
                requestDataList.append(QString("lang=%1").arg(m_params->speechkitLang()));
            }
            if(!m_params->speechkitVoice().isEmpty()) {
                requestDataList.append(QString("voice=%1").arg(m_params->speechkitVoice()));
            }
            if(!m_params->speechkitEmotion().isEmpty()) {
                requestDataList.append(QString("emotion=%1").arg(m_params->speechkitEmotion()));
            }
            if(!m_params->speechkitSpeed().isEmpty()) {
                requestDataList.append(QString("speed=%1").arg(m_params->speechkitSpeed()));
            }
            if(!m_params->speechkitFormat().isEmpty()) {
                requestDataList.append(QString("format=%1").arg(m_params->speechkitFormat()));
            }
            if(!m_params->speechkitSampleRateHertz().isEmpty()) {
                requestDataList.append(QString("sampleRateHertz=%1").arg(m_params->speechkitSampleRateHertz()));
            }
            QByteArray requestData = requestDataList.join("&").toUtf8();
            // - запрос
            // - делаем запрос
            manager->setTransferTimeout(constTimeoutGetAudio);
            requestGetAudio.setUrl(url);
            QNetworkReply *reply = manager->post(requestGetAudio, requestData);
            QNetworkReply::NetworkError errType = QNetworkReply::NoError;
            QList<QSslError> errorsSsl;
            connect(reply, &QNetworkReply::finished, this, [&reply,this]() {
                QByteArray response = reply->readAll();
                if(QJsonDocument::fromJson(response).isObject()) {
                    qDebug() << "no_audio: " << response;
                } else {
                    qDebug() << "with audio";
                    // сохраняем файл на диске и передаем новой задачи имя файла
                    QTemporaryFile tmpfile;
                    if (tmpfile.open()) {
                        tmpfile.setAutoRemove(false);
                        QDataStream ostream(&tmpfile);
                        ostream.writeRawData(response, response.size());
                        tmpfile.close();
                        m_pCore->addTask(CCBotTaskEnums::VoiceSpeech, tmpfile.fileName());
                    }
                }
                emit completeRequestGetAudio();
            });
            connect(reply, &QNetworkReply::errorOccurred, this, [&](QNetworkReply::NetworkError error) {
                errType = error;
                qDebug() << "err" << error;
                emit completeRequestGetAudio();
            });
            connect(reply, &QNetworkReply::sslErrors, this, [&](const QList<QSslError> &errors) {
                errorsSsl = errors;
                qDebug() << "TEST_2";
                emit completeRequestGetAudio();
            });
            bool timeout = !waitSignal(this, &CCBot::completeRequestGetAudio, constTimeoutGetAudio + 500);
            reply->disconnect();

            if(timeout) {
                result = TaskResult(CCBotErrEnums::NetworkRequest, "Timeout request, not get audio!");
                delete manager;
                return result;
            }
            if(errType != QNetworkReply::NoError || !errorsSsl.isEmpty()) {
                QString mainInfo = QString("Error request (") + QString::number(static_cast<int>(errType)) + "), ";
                QString secondInfo = errorsSsl.isEmpty() ? QString("no ssl errors") : QString("%1 ssl errors").arg(errorsSsl.size());
                result = TaskResult(CCBotErrEnums::NetworkRequest, mainInfo + secondInfo);
                delete manager;
                return result;
            }
        }
        // 3. Завершение
        delete manager;
        return result;
    }, 1);

    m_pCore->registerTask(CCBotTaskEnums::VoiceSpeech, [this](QString filename) -> TaskResult {
        TaskResult result;
        qDebug() << "speek_filename: " << filename;
        QMetaObject::invokeMethod(this, "speechFile", Qt::BlockingQueuedConnection,
                                  Q_ARG(QString, filename)
                                  );
        // очистка
        QFile::remove(filename);
        return TaskResult();
    }, 2);
}

void CCBot::speechFile(QString filename)
{
    waitSignalAfterFunction(this, &CCBot::completePlayFile, [&filename, this]() {
        m_player->setMedia(QUrl::fromLocalFile(filename));
        m_player->setVolume(100);
        m_player->play();
    });
}

QString CCBot::generateErrMsg(int type, int errCode, QString info)
{
    Q_UNUSED(type)

    if(errCode == CCBotErrEnums::Ok) return "";
    if(errCode == CCBotErrEnums::NoInit) return QString("Задача не выполнялась, результат не инициализирован.");
    if(errCode == CCBotErrEnums::ParseJson) return QString("Фатальная ошибка, не удалось распарсить JSON-данные.");
    if(errCode == CCBotErrEnums::Sql) return QString("Ошибка SQL: %1").arg(info);
    if(errCode == CCBotErrEnums::NetworkRequest) return QString("%1").arg(info);

    return QString("Неизвестная ошибка, нет описания.");
}

bool CCBot::readMessagesFromJsonStr(QByteArray jsonData, QList<MessageData> &msgList, QString *errInfo)
{
    QJsonDocument jsonDoc;
    QJsonParseError parseError;

    jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        QString info = QString("Parse error at %1:%2").arg(parseError.offset).arg(parseError.errorString());
        qDebug() << info;
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

bool CCBot::openDB()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

//    qDebug() << path;

    if (path.isEmpty()) {
        qDebug() << "Cannot determine settings storage location";
        path = QDir::homePath() + QDir::separator() + ".ccbot";
    }

    QDir d{path};

    if (!d.exists()) {
        if (!d.mkpath(d.absolutePath())) {
            qDebug() << QString("Cannot create path: %1").arg(d.absolutePath());
            return false;
        }
    }

    QString file_path = path + QDir::separator() + constNameBaseStr;

    if (QSqlDatabase::contains(QSqlDatabase::defaultConnection)) {
        m_db = QSqlDatabase::database();
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE");
    }
    m_db.setDatabaseName(file_path);
    if (!m_db.open()) {
        qDebug() << "Error, missing database or opened from another program!";
        return false;
    }

    emit baseOpenned(true);

    return true;
}

void CCBot::closeDB()
{
    m_db.close();
    emit baseOpenned(false);
}

bool CCBot::createTableDB(QString streamId)
{
    QSqlQuery qry;
    const QString values = "id INTEGER PRIMARY KEY AUTOINCREMENT, type INTEGER NOT NULL, sender TEXT, nik_color TEXT, msg TEXT, pay REAL, timestamp DATETIME";
    const QString sql = QString("CREATE TABLE IF NOT EXISTS 't_%1' (%2)").arg(streamId, values);
    bool state = qry.exec(sql);

    return state;
}

bool CCBot::existsTableDB(QString streamId)
{
    QString tableName = "t_" + streamId;
    bool state = m_db.tables().contains(tableName);

    return state;
}

bool CCBot::selectMsgsFromTableDB(QString streamId, QList<MessageData> &msgList, int limit)
{
    QSqlQuery qry;
    QString sql;

    if(limit > 0) {
        sql = QString("SELECT * FROM ( SELECT * FROM t_%1 ORDER BY id DESC LIMIT %2) ORDER BY id ASC;").arg(streamId).arg(limit);
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
    qDebug() << "start_index: " << startIndex;
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
        // проверка на комманду
        if (analyseText.at(0) == QChar('!')) {
            return false;
        }
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
            bool state = openDB();
            emit baseOpenned(state);
        }
        break;
    case CCBotTaskEnums::CloseBase:
        {
            closeDB();
            emit baseOpenned(false);
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
