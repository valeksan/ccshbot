#include "crazycashapi.h"
#include "misc.h"

#include <QDir>
#include <QEventLoop>
#include <QDebug>

CrazyCashApi::CrazyCashApi(QObject *parent) : QObject(parent),
    m_pageLoaded(false), m_pageType(PageTypeEnums::NoPage)
{
    // init profile
    m_profile = new QWebEngineProfile(this);
    m_profile->setDownloadPath(QDir::tempPath());

    // init page
    m_page = new QWebEnginePage(m_profile);
    m_page->settings()->setAttribute(QWebEngineSettings::AutoLoadImages, false);
    m_page->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
}

bool CrazyCashApi::loadAuthPage()
{
    m_pageType = PageTypeEnums::AuthPage;

    bool loadOk = waitSignalAfterFunction(m_page, &QWebEnginePage::loadFinished, [this]() {
        m_page->load(QUrl("https://crazycash.tv/login?return=%2F"));
    }, 10000);

    if(loadOk)
        m_pageLoaded = true;
    else
        m_pageLoaded = false;

#ifdef DEBUG_SAVE_HTML_TO_TMP
    if(loadOk) {
        m_page->toHtml([](const QString &result) {
            qDebug() << "html:" << result << Qt::endl;
            qDebug() << result.size() << Qt::endl;
            QFile file("crazycash.htm");
            QTextStream out(&file);
            out.setCodec("UTF-8");
            if(file.open(QFile::WriteOnly | QFile::Text)) {
                out << result;
            }
            file.close();
        });
    }
#endif

    return false;
}
