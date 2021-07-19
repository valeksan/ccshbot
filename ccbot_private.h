#ifndef CCBOT_PRIVATE_H
#define CCBOT_PRIVATE_H

#include <QObject>
#include <QMutex>
#include <QSqlDatabase>
#include <QMediaPlayer>
#include <QJsonDocument>

#include "core.h"
#include "properties.h"
#include "enums.h"
#include "misc.h"
#include "messagedata.h"
#include "logmaker.h"
#include "console.h"
#include "speechkit_tts.h"

class TaskResult {
public:
    TaskResult() { }
    TaskResult(int errCode, QString errInfo = "", QVariant data = QVariant()) :
        m_errCode(errCode), m_errInfo(errInfo), m_data(data)
    {}

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
    LogMaker m_log;
    QJsonDocument m_dataToReplaceTextForVoice;
    QMap<QString, QStringList> m_mapListType3SendersOld;
    QMap<QString, QString> m_mapListType3SendersOldExpire;
    QMap<QString, bool> m_mapSubscribeUserNotified;
    Console *m_consoleInput;
    QNetworkReply::NetworkError m_errType = QNetworkReply::NoError;
    QList<QSslError> m_errorsSsl;

    mutable QMutex m_mutex;

    bool startLog();
    void updateChat(const QList<MessageData> &msgsl, bool withTime = true, QString timeFormat = "hh:mm", bool history = false);
    void analyseNewMessages(const QList<MessageData> &msgsl);

    bool checkAutoVoiceMessage(const MessageData &msg, QString &text, bool drunked = false);
    QString getReplaceWordForVoice(QString keyword);
    bool checkCmdMessage(const MessageData &msg, QString &cmd, QStringList &args);

    // helper methods
    void mergeMessages(QList<MessageData> oldMsgList, QList<MessageData> newMsgList, QList<MessageData> &mergedMsgList);
    bool equalMessages(const MessageData& msg1, const MessageData& msg2);
    QString generateErrMsg(int type, int errCode, QString info = "");
    bool isValidVoiceName(const QString name);
    QString getLangByVoiceName(const QString name);

    // data unpacking methods
    bool readMessagesFromJsonStr(QByteArray jsonData, QList<MessageData> &msgList, QString *errInfo = nullptr);

    // methods for working with the database
    bool createTableDB(QString streamId);
    bool existsTableDB(QString streamId);
    bool removeMessagesBannedUserFromTableDB(QString streamId, QString user);
    bool selectMsgsFromTableDB(QString streamId, QList<MessageData> &msgList, int limit = -1);
    bool appendMsgIntoTableDB(QString streamId, QList<MessageData> &msgList);
    // - box database
    bool createBoxTableInDB();
    bool boxContainUser(QString nikname, bool &contain);
    bool boxRegisterNewUser(QString nikname);
    bool boxGetStatisticsOfMessage(QString nikname, quint64 &numMessages, quint64 &numSymbols);
    bool boxAddStatisticsOfMessage(QString nikname, int numSymbolsToAdd);
    bool boxGetBalanceInfo(QString nikname, double &donation, double &balance);
    bool boxAddBalance(QString nikname, double cash, bool bonus = false);
    bool boxSpendBalace(QString nikname, double cash, bool &isEmptyMoney);
    bool boxGetStatisticsOfSpeech(QString nikname, quint64 &numSpeechSymbols);
    bool boxAddNumSpeechSymbolsInStatistics(QString nikname, int numSymbolsAdds);
    bool boxCalculatePriceForSpeech(QString nikname, int numSpeechSymbols, double &price, QString *voiceIn = nullptr);
    bool boxSetUserVoice(QString nikname, QString voice);
    bool boxGetUserVoice(QString nikname, QString &voice);
    bool boxSetUserSpeedVoice(QString nikname, QString speed);
    bool boxGetUserSpeedVoice(QString nikname, QString &speed);
    bool boxSetUserEmotionVoice(QString nikname, QString emotion);
    bool boxGetUserEmotionVoice(QString nikname, QString &emotion);
    bool boxGetAchieveList(QString nikname, QStringList &achieves);
    bool boxAddAchieve(QString nikname, QString achieve);
    bool boxSetUserInfo(QString nikname, QString info);
    bool boxGetUserInfo(QString nikname, QString &info);
    bool boxGetFlags(QString nikname, quint32 &flags);
    bool boxSetFlag(QString nikname, int flag, int st);
    void boxUpdate(const QList<MessageData> &newMsgs);
    bool boxGetReservKeyValue(QString nikname, QString key, QString &value, bool all = false);
    bool boxSetReservKeyValue(QString nikname, QString key, QString value);

    //cmds
    bool commandDrink(QStringList &args, const QString target, bool isStreamer = false);
    bool commandVoice(QStringList &args, const QString target, bool isStreamer = false);

public:
    explicit CCBotPrivate(QObject *parent = nullptr);

public slots:
    void addToLog(QString text, bool isTimelined = true);
    void addWordPairToReplaceForVoice(QString keyword, QString word);
    void editRepitWordForVoice(QString keyword, QString oldWord, QString newWord);
    void removeRepWordForVoice(QString keyword, QString word);
    void removeRepKeywordForVoice(QString keyword);
    void downSwapRepKeywordForVoice(int index);
    void upSwapRepKeywordForVoice(int index);
    QString getWordPairListInJson(bool compact = true);
    const QString getAppDataDirPath();

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
    void showChatNotification(QString info);
    void showHistoryMessage(QString message);
    void sendChatMessage(QString text);
};

Q_DECLARE_METATYPE(TaskResult)

#endif // CCBOT_PRIVATE_H
