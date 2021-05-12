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

    initConnections();
    initTasks();
    initTimers();

    if(!openDB()) {
        qDebug() << "DB not open!";
    }

//    QList<QString> oldMsgList = {
//        "tyki: можно прикрыть и танцевать",
//        "UskoGlazik: Короче решайте сами, чел не отвечает",
//        "tyki: так не поворачивайся",
//        "Qvery: не поворачивайся",
//        "ssmaster: сзади прикрой",
//        "Gbird: Nude with hands covering Pus",
//        "UskoGlazik: Так им же для дрочки",
//        "UskoGlazik: Им это и нужно",
//        "ssmaster: может он этого и хочет?)))",
//        "Gbird: look at the amount",
//        "Qvery: сомневаюсь что танеч важен)",
//        "ssmaster: чтобы тебе было ен удобно",
//        "UskoGlazik: Qvery, А что такое танеч?",
//        "UskoGlazik: А, бля, тупой я",
//        "UskoGlazik: Танец",
//        "lilichan1: ой шо це за интим",
//        "ssmaster: так видно бедра и все такое..",
//        "Qvery: UskoGlazik, это какоето национальное блюдо))",
//        "UskoGlazik: Qvery, Нашёл в гугле, это поэт",
//        "NancyVicious: Прочитер из какого ввиду города?",
//        "lilichan1: рука только и лифчик",
//        "Qvery: NancyVicious, приветик)",
//        "Gbird: :)",
//        "lilichan1: боком норм",
//        "lilichan1: задом не поворачивайся ахах",
//        "Gbird: oh yes",
//        "PUMBO22: задание дали. выполняй а нетрепись. не хочешь не выполняй. твои условия нах не нужны никому",
//        "UskoGlazik: Ну ебать",
//        "UskoGlazik: 115 долларов",
//        "lilichan1: потом прочитер апхза",
//        "NancyVicious: Qvery, здравствуйте:з",
//        "UskoGlazik: Ооо",
//        "UskoGlazik: На улице радуга у меня",
//        "UskoGlazik: Я пошёл на неё смотреть",
//        "UskoGlazik: пока",
//        "lilichan1: PUMBO22, странный чел",
//        "lilichan1: ой я не смотрю",
//        "lilichan1: ахах",
//        "AlexNek: lilichan1, опа",
//        "king768: wooooooow",
//        "lilichan1: AlexNek, привет извращенец",
//        "lilichan1: king768, hi hi",
//        "AlexNek: lilichan1, дарова, от извращенки слышу)))",
//        "DaybreakToday: Пиздец, расчехлили бабу",
//        "Gbird: WOW AMAZING",
//        "lilichan1: AlexNek, все, не могу писать, я дрочить",
//        "lilichan1: ахпзахп",
//        "AlexNek: lilichan1, ОРУ",
//        "PravDa: всем привет кого не видел",
//        "ssmaster: ну можно передом немного?) прикрытой",
//        "PravDa: не плохо собрали",
//        "AlexNek: PravDa, дарова, я кста тебя видел, ты стрим вел, неуспел написать..",
//        "lilichan1: 2 руками прикрой",
//        "DaybreakToday: Прочитер сидит как куколд)",
//        "PravDa: AlexNek, сегодня?",
//        "lilichan1: тупа советики",
//        "AlexNek: PravDa, да",
//        "NancyVicious: DaybreakToday, :D",
//        "PravDa: AlexNek, как картинка?",
//        "AlexNek: PravDa, неплохо, только света надо больше",
//        "UskoGlazik: Ты осторожнее",
//        "DaybreakToday: Если не завалишь её прям сейчас, то всё, ккуколд)",
//        "UskoGlazik: Я новосибирских знаю",
//        "123Aleks123: Вы их сексом ещё заставьте заняться",
//        "king768: give us a front view pls.",
//        "UskoGlazik: Приедет закопает тебя в лесу",
//        "UskoGlazik: тсс",
//        "Gbird: come closer and turn to us covering it with hands?",
//        "NancyVicious: AlexNek, лучше чем у нитопы?",
//        "PravDa: AlexNek, толлькл камера и обс, без телефона",
//        "AlexNek: NancyVicious, а причем тут нитопа?)",

