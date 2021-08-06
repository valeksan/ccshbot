#include <QApplication>
#include <QQmlApplicationEngine>
#include <QIcon>
#include <QFontDatabase>
#include <QFileInfo>
#include <QQmlContext>
#include <QDateTime>
#include <QMetaObject>

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

    app.setApplicationName(APP_NAME);
    app.setApplicationVersion(APP_VERSION);
    app.setOrganizationName(ORGANIZATION);
    app.setOrganizationDomain(DOMAIN);

    app.setWindowIcon(QIcon(":/app.svg"));

    // default font for the entire application
    const int id = QFontDatabase::addApplicationFont("qrc:/fonts/NotoSans-Regular.ttf");
    const QString family = QFontDatabase::applicationFontFamilies(id).value(0, "");
    if(!family.isEmpty()) {
        app.setFont(QFont(family));
    }

    // Building the objects necessary for the bot
    Properties *properties = new Properties();

    // Building a bot object
    CCBot *ccbot = new CCBot(properties);

    // Reading the date and time of the current compilation
    const QString build = QString("%1%2%3%4").arg(
        QLocale(QLocale::C).toDate(QString(__DATE__).simplified(), QLatin1String("MMM d yyyy")).toString("yyyyMMdd"),
        QString("%1").arg(__TIME__[0]) + QString("%2").arg(__TIME__[1]),//hh
        QString("%1").arg(__TIME__[3]) + QString("%2").arg(__TIME__[4]),//mm
        QString("%1").arg(__TIME__[6]) + QString("%2").arg(__TIME__[7]) //ss
    );

    const QString releaseDate = QDateTime::fromString(build, "yyyyMMddhhmmss")
            .toString("dd.MM.yyyy hh:mm:ss");

    engine.rootContext()->setContextProperty("releaseDate", releaseDate);
    engine.rootContext()->setContextProperty("ccbot", ccbot);
    engine.rootContext()->setContextProperty("properties", properties);

    // Passing functional objects to the QML engine
    // - Declaring QML Classes
    qmlRegisterUncreatableMetaObject(CCBotTaskEnums::staticMetaObject,
                                     "ccbot.enums",
                                     1, 0,
                                     "Task",
                                     "Access to enums & flags only");
    qmlRegisterUncreatableMetaObject(SpeakReasonEnums::staticMetaObject,
                                     "ccbot.enums",
                                     1, 0,
                                     "SpeakReason",
                                     "Access to enums & flags only");
    qmlRegisterUncreatableMetaObject(SaveSectionEnums::staticMetaObject,
                                     "ccbot.enums",
                                     1, 0,
                                     "SaveSection",
                                     "Access to enums & flags only");

    // Loading the interface
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    // Start bot for correct view (show errors if any)
    ccbot->start();

    return app.exec();
}
