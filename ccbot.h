#ifndef CCBOT_H
#define CCBOT_H

#include <QObject>

#include "ccbotengine.h"
#include "properties.h"
#include "enums.h"
#include "misc.h"

class CCBot : public CCBotEngine
{
    Q_OBJECT

public:
    explicit CCBot(Properties *params, QObject *parent = nullptr);
    ~CCBot() override;

private:
    Properties *m_params;

    // сохр.\загр. настроек
    void loadSettings();
    void saveSettings();

    // методы нач инициализации
    void initTimers();              // инициализация таймеров
    void initConnections();         // инициализация связей
    void initTasks();               // инициализация задач

    // вспомогательные методы
    QString generateErrMsg(int type, int errCode);

    // CCBotEngine interface
public slots:
    void action(int type, QVariantList args) override;
    void slotFinishedTask(long id, int type, QVariantList argsList, QVariant result) override;

signals:
    void showMessage(QString title, QString text, bool alert);
};

#endif // CCBOT_H
