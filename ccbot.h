#ifndef CCBOT_H
#define CCBOT_H

#include <QObject>
#include <QtSql>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QtMultimedia/QMediaPlayer>

#include "ccbotengine.h"
#include "properties.h"
#include "enums.h"
#include "misc.h"
#include "messagedata.h"

#define constNameBaseStr                "ccbot_storage.db"
#define constTimeoutGetIamToken         5000
#define constTimeoutGetAudio            25000

#define defaultSpeechkitHost            "https://tts.api.cloud.yandex.net/speech/v1/tts:synthesize"
#define defaultSpeechkitGetIamTokenHost "https://iam.api.cloud.yandex.net/iam/v1/tokens"

#define defaultSpeechkitLang            "ru-RU"     // def "ru-RU" or: "en-US" or "tr-TR"
#define defaultSpeechkitFormat          ""          // def "oggopus" or: "lpcm" (wav)
#define defaultSpeechkitVoice           "filipp"    // def "oksana" or: (https://cloud.yandex.ru/docs/speechkit/tts/voices)
#define defaultSpeechkitEmotion         ""          // def "neutral" or: "good", "evil" (only RU: jane & omazh)
#define defaultSpeechkitSpeed           ""          // def "1.0" ("0.1" .. "3.0")
#define defaultSpeechkitSampleRateHertz ""          // def "48000" or: "16000", "8000"

class CCBot : public CCBotEngine
{
    Q_OBJECT

public:
    explicit CCBot(Properties *params, QObject *parent = nullptr);
    ~CCBot() override;

private:
    Properties *m_params;
    QSqlDatabase m_db;
    QMediaPlayer *m_player;
    //QNetworkAccessManager *m_speechkitMgr;

    // методы нач инициализации
    void initTimers();              // инициализация таймеров
    void initConnections();         // инициализация связей
    void initTasks();               // инициализация задач

    // вспомогательные методы
    QString generateErrMsg(int type, int errCode, QString info = "");

    // методы распаковки данных
    bool readMessagesFromJsonStr(QByteArray jsonData, QList<MessageData> &msgList, QString *errInfo = nullptr);

    // методы для работы с БД
    bool openDB();
    void closeDB();
    bool createTableDB(QString streamId);
    bool existsTableDB(QString streamId);
    bool selectMsgsFromTableDB(QString streamId, QList<MessageData> &msgList, int limit = -1);
    bool appendMsgIntoTableDB(QString streamId, QList<MessageData> &msgList);

    // вспомогательные методы
    void mergeMessages(QList<MessageData> oldMsgList, QList<MessageData> newMsgList, QList<MessageData> &mergedMsgList);
    bool equalMessages(const MessageData& msg1, const MessageData& msg2);

    // CCBotEngine interface
    void updateChat(const QList<MessageData> &msgsl, bool withTime = true, QString timeFormat = "hh:mm");
    void analyseNewMessages(const QList<MessageData> &msgsl);

    bool checkAutoVoiceMessage(const MessageData &msg, QString &text);
    bool checkCmdMessage(const MessageData &msg, QString &cmd, QStringList &args);

private slots:
    int insertNewMessagesInTable(QString streamId, QByteArray jsonData, bool merge = true, QString *errInfo = nullptr);
    void speechFile(QString filename);

public slots:
    void action(int type, QVariantList args = QVariantList()) override;
    void slotFinishedTask(long id, int type, QVariantList argsList, QVariant result) override;

    // сохр.\загр. настроек
    void loadSettings();
    void saveSettings();

signals:
    void showMessage(QString title, QString text, bool alert);
    void baseOpenned(bool state);
    void showChatMessage(QString message);
    void completeRequestGetIamToken();
    void completeRequestGetAudio();
    void completePlayFile();
};

#endif // CCBOT_H
