#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <QObject>

#include "enums.h"

class Properties : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool pageLoaded READ pageLoaded WRITE setPageLoaded NOTIFY pageLoadedChanged)
    Q_PROPERTY(int pageType READ pageType WRITE setPageType NOTIFY pageTypeChanged)

    bool m_pageLoaded;
    int m_pageType;

public:
    explicit Properties(QObject *parent = nullptr) : QObject(parent),
        m_pageLoaded(false),
        m_pageType(PageTypeEnums::NoPage)
    {
        //
    }

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
};

#endif // PROPERTIES_H
