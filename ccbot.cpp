#include "ccbot.h"

#include <QSettings>
#include <QApplication>
#include <QDir>
#include <QWebEngineSettings>
#include <QUrl>
#include <QDebug>

CCBot::CCBot(Properties *params, QObject *parent) : CCBotEngine(parent), m_params(params)
{
    loadSettings();

    initBrowser();
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

void CCBot::initBrowser()
{
    // init profile
    m_profile = new QWebEngineProfile();
    m_profile->setDownloadPath(QDir::tempPath());

    // init page
    m_page = new QWebEnginePage(m_profile);
    m_page->settings()->setAttribute(QWebEngineSettings::AutoLoadImages, false);
    m_page->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
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
    //...
}

QString CCBot::generateErrMsg(int type, int errCode)
{
    if(errCode == CCBotErrEnums::Ok) return "";
    if(errCode == CCBotErrEnums::NoInit) return tr("Задача не выполнялась, результат не инициализирован.");

    return tr("Неизвестная ошибка, нет описания.");
}

void CCBot::action(int id, QVariantList args)
{
    m_pCore->addTask(id, args);
}

void CCBot::loadPage(QUrl url)
{
    m_page->load(url);
}

void CCBot::slotFinishedTask(long id, int type, QVariantList argsList, QVariant result)
{
    Q_UNUSED(id)

    auto taskResult = result.value<TaskResult>();
    QVariantList args = argsList.value(0).toList();
    int errCode = taskResult.errCode();

    if(errCode != CCBotErrEnums::Ok) {
        emit showMessage(tr("Ошибка"), generateErrMsg(type, errCode), true);
    }
}
