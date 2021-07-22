#include "ttsmanager.h"

#include <QDebug>

quint64 TTSManager::lastId = 0;

TTSManager::TTSManager(QObject *parent) : QObject(parent)
{
    m_pSpeechKitTTS = new SpeechkitTTS(this);
    m_pVoicePlayer = new QMediaPlayer(this);

}

void TTSManager::addTask(const Task task)
{
    //
}
