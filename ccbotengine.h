#ifndef CCBOTENGINE_H
#define CCBOTENGINE_H

#include <QObject>
#include <QMutex>

#include "core.h"

#include "enums.h"

class TaskResult {
public:
    TaskResult() { }
    TaskResult(int errCode, QString errInfo = "", QVariant data = QVariant()) :
        m_errCode(errCode), m_errInfo(errInfo), m_data(data)
    {
    }

    int errCode() const
    {
        return m_errCode;
    }

    QString errInfo() const
    {
        return m_errInfo;
    }

private:
    int m_errCode = CCBotErrEnums::Ok;
    QString m_errInfo = "";
    QVariant m_data;

};

class CCBotEngine : public QObject
{
    Q_OBJECT

protected:
    Core *m_pCore;
    mutable QMutex m_mutex;

public:
    explicit CCBotEngine(QObject *parent = nullptr) : QObject(parent), m_pCore(new Core())
    {
        connect(m_pCore, &Core::finishedTask, this, &CCBotEngine::slotFinishedTask);
    }

public slots:
    virtual void action(int id, QVariantList args = QVariantList()) {
        Q_UNUSED(id)
        Q_UNUSED(args)
    }
    virtual void slotFinishedTask(long id, int type, QVariantList argsList, QVariant result) {
        Q_UNUSED(result)
        Q_UNUSED(type)
        Q_UNUSED(argsList)
        Q_UNUSED(id)
    }
    virtual void slotTerminatedTask(long id, int type, QVariantList argsList)
    {
        Q_UNUSED(type)
        Q_UNUSED(argsList)
        Q_UNUSED(id)
    }
    virtual void slotStartedTask(long id, int type, QVariantList argsList)
    {
        Q_UNUSED(type)
        Q_UNUSED(argsList)
        Q_UNUSED(id)
    }

signals:
    void signQuit();
};

Q_DECLARE_METATYPE(TaskResult)

#endif // CCBOTENGINE_H
