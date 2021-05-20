#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <QObject>
#include <QDateTime>

#include "enums.h"

class Properties : public QObject
{
    Q_OBJECT

    const char* defaultSpeechkitHost = "https://tts.api.cloud.yandex.net/speech/v1/tts:synthesize";
    const char* defaultSpeechkitGetIamTokenHost = "https://iam.api.cloud.yandex.net/iam/v1/tokens";
    const char* defaultSpeechkitLang = "";          // def "ru-RU" or: "en-US" or "tr-TR"
    const char* defaultSpeechkitFormat = "";        // def "oggopus" or: "lpcm" (wav)
    const char* defaultSpeechkitVoice = "filipp";   // def "oksana" or: (https://cloud.yandex.ru/docs/speechkit/tts/voices)
    const char* defaultSpeechkitEmotion = "";       // def "neutral" or: "good", "evil" (only RU: jane & omazh)
    const char* defaultSpeechkitSpeed = "";         // def "1.0" ("0.1" .. "3.0")
    const char* defaultSpeechkitSampleRateHertz = "";   // def "48000" or: "16000", "8000"

    Q_PROPERTY(bool flagLoadingChat READ flagLoadingChat WRITE setFlagLoadingChat NOTIFY flagLoadingChatChanged)
    Q_PROPERTY(QString currentStreamId READ currentStreamId WRITE setCurrentStreamId NOTIFY currentStreamIdChanged)
    Q_PROPERTY(bool listenClients READ listenClients WRITE setListenClients NOTIFY listenClientsChanged)

    Q_PROPERTY(bool flagAnalyseVoiceAllMsgType2 READ flagAnalyseVoiceAllMsgType2 WRITE setFlagAnalyseVoiceAllMsgType2 NOTIFY flagAnalyseVoiceAllMsgType2Changed)
    Q_PROPERTY(bool flagAnalyseVoiceAllMsgType0 READ flagAnalyseVoiceAllMsgType0 WRITE setFlagAnalyseVoiceAllMsgType0 NOTIFY flagAnalyseVoiceAllMsgType0Changed)

    Q_PROPERTY(QString speechkitLang READ speechkitLang WRITE setSpeechkitLang NOTIFY speechkitLangChanged)
    Q_PROPERTY(QString speechkitFormat READ speechkitFormat WRITE setSpeechkitFormat NOTIFY speechkitFormatChanged)
    Q_PROPERTY(QString speechkitVoice READ speechkitVoice WRITE setSpeechkitVoice NOTIFY speechkitVoiceChanged)
    Q_PROPERTY(QString speechkitEmotion READ speechkitEmotion WRITE setSpeechkitEmotion NOTIFY speechkitEmotionChanged)
    Q_PROPERTY(QString speechkitSpeed READ speechkitSpeed WRITE setSpeechkitSpeed NOTIFY speechkitSpeedChanged)
    Q_PROPERTY(QString speechkitSampleRateHertz READ speechkitSampleRateHertz WRITE setSpeechkitSampleRateHertz NOTIFY speechkitSampleRateHertzChanged)
    Q_PROPERTY(QString speechkitHost READ speechkitHost WRITE setSpeechkitHost NOTIFY speechkitHostChanged)
    Q_PROPERTY(QString speechkitOAuthToken READ speechkitOAuthToken WRITE setSpeechkitOAuthToken NOTIFY speechkitOAuthTokenChanged)
    Q_PROPERTY(QString speechkitFolderId READ speechkitFolderId WRITE setSpeechkitFolderId NOTIFY speechkitFolderIdChanged)
    Q_PROPERTY(QString speechkitIamToken READ speechkitIamToken WRITE setSpeechkitIamToken NOTIFY speechkitIamTokenChanged)
    Q_PROPERTY(QString speechkitGetIamTokenHost READ speechkitGetIamTokenHost WRITE setSpeechkitGetIamTokenHost NOTIFY speechkitGetIamTokenHostChanged)
    Q_PROPERTY(QDateTime speechkitIamTokenExpiryDate READ speechkitIamTokenExpiryDate WRITE setSpeechkitIamTokenExpiryDate NOTIFY speechkitIamTokenExpiryDateChanged)

    bool m_flagLoadingChat;
    QString m_currentStreamId;
    bool m_listenClients;
    bool m_flagAnalyseVoiceAllMsgType2;
    bool m_flagAnalyseVoiceAllMsgType0;

