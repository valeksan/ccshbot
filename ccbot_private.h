#ifndef CCBOT_PRIVATE_H
#define CCBOT_PRIVATE_H

#include <QObject>
#include <QMutex>
#include <QSqlDatabase>
#include <QMediaPlayer>

#include "core.h"
#include "properties.h"
#include "enums.h"
#include "misc.h"
#include "messagedata.h"

class TaskResult {
public:
    TaskResult() { }
    TaskResult(int errCode, QString errInfo = "", QVariant data = QVariant()) :
        m_errCode(errCode), m_errInfo(errInfo), m_data(data)
    {
    }

    int errCode() const
    {
        return m_errCode;
    }

    QString errInfo() const
    {
        return m_errInfo;
    }

private:
    int m_errCode = CCBotErrEnums::Ok;
    QString m_errInfo = "";
    QVariant m_data;

};

class CCBotPrivate : public QObject
{
    Q_OBJECT

protected:
    Core *m_pCore;
    Properties *m_params;
    QSqlDatabase m_db;
    QMediaPlayer *m_player;

    mutable QMutex m_mutex;

    void updateChat(const QList<MessageData> &msgsl, bool withTime = true, QString timeFormat = "hh:mm");
    void analyseNewMessages(const QList<MessageData> &msgsl);

    bool checkAutoVoiceMessage(const MessageData &msg, QString &text);
    bool checkCmdMessage(const MessageData &msg, QString &cmd, QStringList &args);

    // helper methods
    void mergeMessages(QList<MessageData> oldMsgList, QList<MessageData> newMsgList, QList<MessageData> &mergedMsgList);
    bool equalMessages(const MessageData& msg1, const MessageData& msg2);
    QString generateErrMsg(int type, int errCode, QString info = "");
    void generateEmotionText(QString &text);

    // data unpacking methods
    bool readMessagesFromJsonStr(QByteArray jsonData, QList<MessageData> &msgList, QString *errInfo = nullptr);

    // methods for working with the database
    bool createTableDB(QString streamId);
    bool existsTableDB(QString streamId);
    bool selectMsgsFromTableDB(QString streamId, QList<MessageData> &msgList, int limit = -1);
    bool appendMsgIntoTableDB(QString streamId, QList<MessageData> &msgList);

public:
    explicit CCBotPrivate(QObject *parent = nullptr);

public slots:
    virtual void action(int id, QVariantList args = QVariantList()) {
        Q_UNUSED(id)
        Q_UNUSED(args)
    }
    virtual void slotFinishedTask(long id, int type, QVariantList argsList, QVariant result) {
        Q_UNUSED(result)
        Q_UNUSED(type)
        Q_UNUSED(argsList)
        Q_UNUSED(id)
    }
    virtual void slotTerminatedTask(long id, int type, QVariantList argsList)
    {
        Q_UNUSED(type)
        Q_UNUSED(argsList)
        Q_UNUSED(id)
    }
    virtual void slotStartedTask(long id, int type, QVariantList argsList)
    {
        Q_UNUSED(type)
        Q_UNUSED(argsList)
        Q_UNUSED(id)
    }

signals:
    void showChatMessage(QString message);
};

Q_DECLARE_METATYPE(TaskResult)

#endif // CCBOT_PRIVATE_H
