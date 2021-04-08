#ifndef CRAZYCASHAPI_H
#define CRAZYCASHAPI_H

#include <QObject>
#include <QWebEngineProfile>
#include <QWebEnginePage>
#include <QWebEngineSettings>

namespace PageTypeEnums {
    Q_NAMESPACE
    enum PageTypeEnum {
        NoPage = -1,
        AuthPage,
        SearchRoomPage,
        AcceptAdultContentPage,
        RoomPage,
    };
    Q_ENUMS(PageTypeEnum)
}

class CrazyCashApi : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool pageLoaded READ pageLoaded WRITE setPageLoaded NOTIFY pageLoadedChanged)
    Q_PROPERTY(int pageType READ pageType WRITE setPageType NOTIFY pageTypeChanged)

public:
    explicit CrazyCashApi(QObject *parent = nullptr);

    bool loadAuthPage();
    //bool auth(const QString username, const QString password);

    bool pageLoaded() const
    {
        return m_pageLoaded;
    }

    int pageType() const
    {
        return m_pageType;
    }

public slots:
    void setPageLoaded(bool pageLoaded)
    {
        if (m_pageLoaded == pageLoaded)
            return;

        m_pageLoaded = pageLoaded;
        emit pageLoadedChanged(m_pageLoaded);
    }

    void setPageType(int pageType)
    {
        if (m_pageType == pageType)
            return;

        m_pageType = pageType;
        emit pageTypeChanged(m_pageType);
    }

signals:
    void pageLoadedChanged(bool pageLoaded);
    void pageTypeChanged(int pageType);

private:
    QWebEngineProfile *m_profile;
    QWebEnginePage *m_page;

    bool m_pageLoaded;
    int m_pageType;
};

//Q_DECLARE_METATYPE(CrazyCashApi)

#endif // CRAZYCASHAPI_H
