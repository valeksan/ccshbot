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

void CCBotPrivate::generateEmotionText(QString &text)
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
            funnyPower += 1;
            laughPower += 1;
            break;
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
                .arg(streamId).arg(limit);
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

void CCBotPrivate::mergeMessages(QList<MessageData> oldMsgList,
                                 QList<MessageData> newMsgList,
                                 QList<MessageData> &mergedMsgList)
{

//    if (oldMsgList.isEmpty()) {
//        mergedMsgList.append(newMsgList);
//        return;
//    }

//    QStringList type3NikNames;

//    // Поиск ников помеченных как тип 3 для игнорирования в сравнении списков
//    foreach (const MessageData &msg, newMsgList) {
//        if (msg.type == 3 && !type3NikNames.contains(msg.sender)) {
//            type3NikNames.append(msg.sender);
//            if (m_params->flagLogging()) {
//                addToLog(QString("Notification. Ban user - %1!").arg(msg.sender));
//            }
//        }
//    }

//    // Удаление из сравнения сообщений с ником из списка
//    for (const auto &nik : type3NikNames) {
//        for (int i = 0; i < oldMsgList.size(); i++) {
//            if (oldMsgList.at(i).sender == nik) {
//                oldMsgList.removeAt(i--);
//            }
//        }
//        for (int i = 0; i < newMsgList.size(); i++) {
//            if (newMsgList.at(i).sender == nik) {
//                newMsgList.removeAt(i--);
//            }
//        }
//    }

    // Поиск интервалов похожести для выбора наилучшего слияния списка
    // (выбирается интервал с большим весом - weight)
    QList<MessageData>::const_reverse_iterator iOldMsgs;
    QList<MessageData>::const_reverse_iterator iNewMsgs;
    QList<QPair<int,int> > intervals;
    int start = -1;
    int weight = 0;
    int spaceMsgCount = 0;
    bool flagEnterInterval = false;

    if (!newMsgList.isEmpty() && !oldMsgList.isEmpty()) {
        for (iNewMsgs = newMsgList.crbegin(), iOldMsgs = oldMsgList.crbegin();
             iNewMsgs != newMsgList.crend() && iOldMsgs != oldMsgList.crend();)
        {
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
    } else {
        if (m_params->flagLogging()) {
            if (newMsgList.isEmpty()) {
                addToLog(QString("Warrning. New message list is empty!"));
            }
            if (oldMsgList.isEmpty()) {
                addToLog(QString("Notification. "
                    "Old message list is empty, no merge, load all messages."));
            }
        }
        if (oldMsgList.isEmpty()) {
            mergedMsgList.append(newMsgList);
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

    // Спам-пакет из пачки сообщений т.к. не найдено совпадений вообще!
    // (если такое возможно) -> передаем его сразу в запись
    if (maxInterval.first == -1) {
        mergedMsgList = newMsgList;
        if (m_params->flagLogging()) {
            addToLog(QString("Warrning. New message list is very big(%1 msgs)! "
                "No matches found in the database.").arg(newMsgList.size()));
        }
        return;
    }

    // Отсутствуют новые сообщения!
    if (maxInterval.first == 0) {
        return;
    }

    // Записываем новые сообщения в список слияния
    int startIndex = newMsgList.size() - maxInterval.first;
    mergedMsgList.append(newMsgList.mid(startIndex));
    if (maxInterval.first != -1 &&
            mergedMsgList.size() > 10 &&
            m_params->flagLogging())
    {
        addToLog(QString("Warrning. New message list is big(%1 msgs)!")
                 .arg(newMsgList.size()));
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
        analyseText = analyseText.remove(
                    QRegularExpression("[\\x{1F600}-\\x{1F7FF}]+"));
        analyseText.replace("Zhivana", "Джеванна");
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
