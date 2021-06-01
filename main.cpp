#include <QApplication>
#include <QQmlApplicationEngine>
#include <QIcon>
#include <QFontDatabase>
#include <QFileInfo>
#include <QQmlContext>
#include <QDateTime>

#include "metadata.h"
#include "properties.h"
#include "ccbot.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
#endif

    QApplication app(argc, argv);
    QQmlApplicationEngine engine;

    app.setOrganizationName(ORGANIZATION_STRING);
    app.setOrganizationDomain(DOMAIN_STRING);
    app.setApplicationName(APP_NAME);
    app.setApplicationVersion(VERSION_STRING);
    app.setWindowIcon(QIcon(":/app.svg"));

    // default font for the entire application
    int id = QFontDatabase::addApplicationFont("qrc:/fonts/NotoSans-Regular.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).value(0, "");
    if(!family.isEmpty()) {
        app.setFont(QFont(family));
    }

    // Building the objects necessary for the bot
    Properties *properties = new Properties();

    // Building a bot object
    CCBot *ccbot = new CCBot(properties);

    // Reading the date and time of the current compilation
    QString build = QString("%1%2").arg(QLocale(QLocale::C).toDate(QString(__DATE__).simplified(), QLatin1String("MMM d yyyy")).toString("yyyyMMdd"))
    .arg( QString("%1%2%3%4%5%6").arg(__TIME__[0]).arg(__TIME__[1]).arg(__TIME__[3]).arg(__TIME__[4]).arg(__TIME__[6]).arg(__TIME__[7]) );
    QDateTime datetime = QDateTime::fromString(build, "yyyyMMddhhmmss");
    const QString releaseDate = datetime.toString("dd.MM.yyyy hh:mm:ss");

    // Passing functional objects to the QML engine
    // - Declaring QML Classes
    qmlRegisterUncreatableMetaObject(CCBotTaskEnums::staticMetaObject, "ccbot.tasks", 1, 0, "Task", "Access to enums - Tasks");

    engine.rootContext()->setContextProperty("releaseDate", releaseDate);
    engine.rootContext()->setContextProperty("ccbot", ccbot);
    engine.rootContext()->setContextProperty("properties", properties);

    // Loading the interface
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    // Start bot for correct view (show errors if any)
    ccbot->start();

    return app.exec();
}
