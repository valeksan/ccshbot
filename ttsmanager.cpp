#include "ttsmanager.h"

#include <QDebug>

quint64 TTSManager::lastId = 0;

TTSManager::TTSManager(QObject *parent) : QObject(parent)
{
    m_pSystemTTS = new QTextToSpeech(this);
    m_pSpeechKitTTS = new SpeechkitTTS(this);
    m_pVoicePlayer = new QMediaPlayer(this);

    initConnections();
}

void TTSManager::initTTS(TypeTTS type, QVariant config)
{
    switch (type) {
    case System:
        initSystemTTS();
        return;
    case SpeechKit:
        initSpeechKitTTS(config.value<SpeechKitConfig>());
        return;
    default:
        break;
    }
}

QStringList TTSManager::availableSystemTTSEngines()
{
    return QTextToSpeech::availableEngines();
}

TTSManager::Task TTSManager::makeTask(TypeTTS tts, QString text, QVariant options)
{
    Task newTask;

    newTask.tts = tts;
    newTask.text = text;
    newTask.options = options;

    switch (tts) {
    case TypeTTS::System:
        newTask.ready = true;
        break;
    case TypeTTS::SpeechKit:
        newTask.ready = false;
        break;
    }

    return newTask;
}

void TTSManager::addTask(Task task)
{
    task.id = (++lastId);

    qDebug() << "addTask" << task.id;

    if (m_isEmpty) {
        m_currentId = task.id;
        m_currentTask = task;
        m_isEmpty = false;
    } else {
        m_tasks.insert({task.id, task});
        m_taskQueue.enqueue(task.id);
    }

    switch (task.tts) {
    case System:
        if (m_currentId == task.id) {
            //... adds options
            m_pSystemTTS->say(task.text);
        }
        break;
    case SpeechKit:
        m_pSpeechKitTTS->makeAudioFile(task.id, task.text, task.options.value<SpeechkitTTS::Options>());
        break;
    }
}

void TTSManager::initSystemTTS()
{
    //...
}

void TTSManager::initSpeechKitTTS(const SpeechKitConfig &config)
{
    m_pSpeechKitTTS->setFolderId(config.folderId);
    m_pSpeechKitTTS->setTokenOAuth(config.tokenOauth);
    m_pSpeechKitTTS->setFormat(config.format);
    m_pSpeechKitTTS->setSampleRateHertz(config.sampleRateHertz);
    if (!config.extHostGetImToken.isEmpty()) {
        m_pSpeechKitTTS->hostGetImToken = config.extHostGetImToken;
    }
    if (!config.extHost.isEmpty()) {
        m_pSpeechKitTTS->hostSpeechKit = config.extHost;
    }
}

void TTSManager::initConnections()
{
    // voice task connect
    connect(m_pSystemTTS, &QTextToSpeech::stateChanged, [this](QTextToSpeech::State state) {
        if (state == QTextToSpeech::Ready) {
            emit complete(m_currentId);
            nextTask();
        }
    });
    connect(m_pVoicePlayer, &QMediaPlayer::stateChanged, [this](QMediaPlayer::State state) {
        if (state == QMediaPlayer::StoppedState) {
            emit complete(m_currentId);
            clearCurrentTmpData();
            nextTask();
        }
    });
    connect(m_pVoicePlayer, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error), [this](QMediaPlayer::Error error) {
        Q_UNUSED(error)
        emit fail(m_currentId, FailPlay, m_pVoicePlayer->errorString());
        emit complete(m_currentId);
        clearCurrentTmpData();
        nextTask();
    });
    // prepare task connect
    connect(m_pSpeechKitTTS, &SpeechkitTTS::complete, [this](quint64 id, QString filename) {
        qDebug() << "Task ready" << id << "OK";
        if (id == m_currentId) {
            qDebug() << " -> Task play" << id;
            m_currentTask.ready = true;
            m_currentTask.sourceAudio = filename;
            m_pVoicePlayer->setMedia(QUrl::fromLocalFile(filename));
            m_pVoicePlayer->play();
        } else {
            if (m_tasks.contains(id)) {
                m_tasks.at(id).ready = true;
                m_tasks.at(id).sourceAudio = filename;
            }
        }
    });
}

bool TTSManager::nextTask()
{
    if (m_taskQueue.isEmpty()) {
        m_isEmpty = true;
        return false;
    }

    m_currentId = m_taskQueue.dequeue();
    m_currentTask = m_tasks.at(m_currentId);
    m_tasks.erase(m_currentId);

    if (m_currentTask.ready) {
        if (m_currentTask.tts == SpeechKit) {
            m_pVoicePlayer->setMedia(QUrl::fromLocalFile(m_currentTask.sourceAudio));
            m_pVoicePlayer->play();
            qDebug() << " -> Task play" << m_currentId;
        } else if (m_currentTask.tts == System) {
            // ... // todo options
            m_pSystemTTS->say(m_currentTask.text);
        }
    }
    return true;
}

void TTSManager::clearCurrentTmpData()
{
    // clear prev task temp data
    switch (m_currentTask.tts) {
    case TTSManager::TypeTTS::SpeechKit:
        QFile::remove(m_currentTask.sourceAudio);
        qDebug() << "Remove" << m_currentTask.sourceAudio << m_currentId << "OK";
        break;
    default:
        break;
    }
}
