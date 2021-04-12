#ifndef CCBOT_H
#define CCBOT_H

#include <QObject>
#include <QWebEngineProfile>

#include "ccbotengine.h"
#include "properties.h"

class CCBot : public CCBotEngine
{
public:
    CCBot(Properties *params, QObject *parent = nullptr);
    ~CCBot();

private:
    Properties *m_params;
    QWebEngineProfile *m_profile;

    // сохр.\загр. настроек
    void loadSettings();
    void saveSettings();

    // методы нач инициализации
    void initTimers();              // инициализация таймеров
    void initConnections();         // инициализация связей
    void initTasks();               // инициализация задач
};

#endif // CCBOT_H
