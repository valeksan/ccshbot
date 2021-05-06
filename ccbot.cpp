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

CCBot::CCBot(Properties *params, QObject *parent) : CCBotEngine(parent), m_params(params)
{
    loadSettings();

    //openDB();

    initConnections();
    initTasks();
    initTimers();
}

CCBot::~CCBot()
{
    saveSettings();
}

void CCBot::loadSettings()
{
    QSettings cfg;
    //...
}

void CCBot::saveSettings()
{
    QSettings cfg;
    //...
}

void CCBot::initTimers()
{
    //
}

void CCBot::initConnections()
{
    // соединение: завершение программы
    connect(this, &CCBotEngine::signQuit, [=]()
    {
        QApplication::quit();
    });
}

void CCBot::initTasks()
{
    m_pCore->registerTask(CCBotTaskEnums::MergeChat, [=](QString streamId, QString messagesJsonStr) -> TaskResult {
        TaskResult result;
        // 1. Parse JSON datagram
        QJsonDocument jsonDoc;
        QJsonParseError parseError;
        jsonDoc = QJsonDocument::fromJson(messagesJsonStr.toUtf8(), &parseError);
        if(parseError.error != QJsonParseError::NoError) {
            QString info = QString("Parse error at %1:%2").arg(parseError.offset).arg(parseError.errorString());
            return TaskResult(CCBotErrEnums::ParseJson, info);
        }
        QJsonArray jsonArr = jsonDoc.array();
        // 2. Init variables from datagram
        //...
        return TaskResult();
    });
}

QString CCBot::generateErrMsg(int type, int errCode)
{
    Q_UNUSED(type)

    if(errCode == CCBotErrEnums::Ok) return "";
    if(errCode == CCBotErrEnums::NoInit) return tr("Задача не выполнялась, результат не инициализирован.");

    return tr("Неизвестная ошибка, нет описания.");
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
        msg.id = 0;
        msg.type = jsonArr.at(i).toInt();
        msg.sender = jsonArr.at(i).toString();
        msg.nik_color = jsonArr.at(i).toString();
        msg.msg = jsonArr.at(i).toString();
        msg.pay = static_cast<float>(jsonArr.at(i).toDouble());
        msg.flagIsNewMsg = true;
        msgList.append(msg);
    }

    return true;
}

bool CCBot::openDB()
{
    auto path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

//    qDebug() << path;

    if (path.isEmpty()) {
        qDebug() << "Cannot determine settings storage location";
        path = QDir::homePath() + QDir::separator() + ".ccbot";
    }

    QDir d{path};

    if(!d.exists()) {
        if(!d.mkpath(d.absolutePath())) {
            qDebug() << QString("Cannot create path: %1").arg(d.absolutePath());
            return false;
        }
    }

    QString file_path = path + QDir::separator() + constNameBaseStr;

    if(QSqlDatabase::contains(QSqlDatabase::defaultConnection)) {
        m_db = QSqlDatabase::database();
    }
    else {
        m_db = QSqlDatabase::addDatabase("QSQLITE");
    }
    m_db.setDatabaseName(file_path);
    if(!m_db.open()) {
        qDebug() << "Error, missing database or opened from another program!";
        return false;
    }
    return true;
}

bool CCBot::createTableDB(QString streamId)
{
    if(!m_db.isOpen()) {
        if(!openDB()) {
            return false;
        }
    }

    QSqlQuery qry;
    const QString values = "id INTEGER PRIMARY KEY AUTOINCREMENT, type INTEGER NOT NULL, sender TEXT, nik_color TEXT, msg TEXT, pay REAL, timestamp INTEGER";

    bool state = qry.exec(QString("CREATE TABLE IF NOT EXISTS '%1' (%2)").arg("t_" + streamId).arg(values));

    m_db.close();

    return state;
}

bool CCBot::existsTableDB(QString streamId)
{
    if(!m_db.isOpen()) {
        if(!openDB()) {
            return false;
        }
    }

    QString tableName = "t_" + streamId;
    bool state = m_db.tables().contains(tableName);

    m_db.close();

    return state;
}