//        "mintie: you need to turn around - but keep covered",
//        "Gbird: We love it",
//        "DaybreakToday: 123Aleks123, кто мешает трахаться по-дружески?)",
//        "Kv1215: Turn if she wants more tipssssss",
//        "123Aleks123: По дружески можно",
//        "mintie: вам нужно развернуться - но держитесь под прикрытием",
//        "Gbird: face us",
//        "Gbird: AWSOME",
//        "Gbird: :) :)",
//        "ssmaster: Prochiter а ты видел ее голую?",
//        "Gbird: thank you that was great",
//        "Gbird: PASS",
//        "ssmaster: да интересно",
//        "OneLove2021 задонатил $10!",
//        "lilichan1: эротика",
//        "UskoGlazik: Ну всё, пошла ёбка",
//        "lilichan1: best friend"
//    };

//    QList<QString> newMsgList = {
//        "lilichan1: AlexNek, привет извращенец",
//        "lilichan1: king768, hi hi",
//        "AlexNek: lilichan1, дарова, от извращенки слышу)))",
//        "DaybreakToday: Пиздец, расчехлили бабу",
//        "Gbird: WOW AMAZING",
//        "lilichan1: AlexNek, все, не могу писать, я дрочить",
//        "lilichan1: ахпзахп",
//        "AlexNek: lilichan1, ОРУ",
//        "PravDa: всем привет кого не видел",
//        "ssmaster: ну можно передом немного?) прикрытой",
//        "PravDa: не плохо собрали",
//        "AlexNek: PravDa, дарова, я кста тебя видел, ты стрим вел, неуспел написать..",
//        "lilichan1: 2 руками прикрой",
//        "DaybreakToday: Прочитер сидит как куколд)",
//        "PravDa: AlexNek, сегодня?",
//        "lilichan1: тупа советики",
//        "AlexNek: PravDa, да",
//        "NancyVicious: DaybreakToday, :D",
//        "PravDa: AlexNek, как картинка?",
//        "AlexNek: PravDa, неплохо, только света надо больше",
//        "UskoGlazik: Ты осторожнее",
//        "DaybreakToday: Если не завалишь её прям сейчас, то всё, ккуколд)",
//        "UskoGlazik: Я новосибирских знаю",
//        "123Aleks123: Вы их сексом ещё заставьте заняться",
//        "king768: give us a front view pls.",
//        "UskoGlazik: Приедет закопает тебя в лесу",
//        "UskoGlazik: тсс",
//        "Gbird: come closer and turn to us covering it with hands?",
//        "NancyVicious: AlexNek, лучше чем у нитопы?",
//        "PravDa: AlexNek, толлькл камера и обс, без телефона",
//        "AlexNek: NancyVicious, а причем тут нитопа?)",

//        "mintie: you need to turn around - but keep covered",
//        "Gbird: We love it",
//        "DaybreakToday: 123Aleks123, кто мешает трахаться по-дружески?)",
//        "Kv1215: Turn if she wants more tipssssss",
//        "123Aleks123: По дружески можно",
//        "mintie: вам нужно развернуться - но держитесь под прикрытием",
//        "Gbird: face us",
//        "Gbird: AWSOME",
//        "Gbird: :) :)",
//        "ssmaster: Prochiter а ты видел ее голую?",
//        "Gbird: thank you that was great",
//        "Gbird: PASS",
//        "ssmaster: да интересно",
//        "OneLove2021 задонатил $10!",
//        "lilichan1: эротика",
//        "UskoGlazik: Ну всё, пошла ёбка",
//        "lilichan1: best friend",

