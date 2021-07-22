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

    struct Task {
        quint64 id;
        TypeTTS tts;
        QString text;
        QVariant options;
        QString source;
        bool ready = false;
    };

public slots:
    void addTask(const Task task);

signals:

private:
    QQueue<TaskID> m_taskQueue;
    std::unordered_map<TaskID, TTSManager::Task> m_tasks;
    QMediaPlayer *m_pVoicePlayer;
    SpeechkitTTS *m_pSpeechKitTTS;

    static quint64 lastId;
};

Q_DECLARE_METATYPE(TTSManager::Task)

#endif // TTSMANAGER_H
