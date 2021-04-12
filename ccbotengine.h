#ifndef CCBOTENGINE_H
#define CCBOTENGINE_H

#include <QObject>
#include <QMutex>

#include "core.h"

class CCBotEngine : public QObject
{
    Q_OBJECT

protected:
    Core *m_pCore;
    mutable QMutex mutex;

public:
    explicit CCBotEngine(QObject *parent = nullptr) : QObject(parent), m_pCore(new Core())
    {
        connect(m_pCore, &Core::finishedTask, this, &CCBotEngine::slotFinishedTask);
    }

    Q_INVOKABLE virtual void action(int id, QVariantList args = QVariantList()) {
        Q_UNUSED(id)
        Q_UNUSED(args)
    }

public slots:
    virtual void slotFinishedTask(long id, int type, QVariantList argsList, QVariant result) {
        Q_UNUSED(result)
        Q_UNUSED(type)
        Q_UNUSED(argsList)
        Q_UNUSED(id)
    }

signals:
    void signQuit();
};

#endif // CCBOTENGINE_H