//        "Gbird: She's very beautiful",
//        "PravDa: это плохо",
//        "lilichan1: тут приличный, а на сливах посмотреть можно и потом",
//        "UskoGlazik: Импотент получаеца",
//        "lilichan1: шутка",
//        "lilichan1: ахах",
//        "PravDa: у меня бы встал",
//        "lilichan1: и у меня",
//        "PravDa: хоть подругу",
//        "DaybreakToday: Прочитер что-то знает, чего не знаем мы",
//        "DaybreakToday: У неё усы под маской?",
//        "lilichan1: ставлю на бороду",
//        "DaybreakToday: Когда носа нет - это сифак",
//        "DaybreakToday: Сифилис в серьёзной стадии",
//        "DaybreakToday: Его сейчас лечат",
//        "DaybreakToday: Нос только у конченных отваливается",
//        "UskoGlazik: Слушай",
//        "AlexTr: Ребята на этом сайте в основном заказывают склеротические стримы так что решайте выходить ли здесь в эфир",
//        "UskoGlazik: Если это не разобьётся",
//        "UskoGlazik: То можно и выкинуть",
//        "DaybreakToday: Не, они на камерах не сидят)",
//        "DaybreakToday: AlexTr, какие?)",
//        "UskoGlazik: Я рекламы в тиктоке насмотреля, пойду оформлять себе брокерский счёт",
//        "Tankist089: Опять голожопы тут ахахахх",
//        "Gbird: lets do different challenge",
//        "AlexTr: эротическии",
//        "Tankist089: Я был цветной, перестал донатить просто",
//        "Tankist089: Хахаах,ну пасибо блин",
//        "Tankist089: Ахахахха",
//        "Tankist089: Вы не думали на Счёт отношений между друг другом?",
//        "Tankist089: Сделайте тише музыку",
//        "Tankist089: Слышал,но музыка все ровно перекрикивает",
//        "Tankist089: Зас норм уже)"
//    };

    /*
    QList<QString> oldMsgList = QString("1,2,3,4,5,6,7,1,2,3,6,7,n,a,d,5,6,7").split(',');
    QList<QString> newMsgList = QString("1,2,3,4,5,6,7,1,2,3,6,7,n,a,d,5,6,7,_,_,_,7").split(',');
    QList<QString> mergedMsgList;
    QList<QPair<int,int> > intervals;

    QList<QString>::const_reverse_iterator iOldMsgs;
    QList<QString>::const_reverse_iterator iNewMsgs;
    int start = -1;
    int weight = 0;
    int spaces = 0;
    bool flagEnterInterval = false;
    for (iNewMsgs = newMsgList.crbegin(), iOldMsgs = oldMsgList.crbegin(); iNewMsgs != newMsgList.crend() && iOldMsgs != oldMsgList.crend(); ) {
        if (*iNewMsgs == *iOldMsgs) {
            if(!flagEnterInterval) {
                qDebug() << "start" << *iNewMsgs << ":" << iNewMsgs - newMsgList.crbegin();
                start = iNewMsgs - newMsgList.crbegin();
                weight = 1;
                flagEnterInterval = true;
            } else {
                weight += 1;
            }
            ++iOldMsgs;
            ++iNewMsgs;
            if (iOldMsgs == oldMsgList.crend() || iNewMsgs == newMsgList.crend()) {
                intervals.append(QPair<int,int>(start, weight));
            }
        } else {
            if (flagEnterInterval) {
                qDebug() << "stop" << *iNewMsgs << ":" << iNewMsgs - newMsgList.crbegin();
                intervals.append(QPair<int,int>(start, weight));
                flagEnterInterval = false;
                iOldMsgs -= weight + spaces;
                spaces = 0;
            }
            ++iNewMsgs;
        }
    }
    qDebug() << intervals;

    // Выбор интервала
    QPair<int,int> maxInterval = QPair(-1, 0);
    for (const auto &interval : intervals) {
        if (interval.second > maxInterval.second) {
            maxInterval = interval;
        }
    }

    // Отсутствуют новые сообщения!
    if (maxInterval.first == 0) {
        qDebug() << "empty new";
        return;
    }

    // Записываем новые сообщения в список слияния
    int startIndex = newMsgList.size() - maxInterval.first;
    qDebug() << "maxInterval.first: " << maxInterval.first;
    qDebug() << "oldMsgList.size(): " << oldMsgList.size();
    qDebug() << "start_index: " << startIndex;
    //for (int i = startIndex; i < newMsgList.size(); i++) {
        mergedMsgList.append(newMsgList.mid(startIndex));
    //}

    qDebug() << "mergedMsgList: " << mergedMsgList.join(",");
//    for (int i = 0; i < mergedMsgList.size(); i++) {
//        qDebug() << mergedMsgList.at(i) << ", ";
//    }
*/
}

CCBot::~CCBot()
{
    saveSettings();

    closeDB();
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
    connect(this, &CCBotEngine::signQuit, [=]() {
        QApplication::quit();
    });
}

