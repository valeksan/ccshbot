#ifndef CCBOT_H
#define CCBOT_H

#include <QObject>
#include <QtSql>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QtMultimedia/QMediaPlayer>

#include "ccbot_private.h"

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

class CCBot : public CCBotPrivate
{
    Q_OBJECT

public:
    explicit CCBot(Properties *params, QObject *parent = nullptr);
    ~CCBot() override;
    void start();                   // Beginning of work

private:
    // initialization methods
    void initDB();                  // database initialization
    void initTimers();              // initializing timers
    void initConnections();         // initialization of connections
    void initSysCommands();         // registration system commands
    void initTasks();               // initialization of tasks

private slots:
    int insertNewMessagesInTable(QString streamId, QByteArray jsonData, bool mergeOnly = true, QString *errInfo = nullptr);
    void speechFile(QString filename);

public slots:
    void openLogDir();
    bool openDB(QString name = "");
    QStringList getModelAvaibleHistoryNiknames();
    QStringList getModelAvaibleHistoryStreamsByNikname(QString nikname);
    void displayChatHistory(QString nikname, QString streamId);
    bool isOpenedDB();
    void closeDB();
    void action(int type, QVariantList args = QVariantList()) override;
    void exec(QString command);
    void slotFinishedTask(long id, int type, QVariantList argsList, QVariant result) override;

    // save \ load settings
    void loadSettings();
    void saveSettings();

signals:
    void showMessage(QString title, QString text, bool alert);
    void completeRequestGetIamToken();
    void completeRequestGetAudio();
    void completePlayFile();
};

#endif // CCBOT_H
