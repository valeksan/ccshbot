#include "ccbot.h"

#include <QSettings>
#include <QApplication>
#include <QDir>
#include <QUrl>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>

CCBot::CCBot(Properties *params, QObject *parent) : CCBotEngine(parent), m_params(params)
{
    loadSettings();

    initConnections();
    initTasks();
    initTimers();
}

CCBot::~CCBot()
{
    saveSettings();
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
    m_pCore->registerTask(CCBotTaskEnums::MergeChat, [=](QString streamId, QString messagesJsonStr) -> TaskResult {
        TaskResult result;
        // 1. Parse JSON datagram
        QJsonDocument jsonDoc;
        QJsonParseError parseError;
        jsonDoc = QJsonDocument::fromJson(messagesJsonStr.toUtf8(), &parseError);
        if(parseError.error != QJsonParseError::NoError) {
            QString info = QString("Parse error at %1:%2").arg(parseError.offset).arg(parseError.errorString());
            return TaskResult(CCBotErrEnums::ParseJson, info);
        }
        QJsonArray jsonArr = jsonDoc.array();
        // 2. Init variables from datagram
        //...
        return TaskResult();
    });
}

QString CCBot::generateErrMsg(int type, int errCode)
{
    Q_UNUSED(type)

    if(errCode == CCBotErrEnums::Ok) return "";
    if(errCode == CCBotErrEnums::NoInit) return tr("Задача не выполнялась, результат не инициализирован.");

    return tr("Неизвестная ошибка, нет описания.");
}

void CCBot::action(int type, QVariantList args)
{
    switch (type) {
    case CCBotTaskEnums::MergeChat:
        {
            QString streamId = args.value(0,"").toString();
            QString messagesJsonStr = args.value(1,"").toString();
            m_pCore->addTask(type, streamId, messagesJsonStr);
        }
        break;
    default:
        break;
    }

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
