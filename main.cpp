#include <QApplication>
#include <QQmlApplicationEngine>
#include <QIcon>
#include <QFontDatabase>
#include <QFileInfo>
#include <QQmlContext>
#include <QDateTime>
#include <qtwebengineglobal.h>

#include "metadata.h"
#include "desktop.h"
#include "properties.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
#endif

    QtWebEngine::initialize();

    QApplication app(argc, argv);

    app.setOrganizationName(ORGANIZATION_STRING);
    app.setOrganizationDomain(DOMAIN_STRING);
    app.setApplicationName(QFileInfo(app.applicationFilePath()).baseName());
    app.setApplicationVersion(VERSION_STRING);

    // установка шрифта по умолчанию для всего приложения
    int id = QFontDatabase::addApplicationFont(":/fonts/NotoSans-Regular.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).value(0, "");
    if(!family.isEmpty()) {
        app.setFont(QFont(family));
    }

    // считывание даты и времени текущей компиляции
    QString build = QString("%1%2").arg(QLocale(QLocale::C).toDate(QString(__DATE__).simplified(), QLatin1String("MMM d yyyy")).toString("yyyyMMdd"))
    .arg( QString("%1%2%3%4%5%6").arg(__TIME__[0]).arg(__TIME__[1]).arg(__TIME__[3]).arg(__TIME__[4]).arg(__TIME__[6]).arg(__TIME__[7]) );
    QDateTime datetime = QDateTime::fromString(build, "yyyyMMddhhmmss");
    const QString releaseDate = datetime.toString("dd.MM.yyyy hh:mm:ss");

    // движок QML
    QQmlApplicationEngine engine;

    // Проброс функциональных объектов в движок QML
    // - Объявление QML-классов
    qmlRegisterSingletonType<Desktop>("Vip.Desktop", 1, 0, "Desktop", Desktop::desktopSingletonProvider);

    engine.rootContext()->setContextProperty("releaseDate", QVariant::fromValue(releaseDate));

    // Подгрузка интерфейса
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    return app.exec();
}