    QString m_speechkitLang;
    QString m_speechkitFormat;
    QString m_speechkitVoice;
    QString m_speechkitApiKey;
    QString m_speechkitHost;
    QString m_speechkitOAuthToken;
    QString m_speechkitFolderId;
    QString m_speechkitEmotion;
    QString m_speechkitSpeed;
    QString m_speechkitSampleRateHertz;
    QString m_speechkitIamToken;
    QDateTime m_speechkitIamTokenExpiryDate;
    QString m_speechkitGetIamTokenHost;

public:
    explicit Properties(QObject *parent = nullptr) : QObject(parent),
        m_flagLoadingChat(true),
        m_currentStreamId(""),
        m_listenClients(false),
        m_flagAnalyseVoiceAllMsgType2(false),
        m_flagAnalyseVoiceAllMsgType0(false),
        m_speechkitLang(defaultSpeechkitLang),
        m_speechkitFormat(defaultSpeechkitFormat),
        m_speechkitVoice(defaultSpeechkitVoice),
        m_speechkitHost(defaultSpeechkitHost),
        m_speechkitEmotion(defaultSpeechkitEmotion),
        m_speechkitSpeed(defaultSpeechkitSpeed),
        m_speechkitSampleRateHertz(defaultSpeechkitSampleRateHertz),
        m_speechkitIamTokenExpiryDate(QDateTime::currentDateTime()),
        m_speechkitGetIamTokenHost(defaultSpeechkitGetIamTokenHost)
    {
        //
        //setSpeechkitOAuthToken("AQAEA7qgv3XNAATuwSkvlrVl9UQApLZ9-QJ31cQ");
        //setSpeechkitFolderId("b1gbekic127evhkiqd2o");
    }

    bool flagLoadingChat() const;
    void setFlagLoadingChat(bool newFlagLoadingChat);

    const QString &currentStreamId() const;
    void setCurrentStreamId(const QString &newCurrentStreamId);

    bool listenClients() const;
    void setListenClients(bool newListenClients);

    bool flagAnalyseVoiceAllMsgType2() const;
    void setFlagAnalyseVoiceAllMsgType2(bool newFlagAnalyseVoiceAllMsgType2);

    bool flagAnalyseVoiceAllMsgType0() const;
    void setFlagAnalyseVoiceAllMsgType0(bool newFlagAnalyseVoiceAllMsgType0);

    const QString &speechkitLang() const;
    void setSpeechkitLang(const QString &newSpeechkitLang);

    const QString &speechkitFormat() const;
    void setSpeechkitFormat(const QString &newSpeechkitFormat);

    const QString &speechkitVoice() const;
    void setSpeechkitVoice(const QString &newSpeechkitVoice);

    const QString &speechkitHost() const;
    void setSpeechkitHost(const QString &newSpeechkitHost);

    const QString &speechkitOAuthToken() const;
    void setSpeechkitOAuthToken(const QString &newSpeechkitOAuthToken);

    const QString &speechkitFolderId() const;
    void setSpeechkitFolderId(const QString &newSpeechkitFolderId);

    const QString &speechkitEmotion() const;
    void setSpeechkitEmotion(const QString &newSpeechkitEmotion);

    const QString &speechkitSpeed() const;
    void setSpeechkitSpeed(const QString &newSpeechkitSpeed);

    const QString &speechkitSampleRateHertz() const;
    void setSpeechkitSampleRateHertz(const QString &newSpeechkitSampleRateHertz);

    const QString &speechkitIamToken() const;
    void setSpeechkitIamToken(const QString &newSpeechkitIamToken);

    const QDateTime &speechkitIamTokenExpiryDate() const;
    void setSpeechkitIamTokenExpiryDate(const QDateTime &newSpeechkitIamTokenExpiryDate);

    const QString &speechkitGetIamTokenHost() const;
    void setSpeechkitGetIamTokenHost(const QString &newSpeechkitGetIamTokenHost);

public slots:

signals:

    void flagLoadingChatChanged();
    void currentStreamIdChanged();
    void listenClientsChanged();
    void flagAnalyseVoiceAllMsgType2Changed();
    void flagAnalyseVoiceAllMsgType0Changed();
    void speechkitLangChanged();
    void speechkitFormatChanged();
    void speechkitVoiceChanged();
    void speechkitHostChanged();
    void speechkitOAuthTokenChanged();
    void speechkitFolderIdChanged();
    void speechkitEmotionChanged();
    void speechkitSpeedChanged();
    void speechkitSampleRateHertzChanged();
    void speechkitIamTokenChanged();
    void speechkitIamTokenExpiryDateChanged();
    void speechkitGetIamTokenHostChanged();
};

inline const QString &Properties::speechkitGetIamTokenHost() const
{
    return m_speechkitGetIamTokenHost;
}

inline void Properties::setSpeechkitGetIamTokenHost(const QString &newSpeechkitGetIamTokenHost)
{
    if (m_speechkitGetIamTokenHost == newSpeechkitGetIamTokenHost)
        return;
    m_speechkitGetIamTokenHost = newSpeechkitGetIamTokenHost;
    emit speechkitGetIamTokenHostChanged();
}

inline const QDateTime &Properties::speechkitIamTokenExpiryDate() const
{
    return m_speechkitIamTokenExpiryDate;
}

inline void Properties::setSpeechkitIamTokenExpiryDate(const QDateTime &newSpeechkitIamTokenExpiryDate)
{
    if (m_speechkitIamTokenExpiryDate == newSpeechkitIamTokenExpiryDate)
        return;
    m_speechkitIamTokenExpiryDate = newSpeechkitIamTokenExpiryDate;
    emit speechkitIamTokenExpiryDateChanged();
}

inline const QString &Properties::speechkitIamToken() const
{
    return m_speechkitIamToken;
}

