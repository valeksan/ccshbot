#include "ccbot_private.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QSqlQuery>
#include <QSqlError>

CCBotPrivate::CCBotPrivate(QObject *parent)
    : QObject(parent), m_pCore(new Core())
{
    connect(m_pCore, &Core::finishedTask, this, &CCBotPrivate::slotFinishedTask);
}

QString CCBotPrivate::generateErrMsg(int type, int errCode, QString info)
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

bool CCBotPrivate::readMessagesFromJsonStr(QByteArray jsonData,
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
        if (errInfo) {
            *errInfo = info;
        }
        if (m_params->flagLogging()) {
            addToLog(info);
        }
        return false;
    }

    if (!jsonDoc.isArray()) {
        QString info = "Parse structure error. This is not Array!";
        if (errInfo) {
            *errInfo = info;
        }
        if (m_params->flagLogging()) {
            addToLog(info);
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

bool CCBotPrivate::createTableDB(QString streamId)
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

    if (m_params->flagLogging() && !state) {
        QString info = QString("Sql query-create error(%1): ")
                .arg(qry.lastError().type()) + qry.lastError().text()
                + QString("\nQuery: %1").arg(qry.lastQuery());
        addToLog(info);
    }

    return state;
}

bool CCBotPrivate::existsTableDB(QString streamId)
{
    QString tableName = "t_" + streamId;
    bool state = m_db.tables().contains(tableName);

    return state;
}

bool CCBotPrivate::removeMessagesBannedUserFromTableDB(QString streamId, QString user)
{
    QString tableName = "t_" + streamId;
    QSqlQuery qry;
    QString sql;

    sql = "DELETE FROM :table WHERE sender=:user;";
    qry.prepare(sql);
    qry.bindValue(":table", tableName);
    qry.bindValue(":user", user);

    bool state = qry.exec();

    if (m_params->flagLogging() && !state) {
        QString info = QString("Sql query-select error(%1): ")
                .arg(qry.lastError().type()) + qry.lastError().text()
                + QString("\nQuery: %1").arg(qry.lastQuery());
        addToLog(info);
    }

    return state;
}

bool CCBotPrivate::selectMsgsFromTableDB(QString streamId,
                                  QList<MessageData> &msgList,
                                  int limit)
{
    QSqlQuery qry;
    QString sql;

    if(limit > 0) {
        sql = QString("SELECT * FROM ( "
                    "SELECT * FROM t_%1 ORDER BY id DESC LIMIT %2) "
                    "ORDER BY id ASC;")
                .arg(streamId)
                .arg(limit);
    } else {
        sql = QString("SELECT * FROM t_%1 ORDER BY id ASC;").arg(streamId);
    }

    bool state = qry.exec(sql);

    if (state) {
        while (qry.next()) {
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
    } else {
        if (m_params->flagLogging()) {
            QString info = QString("Sql query-select error(%1): ")
                    .arg(qry.lastError().type()) + qry.lastError().text()
                    + QString("\nQuery: %1").arg(qry.lastQuery());
            addToLog(info);
        }
    }

    return state;
}

bool CCBotPrivate::appendMsgIntoTableDB(QString streamId,
                                        QList<MessageData> &msgList)
{
    if (msgList.isEmpty()) {
        return true;
    }

    QDateTime timestamp = QDateTime::currentDateTime();

    for (int i = 0; i < msgList.size(); i++) {
        QSqlQuery qry;
        MessageData msg = msgList.at(i);
        msgList[i].timestamp = timestamp;
        QString sql = QString("INSERT INTO t_%1 "
                "(type, sender, nik_color, msg, pay, timestamp) "
                "VALUES (:type, :sender, :nik_color, :msg, :pay, :timestamp);")
                .arg(streamId);
        qry.prepare(sql);
        qry.bindValue(":type", msg.type);
        qry.bindValue(":sender", msg.sender);
        qry.bindValue(":nik_color", msg.type == 1 ? "#fff200" : msg.nik_color);
        qry.bindValue(":msg", msg.msg);
        qry.bindValue(":pay", msg.pay);
        qry.bindValue(":timestamp", timestamp.toString("yyyy-MM-dd hh:mm:ss"));
        bool state = qry.exec();
        if (m_params->flagLogging() && !state) {
            QString info = QString("Sql query-insert error(%1): ")
                    .arg(qry.lastError().type()) + qry.lastError().text()
                    + QString("\nQuery: %1").arg(qry.lastQuery());
            addToLog(info);
        }
    }

    return false;
}

bool CCBotPrivate::createBoxTableInDB()
{
    QSqlQuery qry;

    const QString values =  "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                            "nikname TEXT, "
                            "info TEXT DEFAULT NULL, "
                            "achieves TEXT DEFAULT NULL, "
                            "flags INTEGER DEFAULT 0, "
                            "registration_date DATETIME DEFAULT NULL, "
                            "last_activity_date DATETIME DEFAULT NULL, "
                            "count_msg INTEGER DEFAULT 0, "
                            "count_symbols INTEGER DEFAULT 0, "
                            "count_speech_symbols INTEGER DEFAULT 0, "
                            "donation REAL DEFAULT 0.0, "
                            "balance REAL DEFAULT NULL, "
                            "tts_voice TEXT DEFAULT NULL, "
                            "tts_speed_voice TEXT DEFAULT NULL, "
                            "tts_voice_emotion TEXT DEFAULT NULL, "
                            "rezerv TEXT DEFAULT NULL";
    const QString sql = QString("CREATE TABLE IF NOT EXISTS 'box' (%1)").arg(values);

    bool state = qry.exec(sql);

    if (m_params->flagLogging() && !state) {
        QString info = QString("Sql query-create error(%1): ")
                .arg(qry.lastError().type()) + qry.lastError().text()
                + QString("\nQuery: %1").arg(qry.lastQuery());
        addToLog(info);
    }

    return state;
}

bool CCBotPrivate::boxContainUser(QString nikname, bool &contain)
{
    QSqlQuery qry;
    QString sql;

    sql = "SELECT id FROM box WHERE nikname = :user LIMIT 1;";
    qry.prepare(sql);
    qry.bindValue(":user", nikname);

    bool state = qry.exec();

    if (m_params->flagLogging() && !state) {
        QString info = QString("Sql query-select error(%1): ")
                .arg(qry.lastError().type()) + qry.lastError().text()
                + QString("\nQuery: %1").arg(qry.lastQuery());
        addToLog(info);
        return false;
    }

    if (qry.last()) {
        contain = true;
    }

    return state;
}

bool CCBotPrivate::boxRegisterNewUser(QString nikname)
{
    QSqlQuery qry;
    QString sql;

    QDateTime timestamp = QDateTime::currentDateTime();
    quint32 flags = 0;

    if (m_params->boxDefaultOnFlag0()) {
        macro_qBitOn(flags, 0);
    }

    sql = "INSERT INTO box "
            "(nikname, registration_date, last_activity_date, balance, tts_voice, tts_speed_voice, tts_voice_emotion, flags) "
            "VALUES (:nikname, :reg_date, :reg_date, :balance, :voice, :speed, :emotion, :flags);";
    qry.prepare(sql);
    qry.bindValue(":reg_date", timestamp.toString("yyyy-MM-dd hh:mm:ss"));
    qry.bindValue(":nikname", nikname);
    qry.bindValue(":balance", m_params->boxUserStartingBalance());
    qry.bindValue(":voice", "zahar");
    qry.bindValue(":speed", "1.0");
    qry.bindValue(":emotion", "");
    qry.bindValue(":flags", flags);

    bool state = qry.exec();

    if (m_params->flagLogging() && !state) {
        QString info = QString("Sql query-insert error(%1): ")
                .arg(qry.lastError().type()) + qry.lastError().text()
                + QString("\nQuery: %1").arg(qry.lastQuery());
        addToLog(info);
    }

    return state;
}

bool CCBotPrivate::boxGetStatisticsOfMessage(QString nikname, quint64 &numMessages, quint64 &numSymbols)
{
    QSqlQuery qry;
    QString sql;

    sql = "SELECT count_msg, count_symbols FROM box WHERE nikname=:nikname;";
    qry.prepare(sql);
    qry.bindValue(":nikname", nikname);

    bool state = qry.exec();

    if (m_params->flagLogging() && !state) {
        QString info = QString("Sql query-select error(%1): ")
                .arg(qry.lastError().type()) + qry.lastError().text()
                + QString("\nQuery: %1").arg(qry.lastQuery());
        addToLog(info);
        return false;
    }

    if (qry.next()) {
        numMessages = qry.value(0).toULongLong();
        numSymbols = qry.value(1).toULongLong();
        return true;
    }

    return false;
}

bool CCBotPrivate::boxAddStatisticsOfMessage(QString nikname,
                                                 int numSymbolsToAdd)
{
    QSqlQuery qry;
    QString sql;
    bool state = false;

    // get prev places
    quint64 countMsgIn;
    quint64 countSymbolsIn;

    state = boxGetStatisticsOfMessage(nikname, countMsgIn, countSymbolsIn);

    if (!state)
        return false;

    QDateTime timestamp = QDateTime::currentDateTime();

    quint64 countMsgUpdated = countMsgIn + 1;
    quint64 countSymbolsUpdated = countSymbolsIn
            + static_cast<quint64>(numSymbolsToAdd);

    // update places
    sql = QString("UPDATE box SET "
            "count_msg=%1,"
            "count_symbols=%2,"
            "last_activity_date=\"%3\" "
            "WHERE nikname=\"%4\";")
            .arg(countMsgUpdated)
            .arg(countSymbolsUpdated)
            .arg(timestamp.toString("yyyy-MM-dd hh:mm:ss"))
            .arg(nikname);

    state = qry.exec(sql);

    if (m_params->flagLogging() && !state) {
        QString info = QString("Sql query-update error(%1): ")
                .arg(qry.lastError().type()) + qry.lastError().text()
                + QString("\nQuery: %1").arg(qry.lastQuery());
        addToLog(info);
    }

    return state;
}

bool CCBotPrivate::boxGetBalanceInfo(QString nikname, double &donation, double &balance)
{
    QSqlQuery qry;
    QString sql;

    sql = "SELECT donation, balance FROM box WHERE nikname=:nikname;";
    qry.prepare(sql);
    qry.bindValue(":nikname", nikname);

    bool state = qry.exec();

    if (m_params->flagLogging() && !state) {
        QString info = QString("Sql query-select error(%1): ")
                .arg(qry.lastError().type()) + qry.lastError().text()
                + QString("\nQuery: %1").arg(qry.lastQuery());
        addToLog(info);
        return false;
    }

    if (qry.next()) {
        donation = qry.value(0).toDouble();
        balance = qry.value(1).toDouble();
        return true;
    }

    return false;
}

bool CCBotPrivate::boxAddBalance(QString nikname, double cash, bool bonus)
{
    QSqlQuery qry;
    QString sql;
    bool state = false;

    // get prev places
    double prevDonationIn;
    double prevBalanceIn;
    state = boxGetBalanceInfo(nikname, prevDonationIn, prevBalanceIn);

    if (!state)
        return false;

    // update places
    double balanceUpdated = prevBalanceIn + cash;
    if (!bonus) {
        double donationUpdated = prevDonationIn + cash;
        sql = QString("UPDATE box SET "
                "donation = \"%1\", "
                "balance = \"%2\" "
                "WHERE nikname = \"%3\"")
                .arg(donationUpdated)
                .arg(balanceUpdated)
                .arg(nikname);
    } else {
        sql = QString("UPDATE box SET "
                "balance = \"%1\" "
                "WHERE nikname = \"%2\";")
                .arg(balanceUpdated)
                .arg(nikname);
    }

    state = qry.exec(sql);

    if (m_params->flagLogging() && !state) {
        QString info = QString("Sql query-update error(%1): ")
                .arg(qry.lastError().type()) + qry.lastError().text()
                + QString("\nQuery: %1").arg(qry.lastQuery());
        addToLog(info);
    }

    return state;
}

bool CCBotPrivate::boxSpendBalace(QString nikname, double cash, bool &isEmptyMoney)
{
    QSqlQuery qry;
    QString sql;
    bool state = false;

    // get prev places
    double prevDonationIn;
    double prevBalanceIn;
    state = boxGetBalanceInfo(nikname, prevDonationIn, prevBalanceIn);

    if (!state)
        return false;

    double balanceUpdated = prevBalanceIn - cash;

    if (balanceUpdated <= 0.0) {
        isEmptyMoney = true;
        return false;
    }
    isEmptyMoney = false;

    // update place
    sql = QString("UPDATE box SET "
            "balance = %1 "
            "WHERE nikname = \"%2\";")
            .arg(balanceUpdated)
            .arg(nikname);

    state = qry.exec(sql);

    if (m_params->flagLogging() && !state) {
        QString info = QString("Sql query-update error(%1): ")
                .arg(qry.lastError().type()) + qry.lastError().text()
                + QString("\nQuery: %1").arg(qry.lastQuery());
        addToLog(info);
    }

    return state;
}

bool CCBotPrivate::boxGetStatisticsOfSpeech(QString nikname, quint64 &numSpeechSymbols)
{
    QSqlQuery qry;
    QString sql;

    sql = "SELECT count_speech_symbols FROM box WHERE nikname=:nikname;";
    qry.prepare(sql);
    qry.bindValue(":nikname", nikname);

    bool state = qry.exec();

    if (m_params->flagLogging() && !state) {
        QString info = QString("Sql query-select error(%1): ")
                .arg(qry.lastError().type()) + qry.lastError().text()
                + QString("\nQuery: %1").arg(qry.lastQuery());
        addToLog(info);
        return false;
    }

    if (qry.next()) {
        numSpeechSymbols = qry.value(0).toULongLong();
        return true;
    }

    return false;
}

bool CCBotPrivate::boxAddNumSpeechSymbolsInStatistics(QString nikname, int numSymbolsAdds)
{
    QSqlQuery qry;
    QString sql;
    bool state = false;

    // get prev places
    quint64 prevSpeechSymbolsIn;
    state = boxGetStatisticsOfSpeech(nikname, prevSpeechSymbolsIn);

    if (!state)
        return false;

    quint64 speechSymbolsUpdated = prevSpeechSymbolsIn
            + static_cast<quint64>(numSymbolsAdds);

    // update places
    sql = QString("UPDATE box SET "
            "count_speech_symbols = %1 "
            "WHERE nikname = \"%2\";")
            .arg(speechSymbolsUpdated)
            .arg(nikname);

    state = qry.exec(sql);

    if (m_params->flagLogging() && !state) {
        QString info = QString("Sql query-update error(%1): ")
                .arg(qry.lastError().type()) + qry.lastError().text()
                + QString("\nQuery: %1").arg(qry.lastQuery());
        addToLog(info);
    }

    return state;
}

bool CCBotPrivate::boxCalculatePriceForSpeech(QString nikname, int numSpeechSymbols, double &price, QString *voiceIn)
{
    QSqlQuery qry;
    QString sql;

    if (qFuzzyCompare(m_params->speechKitPriceBySymbol(), 0.0)) {
        price = 0.0;
        return true;
    }

    sql = "SELECT tts_voice FROM box WHERE nikname=:nikname;";
    qry.prepare(sql);
    qry.bindValue(":nikname", nikname);

    bool state = qry.exec();

    if (m_params->flagLogging() && !state) {
        QString info = QString("Sql query-select error(%1): ")
                .arg(qry.lastError().type()) + qry.lastError().text()
                + QString("\nQuery: %1").arg(qry.lastQuery());
        addToLog(info);
        return false;
    }

    QString voice;

    if (qry.next()) {
        voice = qry.value(0).toString();
        if (voiceIn) {
            *voiceIn = voice;
        }
        double priceSymbol = m_params->speechKitPriceBySymbol();
        price = priceSymbol * numSpeechSymbols;
        return true;
    }

    price = 0.0;

    return false;
}

bool CCBotPrivate::boxSetUserVoice(QString nikname, QString voice)
{
    QSqlQuery qry;
    QString sql;

    // update places
    sql = QString("UPDATE box SET "
            "tts_voice = \"%1\" "
            "WHERE nikname = \"%2\";")
            .arg(voice)
            .arg(nikname);

    bool state = qry.exec(sql);

    if (m_params->flagLogging() && !state) {
        QString info = QString("Sql query-update error(%1): ")
                .arg(qry.lastError().type()) + qry.lastError().text()
                + QString("\nQuery: %1").arg(qry.lastQuery());
        addToLog(info);
    }

    return state;
}

bool CCBotPrivate::boxGetUserVoice(QString nikname, QString &voice)
{
    QSqlQuery qry;
    QString sql;

    sql = "SELECT tts_voice FROM box WHERE nikname=:nikname;";
    qry.prepare(sql);
    qry.bindValue(":nikname", nikname);

    bool state = qry.exec();

    if (m_params->flagLogging() && !state) {
        QString info = QString("Sql query-select error(%1): ")
                .arg(qry.lastError().type()) + qry.lastError().text()
                + QString("\nQuery: %1").arg(qry.lastQuery());
        addToLog(info);
        return false;
    }

    if (qry.next()) {
        voice = qry.value(0).toString();
        return true;
    }

    return false;
}

bool CCBotPrivate::boxSetUserSpeedVoice(QString nikname, QString speed)
{
    QSqlQuery qry;
    QString sql;

    sql = QString("UPDATE box SET "
            "tts_speed_voice = \"%1\" "
            "WHERE nikname = \"%2\";")
            .arg(speed)
            .arg(nikname);

    bool state = qry.exec(sql);

    if (m_params->flagLogging() && !state) {
        QString info = QString("Sql query-update error(%1): ")
                .arg(qry.lastError().type()) + qry.lastError().text()
                + QString("\nQuery: %1").arg(qry.lastQuery());
        addToLog(info);
    }

    return state;
}

bool CCBotPrivate::boxGetUserSpeedVoice(QString nikname, QString &speed)
{
    QSqlQuery qry;
    QString sql;

    sql = "SELECT tts_speed_voice FROM box WHERE nikname=:nikname;";
    qry.prepare(sql);
    qry.bindValue(":nikname", nikname);

    bool state = qry.exec();

    if (m_params->flagLogging() && !state) {
        QString info = QString("Sql query-select error(%1): ")
                .arg(qry.lastError().type()) + qry.lastError().text()
                + QString("\nQuery: %1").arg(qry.lastQuery());
        addToLog(info);
        return false;
    }

    if (qry.next()) {
        speed = qry.value(0).toString();
        return true;
    }

    return false;
}

bool CCBotPrivate::boxSetUserEmotionVoice(QString nikname, QString emotion)
{
    QSqlQuery qry;
    QString sql;

    sql = QString("UPDATE box SET "
            "tts_voice_emotion = \"%1\" "
            "WHERE nikname = \"%2\";")
            .arg(emotion)
            .arg(nikname);

    bool state = qry.exec(sql);

    if (m_params->flagLogging() && !state) {
        QString info = QString("Sql query-update error(%1): ")
                .arg(qry.lastError().type()) + qry.lastError().text()
                + QString("\nQuery: %1").arg(qry.lastQuery());
        addToLog(info);
    }

    return state;
}

bool CCBotPrivate::boxGetUserEmotionVoice(QString nikname, QString &emotion)
{
    QSqlQuery qry;
    QString sql;

    sql = "SELECT tts_voice_emotion FROM box WHERE nikname=:nikname;";
    qry.prepare(sql);
    qry.bindValue(":nikname", nikname);

    bool state = qry.exec();

    if (m_params->flagLogging() && !state) {
        QString info = QString("Sql query-select error(%1): ")
                .arg(qry.lastError().type()) + qry.lastError().text()
                + QString("\nQuery: %1").arg(qry.lastQuery());
        addToLog(info);
        return false;
    }

    if (qry.next()) {
        emotion = qry.value(0).toString();
        return true;
    }

    return false;
}

bool CCBotPrivate::boxGetAchieveList(QString nikname, QStringList &achieves)
{
    QSqlQuery qry;
    QString sql;

    sql = "SELECT achieves FROM box WHERE nikname=:nikname;";
    qry.prepare(sql);
    qry.bindValue(":nikname", nikname);

    bool state = qry.exec();

    if (m_params->flagLogging() && !state) {
        QString info = QString("Sql query-select error(%1): ")
                .arg(qry.lastError().type()) + qry.lastError().text()
                + QString("\nQuery: %1").arg(qry.lastQuery());
        addToLog(info);
        return false;
    }

    if (qry.next()) {
        achieves = qry.value(0).toString().split(",");
        return true;
    }

    return false;
}

bool CCBotPrivate::boxAddAchieve(QString nikname, QString achieve)
{
    QSqlQuery qry;
    QString sql;
    bool state = false;

    // get list
    QStringList achieveList;
    state = boxGetAchieveList(nikname, achieveList);

    if (!state)
        return false;

    // update list
    if (achieveList.contains(achieve)) {
        return true;
    } else {
        achieveList.append(achieve);
    }

    QString achieves = achieveList.join(",");

    sql = QString("UPDATE box SET "
            "achieves = \"%1\" "
            "WHERE nikname = \"%2\";")
            .arg(achieves)
            .arg(nikname);

    state = qry.exec(sql);

    if (m_params->flagLogging() && !state) {
        QString info = QString("Sql query-update error(%1): ")
                .arg(qry.lastError().type()) + qry.lastError().text()
                + QString("\nQuery: %1").arg(qry.lastQuery());
        addToLog(info);
    }

    return state;
}

bool CCBotPrivate::boxSetUserInfo(QString nikname, QString info)
{
    QSqlQuery qry;
    QString sql;

    sql = QString("UPDATE box SET "
            "info = \"%1\" "
            "WHERE nikname = \"%2\";")
            .arg(info)
            .arg(nikname);

    bool state = qry.exec(sql);

    if (m_params->flagLogging() && !state) {
        QString info = QString("Sql query-update error(%1): ")
                .arg(qry.lastError().type()) + qry.lastError().text()
                + QString("\nQuery: %1").arg(qry.lastQuery());
        addToLog(info);
    }

    return state;
}

bool CCBotPrivate::boxGetUserInfo(QString nikname, QString &info)
{
    QSqlQuery qry;
    QString sql;

    sql = "SELECT info FROM box WHERE nikname=:nikname;";
    qry.prepare(sql);
    qry.bindValue(":nikname", nikname);

    bool state = qry.exec();

    if (m_params->flagLogging() && !state) {
        QString info = QString("Sql query-select error(%1): ")
                .arg(qry.lastError().type()) + qry.lastError().text()
                + QString("\nQuery: %1").arg(qry.lastQuery());
        addToLog(info);
        return false;
    }

    if (qry.next()) {
        info = qry.value(0).toString();
        return true;
    }

    return false;
}

bool CCBotPrivate::boxGetFlags(QString nikname, quint32 &flags)
{
    QSqlQuery qry;
    QString sql;

    sql = "SELECT flags FROM box WHERE nikname=:nikname;";
    qry.prepare(sql);
    qry.bindValue(":nikname", nikname);

    bool state = qry.exec();

    if (m_params->flagLogging() && !state) {
        QString info = QString("Sql query-select error(%1): ")
                .arg(qry.lastError().type()) + qry.lastError().text()
                + QString("\nQuery: %1").arg(qry.lastQuery());
        addToLog(info);
        return false;
    }

    if (qry.next()) {
        flags = qry.value(0).toUInt();
        return true;
    }

    return false;
}

bool CCBotPrivate::boxSetFlag(QString nikname, int flag, int st)
{
    QSqlQuery qry;
    QString sql;
    bool state = false;

    // get flags
    quint32 flagsIn;
    state = boxGetFlags(nikname, flagsIn);

    if (!state)
        return false;

    // update flags
    quint32 flagsUpdated = flagsIn;
    if (st == 1)
        macro_qBitOn(flagsUpdated, flag);
    else
        macro_qBitOff(flagsUpdated, flag);

    if (flagsIn == flagsUpdated)
        return true;

    sql = QString("UPDATE box SET "
            "flags = %1 "
            "WHERE nikname = \"%2\";")
            .arg(flagsUpdated)
            .arg(nikname);

    state = qry.exec(sql);

    if (m_params->flagLogging() && !state) {
        QString info = QString("Sql query-update error(%1): ")
                .arg(qry.lastError().type()) + qry.lastError().text()
                + QString("\nQuery: %1").arg(qry.lastQuery());
        addToLog(info);
    }

    return state;
}

void CCBotPrivate::boxUpdate(const QList<MessageData> &newMsgs)
{
    bool boxTableCreated = m_db.tables().contains("box");

    if (!boxTableCreated) {
        boxTableCreated = createBoxTableInDB();
        if (!boxTableCreated)
            return;
    }

    for (int i = 0; i < newMsgs.size(); i++) {
        if (newMsgs.at(i).type == 1)
            continue;
        MessageData msg = newMsgs.at(i);
        bool userIsRegistred = false;
        bool state = boxContainUser(msg.sender, userIsRegistred);
        if (!userIsRegistred && state) {
            state = boxRegisterNewUser(msg.sender);
            if (!state) {
                continue;
            }
        }
        if (msg.type == 2) {
            boxAddBalance(msg.sender, static_cast<double>(msg.pay));
        }
        boxAddStatisticsOfMessage(msg.sender, msg.msg.length());
        QString textForSpeek = "";
        if (checkAutoVoiceMessage(msg, textForSpeek)) {
            boxAddNumSpeechSymbolsInStatistics(msg.sender, textForSpeek.length());
        }
    }
}

void CCBotPrivate::mergeMessages(QList<MessageData> oldMsgList,
                                 QList<MessageData> newMsgList,
                                 QList<MessageData> &mergedMsgList)
{
    // Поиск интервалов похожести для выбора наилучшего слияния списка
    // (выбирается интервал с большим весом - weight)
    QList<MessageData>::const_reverse_iterator iOldMsgs;
    QList<MessageData>::const_reverse_iterator iNewMsgs;
    QList<QPair<int,int> > intervals;
    int start = -1;
    int weight = 0;
    int spaceMsgCount = 0;
    bool flagEnterInterval = false;
    bool isBanState = false;

    QStringList allNiknames;

    for (auto msg : oldMsgList) {
        if(msg.type != 1) {
            if (!allNiknames.contains(msg.sender)) {
                allNiknames.append(msg.sender);
            }
        }
    }

    int currentNikIndexIfBanState = -1;

    QList<MessageData> oldMsgListTmp;
    QList<MessageData> newMsgListTmp;

    QPair<int,int> savedMaxInterval;
    QList<MessageData> savedOldMsgListTmp = oldMsgList;

    fixBanState:

    oldMsgListTmp = savedOldMsgListTmp;
    newMsgListTmp = newMsgList;

    if (isBanState) {
        for (int i = 0; i < oldMsgListTmp.size(); i++) {
            if (oldMsgListTmp.at(i).sender == allNiknames.at(currentNikIndexIfBanState)) {
                oldMsgListTmp.removeAt(i--);
            }
        }
    }

    if (!newMsgListTmp.isEmpty() && !oldMsgListTmp.isEmpty()) {
        for (iNewMsgs = newMsgListTmp.crbegin(), iOldMsgs = oldMsgListTmp.crbegin();
             iNewMsgs != newMsgListTmp.crend() && iOldMsgs != oldMsgListTmp.crend();)
        {
            if (equalMessages(*iNewMsgs, *iOldMsgs)) {
                if (!flagEnterInterval) {
                    start = iNewMsgs - newMsgListTmp.crbegin();
                    weight = 1;
                    flagEnterInterval = true;
                } else {
                    weight += 1;
                }
                ++iOldMsgs;
                ++iNewMsgs;
                if (iOldMsgs == oldMsgListTmp.crend()) {
                    intervals.append(QPair<int,int>(start, weight));
                    break;
                }
                if (iNewMsgs == newMsgListTmp.crend()) {
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
    } else {
        if (m_params->flagLogging()) {
            if (newMsgListTmp.isEmpty()) {
                addToLog(QString("Warrning. New message list is empty!"));
            }
            if (oldMsgListTmp.isEmpty()) {
                addToLog(QString("Notification. "
                    "Old message list is empty, no merge, load all messages."));
            }
        }
        if (oldMsgListTmp.isEmpty()) {
            mergedMsgList.append(newMsgListTmp);
        }
        return;
    }

    // Выбор интервала
    QPair<int,int> maxInterval = QPair<int,int>(-1, 0);
    for (const auto &interval : intervals) {
        if (interval.second > maxInterval.second) {
            maxInterval = interval;
        }
    }

    if (maxInterval.second < oldMsgListTmp.size()/2
            && allNiknames.size() > 1
            && oldMsgListTmp.size() > 1)
    {
        if (!isBanState || savedMaxInterval.second < maxInterval.second) {
            savedMaxInterval = maxInterval;
            savedOldMsgListTmp = oldMsgListTmp;
        }
        isBanState = true;
        if (currentNikIndexIfBanState != allNiknames.size()-1) {
            ++currentNikIndexIfBanState;
            goto fixBanState;
        }
    } else {
        isBanState = false;
    }

    if (isBanState) {
        maxInterval = savedMaxInterval;
        oldMsgListTmp = savedOldMsgListTmp;
    }

    // Спам-пакет из пачки сообщений т.к. не найдено совпадений вообще!
    // (если такое возможно) -> передаем его сразу в запись
    if (maxInterval.first == -1) {
        mergedMsgList = newMsgListTmp;
        if (m_params->flagLogging()) {
            addToLog(QString("Warrning. New message list is very big(%1 msgs)! "
                "No matches found in the database.").arg(newMsgListTmp.size()));
        }
        return;
    }

    // Отсутствуют новые сообщения!
    if (maxInterval.first == 0) {
        return;
    }

    // Записываем новые сообщения в список слияния
    int startIndex = newMsgListTmp.size() - maxInterval.first;
    mergedMsgList.append(newMsgListTmp.mid(startIndex));
    if (maxInterval.first != -1 &&
            mergedMsgList.size() > 10 &&
            m_params->flagLogging())
    {
        addToLog(QString("Warrning. New message list is big(%1 msgs)!")
                 .arg(newMsgListTmp.size()));
    }
}

bool CCBotPrivate::equalMessages(const MessageData &msg1,
                                 const MessageData &msg2)
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

void CCBotPrivate::updateChat(const QList<MessageData> &msgsl,
                              bool withTime,
                              QString timeFormat, bool history)
{
    for (int i = 0; i < msgsl.size(); i++) {
        MessageData msg = msgsl.value(i);
        QString timeStr = msg.timestamp.toString(timeFormat);
        QString fragment0 = withTime ?
                    timeStr + ": "
                    :
                    "";
        QString nikStr = msg.sender;
        QString fragment1 = nikStr.isEmpty() ?
                    ""
                    :
                    msg.sender + ": ";
        QString fmtFragment1 = msg.nik_color.isEmpty() ?
                    fragment1
                    :
                    _clr_(fragment1, msg.nik_color);
        QString fragment2 = msg.msg;
        QString fmtFragment2 = fragment2.isEmpty() ?
                    (
                        msg.type == 2 ?
                        _bclr_(
                                QString("($")
                                + QString::number(static_cast<double>(msg.pay),
                                                   'f', 2) + ")",
                                "#fff200"
                        )
                        :
                        ""
                    )
                    :
                    (
                        msg.type == 2 ?
                        _bclr_(
                                fragment2
                                + " ($"
                                + QString::number(static_cast<double>(msg.pay),
                                                  'f', 2) + ")",
                               "#fff200")
                        :
                        fragment2
                    );
        QString msgStr = fragment0 + fmtFragment1 + fmtFragment2;

        if (!history)
            emit showChatMessage(msgStr);
        else
            emit showHistoryMessage(msgStr);
    }
}

void CCBotPrivate::analyseNewMessages(const QList<MessageData> &msgsl)
{
    for (int i = 0; i < msgsl.size(); i++) {
        MessageData msg = msgsl.at(i);
        QString text = "";
        if (checkAutoVoiceMessage(msg, text)) {
            m_pCore->addTask(CCBotTaskEnums::VoiceLoad, text);
        }
    }
}

bool CCBotPrivate::checkAutoVoiceMessage(const MessageData &msg, QString &text)
{
    if (msg.msg.isEmpty()) {
        return false;
    }
    if ((m_params->flagAnalyseVoiceAllMsgType2() && msg.type == 2)
            || (m_params->flagAnalyseVoiceAllMsgType0() && msg.type == 0)
            )
    {
        QString analyseText = msg.msg;

        QJsonArray jarr = m_dataToReplaceTextForVoice.array();
        for (int i = 0; i < jarr.size(); i++) {
            QJsonValue value = jarr.at(i);
            if (value.isObject()) {
                QJsonObject objItem = value.toObject();
                QString keyword = objItem.value("w").toString();
                analyseText.replace(keyword, getReplaceWordForVoice(keyword));
            }
        }
        analyseText = analyseText.remove(
                    QRegularExpression("[\\x{1F600}-\\x{1F7FF}]+"));

        // check on empty message
        bool emptyMsg = true;
        for (int i = 0; i < analyseText.length(); i++) {
            if(analyseText.at(i).isLetterOrNumber()) {
                emptyMsg = false;
                text = analyseText;
                break;
            }
        }
        return !emptyMsg;
    }
    return false;
}

QString CCBotPrivate::getReplaceWordForVoice(QString keyword)
{
    QString result = keyword;

    QJsonArray jarr = m_dataToReplaceTextForVoice.array();

    for (int i = 0; i < jarr.size(); i++) {
        QJsonValue value = jarr.at(i);
        if (value.isObject()) {
            QJsonObject objItem = value.toObject();
            if (objItem.value("w").toString() == keyword) {
                QJsonArray r = objItem.value("r").toArray();
                if (r.size() == 1) {
                    result = r.at(0).toString();
                    break;
                } else {
                    QRandomGenerator *rg = QRandomGenerator::global();
                    int max = r.size();
                    int dash = rg->bounded(1, max);
                    result = r.at(dash).toString();
                }
            }
        }
    }

    return result;
}

bool CCBotPrivate::checkCmdMessage(const MessageData &msg,
                                   QString &cmd,
                                   QStringList &args)
{
    Q_UNUSED(msg)
    Q_UNUSED(cmd)
    Q_UNUSED(args)
    //
    return false;
}

void CCBotPrivate::addToLog(QString text, bool isTimelined)
{
    if (m_log.lastLogName().isEmpty()) {
        return;
    }
    if (isTimelined) {
        m_log.appendLastLogTimeline(text);
        return;
    }
    m_log.appendLastLog(text);
}

void CCBotPrivate::addWordPairToReplaceForVoice(QString keyword, QString word)
{
    if (!m_dataToReplaceTextForVoice.isArray()) {
        m_dataToReplaceTextForVoice = QJsonDocument::fromJson("[]");
    }
    QJsonArray jarr = m_dataToReplaceTextForVoice.array();

    // if keyword was contained in data
    for (int i = 0; i < jarr.size(); i++) {
        QJsonValue value = jarr.at(i);
        if (value.isObject()) {
            QJsonObject objItem = value.toObject();
            if (objItem.value("w").toString() == keyword) {
                QJsonArray r = objItem.value("r").toArray();
                r.append(QJsonValue(word));
                objItem.insert("r", QJsonValue(r));
                jarr.replace(i, QJsonValue(objItem));
                m_dataToReplaceTextForVoice.setArray(jarr);
                return;
            }
        }
    }

    // append keyword pair
    QJsonObject newObjItem;
    newObjItem.insert("w", QJsonValue(keyword));
    QJsonArray r = {word};
    newObjItem.insert("r", QJsonValue(r));
    jarr.append(QJsonValue(newObjItem));
    m_dataToReplaceTextForVoice.setArray(jarr);
    qDebug() << "JSON:" << m_dataToReplaceTextForVoice.toJson(QJsonDocument::Compact);
}

void CCBotPrivate::editRepitWordForVoice(QString keyword, QString oldWord, QString newWord)
{
    if (!m_dataToReplaceTextForVoice.isArray()) {
        m_dataToReplaceTextForVoice = QJsonDocument::fromJson("[]");
    }
    QJsonArray jarr = m_dataToReplaceTextForVoice.array();

    // if keyword was contained in data
    for (int i = 0; i < jarr.size(); i++) {
        QJsonValue value = jarr.at(i);
        if (value.isObject()) {
            QJsonObject objItem = value.toObject();
            if (objItem.value("w").toString() == keyword) {
                QJsonArray r = objItem.value("r").toArray();
                for (int j = 0; j < r.size(); j++) {
                    if (r.at(j) == oldWord) {
                        r.replace(j, QJsonValue(newWord));
                        objItem.insert("r", QJsonValue(r));
                        jarr.replace(i, QJsonValue(objItem));
                        m_dataToReplaceTextForVoice.setArray(jarr);
                        return;
                    }
                }
                return;
            }
        }
    }
}

void CCBotPrivate::removeRepWordForVoice(QString keyword, QString word)
{
    if (!m_dataToReplaceTextForVoice.isArray()) {
        m_dataToReplaceTextForVoice = QJsonDocument::fromJson("[]");
    }
    QJsonArray jarr = m_dataToReplaceTextForVoice.array();

    // if keyword was contained in data
    for (int i = 0; i < jarr.size(); i++) {
        QJsonValue value = jarr.at(i);
        if (value.isObject()) {
            QJsonObject objItem = value.toObject();
            if (objItem.value("w").toString() == keyword) {
                QJsonArray r = objItem.value("r").toArray();
                for (int j = 0; j < r.size(); j++) {
                    if (r.at(j) == word) {
                        r.removeAt(j);
                        objItem.insert("r", QJsonValue(r));
                        jarr.replace(i, QJsonValue(objItem));
                        m_dataToReplaceTextForVoice.setArray(jarr);
                        if (r.isEmpty()) {
                            removeRepKeywordForVoice(keyword);
                        }
                        return;
                    }
                }
                return;
            }
        }
    }
}

void CCBotPrivate::removeRepKeywordForVoice(QString keyword)
{
    if (!m_dataToReplaceTextForVoice.isArray()) {
        m_dataToReplaceTextForVoice = QJsonDocument::fromJson("[]");
    }
    QJsonArray jarr = m_dataToReplaceTextForVoice.array();

    // if keyword was contained in data
    for (int i = 0; i < jarr.size(); i++) {
        QJsonValue value = jarr.at(i);
        if (value.isObject()) {
            QJsonObject objItem = value.toObject();
            if (objItem.value("w").toString() == keyword) {
                jarr.removeAt(i);
                m_dataToReplaceTextForVoice.setArray(jarr);
                return;
            }
        }
    }
}

void CCBotPrivate::downSwapRepKeywordForVoice(int index)
{
    if (!m_dataToReplaceTextForVoice.isArray()) {
        m_dataToReplaceTextForVoice = QJsonDocument::fromJson("[]");
    }
    QJsonArray jarr = m_dataToReplaceTextForVoice.array();

    if (index >= jarr.size() - 1
            || index == -1
            || jarr.size() < 2)
    {
        return;
    }

    QJsonValue tmp = jarr.at(index);
    jarr.replace(index, jarr.at(index + 1));
    jarr.replace(index + 1, tmp);

    m_dataToReplaceTextForVoice.setArray(jarr);
}

void CCBotPrivate::upSwapRepKeywordForVoice(int index)
{
    if (!m_dataToReplaceTextForVoice.isArray()) {
        m_dataToReplaceTextForVoice = QJsonDocument::fromJson("[]");
    }
    QJsonArray jarr = m_dataToReplaceTextForVoice.array();

    if (index < 1
            || jarr.size() < 2)
    {
        return;
    }

    QJsonValue tmp = jarr.at(index);
    jarr.replace(index, jarr.at(index - 1));
    jarr.replace(index - 1, tmp);

    m_dataToReplaceTextForVoice.setArray(jarr);
}

QString CCBotPrivate::getWordPairListInJson(bool compact)
{
    return QString::fromUtf8(m_dataToReplaceTextForVoice.toJson(compact ? QJsonDocument::Compact : QJsonDocument::Indented));
}

const QString CCBotPrivate::getAppDataDirPath()
{
    QString path = QStandardPaths::writableLocation(
                QStandardPaths::AppDataLocation);
    if (path.isEmpty()) {
        qDebug() << "Cannot determine settings storage location";
        path = QDir::homePath() + QDir::separator() + ".ccbot";
    }
    return path;
}

bool CCBotPrivate::startLog()
{
    return m_log.startLogSession();
}
