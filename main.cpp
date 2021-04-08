#include <QApplication>
#include <QQmlApplicationEngine>
#include <QIcon>
#include <QFontDatabase>
#include <QFileInfo>
#include <QQmlContext>
#include <QDateTime>
#include <qtwebengineglobal.h>
//#include <QtWebEngineWidgets>

//#include <QDebug>

#include "metadata.h"
#include "desktop.h"

#include "crazycashapi.h"

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

    // API
    CrazyCashApi api;

    // Проброс функциональных объектов в движок QML
    // - Объявление QML-классов
    qmlRegisterSingletonType<Desktop>("Vip.Desktop", 1, 0, "Desktop", Desktop::desktopSingletonProvider);

    engine.rootContext()->setContextProperty("releaseDate", QVariant::fromValue(releaseDate));
    //engine.rootContext()->setContextProperty("api", QVariant::fromValue(api));

    // подгрузка страницы для авторизации
    api.loadAuthPage();

    // Подгрузка интерфейса
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    /*
    QWebEngineProfile *m_profile = new QWebEngineProfile;
    m_profile->setDownloadPath(QDir::tempPath());
    QWebEnginePage *page = new QWebEnginePage(m_profile);
    page->settings()->setAttribute(QWebEngineSettings::AutoLoadImages, false);
    page->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);

    QEventLoop *event = new QEventLoop;
    QObject::connect(page, &QWebEnginePage::loadFinished, event, &QEventLoop::quit);

    page->load(QUrl("https://crazycash.tv/login"));

    event->exec();

    page->toPlainText([&](const QString &result){
       qDebug() << "html:";
       qDebug() << result.size();
       //ui->textBrowser->append(result);
       QFile file("crazycash.htm");
       if(file.open(QFile::WriteOnly | QFile::Text)) {
           QTextStream out(&file);
           out << result;
           file.close();
       }
    });

    //QWebEngineView *view = new QWebEngineView();

//    QObject::connect(view->page(), &QWebEnginePage::loadFinished, [&view](bool state) {
//        QFile htmlFile("crazycash.html");
//        if(!htmlFile.open(QFile::WriteOnly | QFile::Truncate)) {
//            return;
//        }
//        //...
//        view->page()->toPlainText([&](const QString &text) {
//            QTextStream out(&htmlFile);
//            out.setCodec("UTF-8");
//            out << text;
//            //htmlFile.write(text.toLatin1());
//        });
//        htmlFile.close();
//    });

//    view->load(QUrl("https://crazycash.tv/login"));
//    view->show();

    */

    return app.exec();
}
