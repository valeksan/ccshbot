#ifndef CCBOT_H
#define CCBOT_H

#include <QObject>
#include <QWebEngineProfile>
#include <QWebEnginePage>

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
    QWebEngineProfile *m_profile;
    QWebEnginePage *m_page;
    QString m_currentHtml;

    // сохр.\загр. настроек
    void loadSettings();
    void saveSettings();

    // методы нач инициализации
    void initBrowser();             // инициализация браузера
    void initTimers();              // инициализация таймеров
    void initConnections();         // инициализация связей
    void initTasks();               // инициализация задач

    // вспомогательные методы
    QString generateErrMsg(int type, int errCode);

    // CCBotEngine interface
public slots:
    void action(int id, QVariantList args) override;
    void loadPage(QUrl url);
    void slotFinishedTask(long id, int type, QVariantList argsList, QVariant result) override;

signals:
    void showMessage(QString title, QString text, bool alert);
    void pageReaded();
};

#endif // CCBOT_H
