#include "ccbot.h"

#include <QSettings>
#include <QApplication>

CCBot::CCBot(Properties *params, QObject *parent) : CCBotEngine(parent), m_params(params)
{
    m_profile = new QWebEngineProfile();

    loadSettings();
    initConnections();
    initTasks();
    initTimers();
}

CCBot::~CCBot()
{
    saveSettings();

    delete m_profile;
}

void CCBot::loadSettings()
{
    QSettings cfg;
    //...
}

void CCBot::saveSettings()
{
    QSettings cfg;
    //...
}

void CCBot::initTimers()
{
    //
}

void CCBot::initConnections()
{
    // соединение: завершение программы
    connect(this, &CCBotEngine::signQuit, [=]()
    {
        QApplication::quit();
    });
}

void CCBot::initTasks()
{
    //
}