inline void Properties::setSpeechkitIamToken(const QString &newSpeechkitIamToken)
{
    if (m_speechkitIamToken == newSpeechkitIamToken)
        return;
    m_speechkitIamToken = newSpeechkitIamToken;
    emit speechkitIamTokenChanged();
}

inline const QString &Properties::speechkitSampleRateHertz() const
{
    return m_speechkitSampleRateHertz;
}

inline void Properties::setSpeechkitSampleRateHertz(const QString &newSpeechkitSampleRateHertz)
{
    if (m_speechkitSampleRateHertz == newSpeechkitSampleRateHertz)
        return;
    m_speechkitSampleRateHertz = newSpeechkitSampleRateHertz;
    emit speechkitSampleRateHertzChanged();
}

inline const QString &Properties::speechkitSpeed() const
{
    return m_speechkitSpeed;
}

inline void Properties::setSpeechkitSpeed(const QString &newSpeechkitSpeed)
{
    if (m_speechkitSpeed == newSpeechkitSpeed)
        return;
    m_speechkitSpeed = newSpeechkitSpeed;
    emit speechkitSpeedChanged();
}

inline const QString &Properties::speechkitEmotion() const
{
    return m_speechkitEmotion;
}

inline void Properties::setSpeechkitEmotion(const QString &newSpeechkitEmotion)
{
    if (m_speechkitEmotion == newSpeechkitEmotion)
        return;
    m_speechkitEmotion = newSpeechkitEmotion;
    emit speechkitEmotionChanged();
}

inline const QString &Properties::speechkitFolderId() const
{
    return m_speechkitFolderId;
}

inline void Properties::setSpeechkitFolderId(const QString &newSpeechkitFolderId)
{
    if (m_speechkitFolderId == newSpeechkitFolderId)
        return;
    m_speechkitFolderId = newSpeechkitFolderId;
    emit speechkitFolderIdChanged();
}

inline const QString &Properties::speechkitOAuthToken() const
{
    return m_speechkitOAuthToken;
}

inline void Properties::setSpeechkitOAuthToken(const QString &newSpeechkitOAuthToken)
{
    if (m_speechkitOAuthToken == newSpeechkitOAuthToken)
        return;
    m_speechkitOAuthToken = newSpeechkitOAuthToken;
    emit speechkitOAuthTokenChanged();
}

inline const QString &Properties::speechkitHost() const
{
    return m_speechkitHost;
}

inline void Properties::setSpeechkitHost(const QString &newSpeechkitHost)
{
    if (m_speechkitHost == newSpeechkitHost)
        return;
    m_speechkitHost = newSpeechkitHost;
    emit speechkitHostChanged();
}

inline const QString &Properties::speechkitVoice() const
{
    return m_speechkitVoice;
}

inline void Properties::setSpeechkitVoice(const QString &newSpeechkitVoice)
{
    if (m_speechkitVoice == newSpeechkitVoice)
        return;
    m_speechkitVoice = newSpeechkitVoice;
    emit speechkitVoiceChanged();
}

inline const QString &Properties::speechkitFormat() const
{
    return m_speechkitFormat;
}

inline void Properties::setSpeechkitFormat(const QString &newSpeechkitFormat)
{
    if (m_speechkitFormat == newSpeechkitFormat)
        return;
    m_speechkitFormat = newSpeechkitFormat;
    emit speechkitFormatChanged();
}

inline const QString &Properties::speechkitLang() const
{
    return m_speechkitLang;
}

inline void Properties::setSpeechkitLang(const QString &newSpeechkitLang)
{
    if (m_speechkitLang == newSpeechkitLang)
        return;
    m_speechkitLang = newSpeechkitLang;
    emit speechkitLangChanged();
}

inline bool Properties::flagAnalyseVoiceAllMsgType0() const
{
    return m_flagAnalyseVoiceAllMsgType0;
}

inline void Properties::setFlagAnalyseVoiceAllMsgType0(bool newFlagAnalyseVoiceAllMsgType0)
{
    if (m_flagAnalyseVoiceAllMsgType0 == newFlagAnalyseVoiceAllMsgType0)
        return;
    m_flagAnalyseVoiceAllMsgType0 = newFlagAnalyseVoiceAllMsgType0;
    emit flagAnalyseVoiceAllMsgType0Changed();
}

inline bool Properties::flagAnalyseVoiceAllMsgType2() const
{
    return m_flagAnalyseVoiceAllMsgType2;
}

inline void Properties::setFlagAnalyseVoiceAllMsgType2(bool newFlagAnalyseVoiceAllMsgType2)
{
    if (m_flagAnalyseVoiceAllMsgType2 == newFlagAnalyseVoiceAllMsgType2)
        return;
    m_flagAnalyseVoiceAllMsgType2 = newFlagAnalyseVoiceAllMsgType2;
    emit flagAnalyseVoiceAllMsgType2Changed();
}

inline bool Properties::listenClients() const
{
    return m_listenClients;
}

inline void Properties::setListenClients(bool newListenClients)
{
    if (m_listenClients == newListenClients)
        return;
    m_listenClients = newListenClients;
    emit listenClientsChanged();
}

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