void CCBot::initTasks()
{
    m_pCore->registerTask(CCBotTaskEnums::MergeChat, [this](QString streamId, QString messagesJsonStr) -> TaskResult {
        TaskResult result;
        QString errInfo = "";
        int state = 0;
        m_mutex.lock();
        //QMetaObject::invokeMethod(this, "openDB", Qt::QueuedConnection, Q_RETURN_ARG(bool, isOpen));

        QMetaObject::invokeMethod(this, "insertNewMessagesInTable", Qt::BlockingQueuedConnection,
                                  Q_RETURN_ARG(int, state),
                                  Q_ARG(QString, streamId),
                                  Q_ARG(QByteArray, messagesJsonStr.toUtf8()),
                                  Q_ARG(QString*, &errInfo)
                                  );

        m_mutex.unlock();
        if(state != CCBotErrEnums::Ok) {
            return TaskResult(state, errInfo);
        }
        return TaskResult();
    });
}

QString CCBot::generateErrMsg(int type, int errCode, QString info)
{
    Q_UNUSED(type)

    if(errCode == CCBotErrEnums::Ok) return "";
    if(errCode == CCBotErrEnums::NoInit) return QString("Задача не выполнялась, результат не инициализирован.");
    if(errCode == CCBotErrEnums::ParseJson) return QString("Фатальная ошибка, не удалось распарсить JSON-данные.");
    if(errCode == CCBotErrEnums::Sql) return QString("Ошибка SQL: %1").arg(info);

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

void CCBot::closeDB()
{
    m_db.close();
}

bool CCBot::createTableDB(QString streamId)
{
    QSqlQuery qry;
    const QString values = "id INTEGER PRIMARY KEY AUTOINCREMENT, type INTEGER NOT NULL, sender TEXT, nik_color TEXT, msg TEXT, pay REAL, timestamp INTEGER";

    bool state = qry.exec(QString("CREATE TABLE IF NOT EXISTS 't_%1' (%2)").arg(streamId).arg(values));

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
            msg.msg = qry.value("msg").toString();
            msg.pay = qry.value("pay").toFloat();
            msg.timestamp = qry.value("timestamp").toUInt();
            msgList.append(msg);
        }
    }

    return state;
}

bool CCBot::appendMsgIntoTableDB(QString streamId, QList<MessageData> msgList)
{
    if (msgList.isEmpty()) {
        return true;
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

    return false;
}

int CCBot::insertNewMessagesInTable(QString streamId, QByteArray jsonData, QString *errInfo)
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
    if (!existsTableDB(streamId)) {
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
    state = selectMsgsFromTableDB(streamId, rowsFromDB, 100 + removeCount);
    if (!state) {
        if (errInfo) {
            *errInfo = m_db.lastError().text();
        }
        return CCBotErrEnums::Sql;
    }

    // 3. Слияние
    mergeMessages(rowsFromDB, rowsFromServer, rowsForInsert);

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
    //for(int i=0; i<newMsgList.size(); i++) qDebug() << "NML" << newMsgList.at(i).msg;

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
            if (iOldMsgs == oldMsgList.crend() || iNewMsgs == newMsgList.crend()) {
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
    QPair<int,int> maxInterval = QPair(-1, 0);
    for (const auto &interval : intervals) {
        if (interval.second > maxInterval.second) {
            maxInterval = interval;
        }
    }

    //qDebug() << "intervals:" << intervals;

    // Спам-пакет из пачки сообщений т.к. не найдено совпадений вообще! (если такое возможно) -> передаем его сразу в запись
    if (maxInterval.first == -1) {
        mergedMsgList = newMsgList;
        qDebug() << "spam!";
        return;
    }

    // Отсутствуют новые сообщения!
    if (maxInterval.first == 0) {
        qDebug() << "empty new";
        return;
    }

    // Записываем новые сообщения в список слияния
    int startIndex = newMsgList.size() - maxInterval.first;
    qDebug() << "stert_index: " << startIndex;
    //for (int i = startIndex; i < newMsgList.size(); i++) {
    mergedMsgList.append(newMsgList.mid(startIndex));
    //}
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
    QString errInfo = taskResult.errInfo();

    if (errCode != CCBotErrEnums::Ok) {
        emit showMessage(tr("Ошибка"), generateErrMsg(type, errCode, errInfo), true);
    }
}
