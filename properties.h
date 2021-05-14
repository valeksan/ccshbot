#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <QObject>

#include "enums.h"

class Properties : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool flagLoadingChat READ flagLoadingChat WRITE setFlagLoadingChat NOTIFY flagLoadingChatChanged)
    Q_PROPERTY(QString currentStreamId READ currentStreamId WRITE setCurrentStreamId NOTIFY currentStreamIdChanged)

    bool m_flagLoadingChat;
    QString m_currentStreamId;

public:
    explicit Properties(QObject *parent = nullptr) : QObject(parent),
        m_flagLoadingChat(true),
        m_currentStreamId("")
    {
        //
    }

    bool flagLoadingChat() const;
    void setFlagLoadingChat(bool newFlagLoadingChat);

    const QString &currentStreamId() const;
    void setCurrentStreamId(const QString &newCurrentStreamId);

public slots:

signals:

    void flagLoadingChatChanged();
    void currentStreamIdChanged();
};

inline const QString &Properties::currentStreamId() const
{
    return m_currentStreamId;
}

inline void Properties::setCurrentStreamId(const QString &newCurrentStreamId)
{
    if (m_currentStreamId == newCurrentStreamId)
        return;
    m_currentStreamId = newCurrentStreamId;
    emit currentStreamIdChanged();
}

inline bool Properties::flagLoadingChat() const
{
    return m_flagLoadingChat;
}

inline void Properties::setFlagLoadingChat(bool newFlagLoadingChat)
{
    if (m_flagLoadingChat == newFlagLoadingChat)
        return;
    m_flagLoadingChat = newFlagLoadingChat;
    emit flagLoadingChatChanged();
}

#endif // PROPERTIES_H