bool CCBot::selectMsgsFromTableDB(QString streamId, QList<MessageData> &msgList, int limit)
{
    QSqlQuery qry;
    QString sql;

    if(!m_db.isOpen()) {
        if(!openDB()) {
            return false;
        }
    }

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
            msg.msg = qry.value("msg").toString();
            msg.pay = qry.value("pay").toFloat();
            msg.timestamp = qry.value("timestamp").toUInt();
            msgList.append(msg);
        }
    }

    m_db.close();

    return state;
}

bool CCBot::appendMsgIntoTableDB(QString streamId, QList<MessageData> msgList)
{
    if (msgList.isEmpty()) {
        return true;
    }

    if (!m_db.isOpen()) {
        if(!openDB()) {
            return false;
        }
    }

    quint32 timestamp = QDateTime::currentDateTime().toTime_t();

    for (int i = 0; i < msgList.size(); i++) {
        QSqlQuery qry;
        MessageData msg = msgList.at(i);
        QString sql = QString("INSERT INTO t_%1 (type, sender, nik_color, msg, pay, timestamp) VALUES (:type, :sender, :nik_color, :msg, :pay, :timestamp);").arg(streamId);
        qry.prepare(sql);
        qry.bindValue(":type", msg.type);
        qry.bindValue(":sender", msg.sender);
        qry.bindValue(":nik_color", msg.nik_color);
        qry.bindValue(":msg", msg.msg);
        qry.bindValue(":pay", msg.pay);
        qry.bindValue(":timestamp", timestamp);
        qry.exec();
    }

    m_db.close();

    return false;
}

int CCBot::insertNewMessagesInTable(QString streamId, QByteArray jsonData, QString *errInfo)
{
    QSqlQuery qry;
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
    if (!existsTableDB(streamId)) {
        if (!createTableDB(streamId)) {
            if (errInfo) {
                *errInfo = m_db.lastError().text();
            }
            return CCBotErrEnums::Sql;
        }
    }

    // 2. Запрос 100 сообщений с таблицы
    state = selectMsgsFromTableDB(streamId, rowsFromDB, 100);
    if (!state) {
        if (errInfo) {
            *errInfo = m_db.lastError().text();
        }
        return CCBotErrEnums::Sql;
    }

    // 3. Слияние
    if (qry.size() > 0) {
        // * Выборка-поиск новых сообщений из пачки от CrazyCash
        mergeMessages(rowsFromDB, rowsFromServer, rowsForInsert);
    } else {
        // * 3.1 Если в базе ничего нет то сразу записываем новые данные без слияния
        rowsForInsert.append(rowsFromServer);
    }

    // 4. Вставка новых сообщений в БД
    state = appendMsgIntoTableDB(streamId, rowsForInsert);
    if(!state) {
        if (errInfo) {
            *errInfo = m_db.lastError().text();
        }
        return CCBotErrEnums::Sql;
    }

    return CCBotErrEnums::Ok;
}

void CCBot::mergeMessages(QList<MessageData> oldMsgList, QList<MessageData> newMsgList, QList<MessageData> &mergedMsgList)
{
    //...
}

void CCBot::action(int type, QVariantList args)
{
    switch (type) {
    case CCBotTaskEnums::MergeChat:
        {
            QString streamId = args.value(0,"").toString();
            QString messagesJsonStr = args.value(1,"").toString();
            m_pCore->addTask(type, streamId, messagesJsonStr);
        }
        break;
    default:
        break;
    }

}

void CCBot::slotFinishedTask(long id, int type, QVariantList argsList, QVariant result)
{
    Q_UNUSED(id)

    auto taskResult = result.value<TaskResult>();
    QVariantList args = argsList.value(0).toList();
    int errCode = taskResult.errCode();

    if(errCode != CCBotErrEnums::Ok) {
        emit showMessage(tr("Ошибка"), generateErrMsg(type, errCode), true);
    }
}
