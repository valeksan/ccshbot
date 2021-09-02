#ifndef TTSMANAGER_H
#define TTSMANAGER_H

#include <QQueue>
#include <QMediaPlayer>
#include <QTextToSpeech>
#include <unordered_map>

#include "speechkit_tts.h"

using TaskID = quint64;

class TTSManager : public QObject
{
    Q_OBJECT
public:
    explicit TTSManager(QObject *parent = nullptr);

    enum TypeTTS {
        System = 0,
        SpeechKit,
    };

    enum ErrorType {
        FailPlay,
        FailLoad,
    };

    struct Task {
        quint64 id;
        TypeTTS tts;
        QString text;
        QVariant options;
        QString sourceAudio;
        bool ready = false;
    };

    struct SpeechKitConfig {
        QString tokenOauth;
        QString folderId;
        QString format;
        QString sampleRateHertz;
        QString extHost;
        QString extHostGetImToken;
    };

    void initTTS(TypeTTS type, QVariant config);

    static QStringList availableSystemTTSEngines();
    static Task makeTask(TypeTTS tts, QString text, QVariant options);

public slots:
    void addTask(Task task);

signals:
    void fail(quint64 id, int type, const QString info);
    void complete(quint64 id);

private:
    bool m_isEmpty = true;
    quint64 m_currentId = 0;
    Task m_currentTask;
    QQueue<TaskID> m_taskQueue;
    std::unordered_map<TaskID, TTSManager::Task> m_tasks;
    QMediaPlayer *m_pVoicePlayer;
    SpeechkitTTS *m_pSpeechKitTTS;
    QTextToSpeech *m_pSystemTTS;

    static quint64 lastId;

    void initSystemTTS();
    void initSpeechKitTTS(const SpeechKitConfig &config);
    void initConnections();

    bool nextTask();
    void clearCurrentTmpData();
};

Q_DECLARE_METATYPE(TTSManager::Task)
Q_DECLARE_METATYPE(TTSManager::SpeechKitConfig)

#endif // TTSMANAGER_H
