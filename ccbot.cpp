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
    // 1. Авторизация
    m_pCore->registerTask(CCBotTaskEnums::Auth, [this](QVariantList args) -> TaskResult
    {
        qDebug() << "AUTH";
        qDebug() << "email:" << args.at(0).toString();
        qDebug() << "password:" << args.at(1).toString();

        // 1) Загрузка страницы в память программы


        auto loadPage = [=]() {
            QUrl url("https://crazycash.tv/login?return=%2F");
            QMetaObject::invokeMethod(m_page, static_cast<void (QWebEnginePage::*)(QUrl) >(&QWebEnginePage::load), Q_ARG(QUrl, url));
        };

        //m_page->lo

        //m_mutex.lock();
        bool loadOk = waitSignalAfterFunction(m_page, &QWebEnginePage::loadFinished, loadPage, 10000);
        //m_mutex.unlock();

        if(!loadOk)
            return TaskResult(CCBotErrEnums::UnlodPage);

//#ifdef DEBUG_SAVE_HTML_TO_TMP
//        m_page->toHtml([](const QString &result) {
//            qDebug() << "html:" << result;
//            qDebug() << result.size();
////            QFile file("cc_auth.htm");
////            QTextStream out(&file);
////            out.setCodec("UTF-8");
////            if(file.open(QFile::WriteOnly | QFile::Text)) {
////                out << result;
////            }
////            file.close();
//        });
//#endif

        // fin
        return TaskResult();
    }, 0);
}

QString CCBot::generateErrMsg(int type, int errCode)
{
    if(errCode == CCBotErrEnums::Ok) return "";
    if(errCode == CCBotErrEnums::NoInit) return tr("Задача не выполнялась, результат не инициализирован.");

    switch (type) {
    case CCBotTaskEnums::Auth:
        switch (errCode) {
        case CCBotErrEnums::AuthDenied: return tr("Отказано в доступе, неверный логин или пароль.");
        case CCBotErrEnums::UnlodPage: return tr("Не удалось подгрузить данные необходимые для авторизации от сервера (страница входа, скрипты).");
        case CCBotErrEnums::UncorrectPage: return tr("Получена некорректная страница входа от сервера, возможно сервер не доступен, либо сервис изменил функционал входа.");
        default:
            break;
        }
        break;
    default:
        break;
    }

    return tr("Неизвестная ошибка, нет описания.");
}

void CCBot::action(int id, QVariantList args)
{
    m_pCore->addTask(id, args);
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
