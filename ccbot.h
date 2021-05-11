#ifndef CCBOT_H
#define CCBOT_H

#include <QObject>
#include <QtSql>

#include "ccbotengine.h"
#include "properties.h"
#include "enums.h"
#include "misc.h"
#include "messagedata.h"

const QString constNameBaseStr = "ccbot_storage.db";

class CCBot : public CCBotEngine
{
    Q_OBJECT

public:
    explicit CCBot(Properties *params, QObject *parent = nullptr);
    ~CCBot() override;

private:
    Properties *m_params;
    QSqlDatabase m_db;

    // сохр.\загр. настроек
    void loadSettings();
    void saveSettings();

    // методы нач инициализации
    void initTimers();              // инициализация таймеров
    void initConnections();         // инициализация связей
    void initTasks();               // инициализация задач

    // вспомогательные методы
    QString generateErrMsg(int type, int errCode, QString info = "");

    // методы распаковки данных
    bool readMessagesFromJsonStr(QByteArray jsonData, QList<MessageData> &msgList, QString *errInfo = nullptr);

    // методы для работы с БД
    Q_INVOKABLE bool openDB();
    Q_INVOKABLE void closeDB();
    bool createTableDB(QString streamId);
    bool existsTableDB(QString streamId);
    bool selectMsgsFromTableDB(QString streamId, QList<MessageData> &msgList, int limit = -1);
    bool appendMsgIntoTableDB(QString streamId, QList<MessageData> msgList);
    Q_INVOKABLE int insertNewMessagesInTable(QString streamId, QByteArray jsonData, QString *errInfo = nullptr);

    // вспомогательные методы
    void mergeMessages(QList<MessageData> oldMsgList, QList<MessageData> newMsgList, QList<MessageData> &mergedMsgList);
    bool equalMessages(const MessageData& msg1, const MessageData& msg2);

    // CCBotEngine interface
public slots:
    void action(int type, QVariantList args) override;
    void slotFinishedTask(long id, int type, QVariantList argsList, QVariant result) override;

signals:
    void showMessage(QString title, QString text, bool alert);
};

#endif // CCBOT_H
