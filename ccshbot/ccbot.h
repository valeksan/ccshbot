#ifndef CCBOT_H
#define CCBOT_H

#include <QObject>
#include <QtSql>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QtMultimedia/QMediaPlayer>
#include <QJsonObject>
#include <QJsonDocument>

#include "ccbot_private.h"

#define constNameBaseStr                "ccbot_storage.db"

class CCBot : public CCBotPrivate
{
    Q_OBJECT

public:
    explicit CCBot(Properties *params, QObject *parent = nullptr);
    void start();                   // Beginning of work

private:
    // initialization methods
    void initComponents();          // init inline components
    void initDatabase();            // -- database sqlite3
    void initSpeechkitTts();        // -- SpeechKit TTS
    void initConnections();         // initialization of connections
    void initSysCommands();         // registration system commands
    void initTasks();               // initialization of tasks

    TTSManager::SpeechKitConfig getSpeechkitConfig();

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
    QString keyUpCommand();
    QString keyDownCommand();
    void slotFinishedTask(long id, int type, QVariantList argsList, QVariant result) override;

    // save \ load settings
    void loadSettings();
    void saveSettings(quint32 section = SaveSectionEnums::All, bool beforeExit = true);

    // anti-pirat
    const QString getRegistrationCode();
    const QString getActivationCode();
    void setActivationCode(QString keyFmt);
    bool verifyActivation();

signals:
    void showMessage(QString title, QString text, bool alert);
    void completeRequestGetIamToken();
    void completeRequestGetAudio();
    void completePlayFile();
};

#endif // CCBOT_H
