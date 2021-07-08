#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <QObject>
#include <QDateTime>
#include <QColor>

#include "enums.h"

class Properties : public QObject
{
    Q_OBJECT

    // test
    Q_PROPERTY(QString testStr READ testStr WRITE setTestStr NOTIFY testStrChanged)

    // Window
    Q_PROPERTY(int windowX READ windowX WRITE setWindowX NOTIFY windowXChanged)
    Q_PROPERTY(int windowY READ windowY WRITE setWindowY NOTIFY windowYChanged)
    Q_PROPERTY(int windowHeight READ windowHeight WRITE setWindowHeight NOTIFY windowHeightChanged)
    Q_PROPERTY(int windowWidth READ windowWidth WRITE setWindowWidth NOTIFY windowWidthChanged)

    // View
    Q_PROPERTY(QString fontNameForChat READ fontNameForChat WRITE setFontNameForChat NOTIFY fontNameForChatChanged)
    Q_PROPERTY(float fontPointSizeForChat READ fontPointSizeForChat WRITE setFontPointSizeForChat NOTIFY fontPointSizeForChatChanged)
    Q_PROPERTY(QColor textColorForChat READ textColorForChat WRITE setTextColorForChat NOTIFY textColorForChatChanged)

    // Server
    Q_PROPERTY(bool flagLoadingChat READ flagLoadingChat WRITE setFlagLoadingChat NOTIFY flagLoadingChatChanged)
    Q_PROPERTY(QString currentStreamId READ currentStreamId WRITE setCurrentStreamId NOTIFY currentStreamIdChanged)
    Q_PROPERTY(QString currentStreamerNikname READ currentStreamerNikname WRITE setCurrentStreamerNikname NOTIFY currentStreamerNiknameChanged)
    Q_PROPERTY(bool listenClients READ listenClients WRITE setListenClients NOTIFY listenClientsChanged)
    Q_PROPERTY(quint32 maxTimestampDiff READ maxTimestampDiff WRITE setMaxTimestampDiff NOTIFY maxTimestampDiffChanged)
    Q_PROPERTY(QString listenHost READ listenHost WRITE setListenHost NOTIFY listenHostChanged)
    Q_PROPERTY(quint32 listenPort READ listenPort WRITE setListenPort NOTIFY listenPortChanged)

    // Options
    Q_PROPERTY(int speakOptionReasonType READ speakOptionReasonType WRITE setSpeakOptionReasonType NOTIFY speakOptionReasonTypeChanged)
    Q_PROPERTY(bool flagAnalyseVoiceAllMsgType2 READ flagAnalyseVoiceAllMsgType2 WRITE setFlagAnalyseVoiceAllMsgType2 NOTIFY flagAnalyseVoiceAllMsgType2Changed)
    Q_PROPERTY(bool flagAnalyseVoiceAllMsgType0 READ flagAnalyseVoiceAllMsgType0 WRITE setFlagAnalyseVoiceAllMsgType0 NOTIFY flagAnalyseVoiceAllMsgType0Changed)
    Q_PROPERTY(bool flagLogging READ flagLogging WRITE setFlagLogging NOTIFY flagLoggingChanged)

    // Speechkit
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
    Q_PROPERTY(double speechKitPriceBySymbol READ speechKitPriceBySymbol WRITE setSpeechKitPriceBySymbol NOTIFY speechKitPriceBySymbolChanged)

    // Box settings (for chat interlocutors)
    Q_PROPERTY(double boxUserStartingBalance READ boxUserStartingBalance WRITE setBoxUserStartingBalance NOTIFY boxUserStartingBalanceChanged)
    Q_PROPERTY(bool boxDefaultOnFlag0 READ boxDefaultOnFlag0 WRITE setBoxDefaultOnFlag0 NOTIFY boxDefaultOnFlag0Changed)

    bool m_flagLoadingChat = true;
    QString m_currentStreamId;
    bool m_listenClients = false;
    bool m_flagAnalyseVoiceAllMsgType2 = false;
    bool m_flagAnalyseVoiceAllMsgType0 = false;
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
    int m_windowX;
    int m_windowY;
    int m_windowWidth;
    int m_windowHeight;
    quint32 m_maxTimestampDiff = 5;
    QString m_listenHost;
    quint32 m_listenPort;
    QString m_fontNameForChat;
    float m_fontPointSizeForChat;
    QColor m_textColorForChat;
    QString m_testStr;
    QString m_currentStreamerNikname;
    bool m_flagLogging = false;
    double m_boxUserStartingBalance = 0.0;
    bool m_boxDefaultOnFlag0 = false;
    double m_speechKitPriceBySymbol = 0.0;

    int m_speakOptionReasonType;

public:
    explicit Properties(QObject *parent = nullptr) : QObject(parent),
        m_speechkitIamTokenExpiryDate(QDateTime::currentDateTime())
    {}

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

    int windowX() const;
    void setWindowX(int newWindowX);

    int windowY() const;
    void setWindowY(int newWindowY);

    int windowWidth() const;
    void setWindowWidth(int newWindowWidth);

    int windowHeight() const;
    void setWindowHeight(int newWindowHeight);

    quint32 maxTimestampDiff() const;
    void setMaxTimestampDiff(quint32 newMaxTimestampDiff);

    const QString &listenHost() const;
    void setListenHost(const QString &newListenHost);

    quint32 listenPort() const;
    void setListenPort(quint32 newListenPort);

    const QString &fontNameForChat() const;
    void setFontNameForChat(const QString &newFontNameForChat);

    float fontPointSizeForChat() const;
    void setFontPointSizeForChat(float newFontPointSizeForChat);

    const QColor &textColorForChat() const;
    void setTextColorForChat(const QColor &newTextColorForChat);

    const QString &testStr() const;
    void setTestStr(const QString &newTestStr);

    const QString &currentStreamerNikname() const;
    void setCurrentStreamerNikname(const QString &newCurrentStreamerNikname);

    bool flagLogging() const;
    void setFlagLogging(bool newFlagLogging);

    double boxUserStartingBalance() const;
    void setBoxUserStartingBalance(double newBoxUserStartingBalance);

    bool boxDefaultOnFlag0() const;
    void setBoxDefaultOnFlag0(bool newBoxDefaultOnFlag0);

    double speechKitPriceBySymbol() const;
    void setSpeechKitPriceBySymbol(double newSpeechKitPriceBySymbol);

    int speakOptionReasonType() const;
    void setSpeakOptionReasonType(int newSpeakOptionReasonType);

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
    void windowXChanged();
    void windowYChanged();
    void windowWidthChanged();
    void windowHeightChanged();
    void maxTimestampDiffChanged();
    void listenHostChanged();
    void listenPortChanged();
    void fontNameForChatChanged();
    void fontPointSizeForChatChanged();
    void textColorForChatChanged();
    void testStrChanged();
    void currentStreamerNiknameChanged();
    void flagLoggingChanged();
    void boxUserStartingBalanceChanged();
    void boxDefaultOnFlag0Changed();
    void speechKitPriceBySymbolChanged();
    void speakOptionReasonTypeChanged();
};

inline int Properties::speakOptionReasonType() const
{
    return m_speakOptionReasonType;
}

inline void Properties::setSpeakOptionReasonType(int newSpeakOptionReasonType)
{
    if (m_speakOptionReasonType == newSpeakOptionReasonType)
        return;
    m_speakOptionReasonType = newSpeakOptionReasonType;
    emit speakOptionReasonTypeChanged();
}

inline double Properties::speechKitPriceBySymbol() const
{
    return m_speechKitPriceBySymbol;
}

inline void Properties::setSpeechKitPriceBySymbol(double newSpeechKitPriceBySymbol)
{
    if (qFuzzyCompare(m_speechKitPriceBySymbol, newSpeechKitPriceBySymbol))
        return;
    m_speechKitPriceBySymbol = newSpeechKitPriceBySymbol;
    emit speechKitPriceBySymbolChanged();
}

inline bool Properties::boxDefaultOnFlag0() const
{
    return m_boxDefaultOnFlag0;
}

inline void Properties::setBoxDefaultOnFlag0(bool newBoxDefaultOnFlag0)
{
    if (m_boxDefaultOnFlag0 == newBoxDefaultOnFlag0)
        return;
    m_boxDefaultOnFlag0 = newBoxDefaultOnFlag0;
    emit boxDefaultOnFlag0Changed();
}

inline double Properties::boxUserStartingBalance() const
{
    return m_boxUserStartingBalance;
}

inline void Properties::setBoxUserStartingBalance(double newBoxUserStartingBalance)
{
    if (qFuzzyCompare(m_boxUserStartingBalance, newBoxUserStartingBalance))
        return;
    m_boxUserStartingBalance = newBoxUserStartingBalance;
    emit boxUserStartingBalanceChanged();
}

inline bool Properties::flagLogging() const
{
    return m_flagLogging;
}

inline void Properties::setFlagLogging(bool newFlagLogging)
{
    if (m_flagLogging == newFlagLogging)
        return;
    m_flagLogging = newFlagLogging;
    emit flagLoggingChanged();
}

inline const QString &Properties::currentStreamerNikname() const
{
    return m_currentStreamerNikname;
}

inline void Properties::setCurrentStreamerNikname(const QString &newCurrentStreamerNikname)
{
    if (m_currentStreamerNikname == newCurrentStreamerNikname)
        return;
    m_currentStreamerNikname = newCurrentStreamerNikname;
    emit currentStreamerNiknameChanged();
}

inline const QString &Properties::testStr() const
{
    return m_testStr;
}

inline void Properties::setTestStr(const QString &newTestStr)
{
    if (m_testStr == newTestStr)
        return;
    m_testStr = newTestStr;
    emit testStrChanged();
}

inline const QColor &Properties::textColorForChat() const
{
    return m_textColorForChat;
}

inline void Properties::setTextColorForChat(const QColor &newTextColorForChat)
{
    if (m_textColorForChat == newTextColorForChat)
        return;
    m_textColorForChat = newTextColorForChat;
    emit textColorForChatChanged();
}

inline float Properties::fontPointSizeForChat() const
{
    return m_fontPointSizeForChat;
}

inline void Properties::setFontPointSizeForChat(float newFontPointSizeForChat)
{
    if (m_fontPointSizeForChat == newFontPointSizeForChat)
        return;
    m_fontPointSizeForChat = newFontPointSizeForChat;
    emit fontPointSizeForChatChanged();
}

inline const QString &Properties::fontNameForChat() const
{
    return m_fontNameForChat;
}

inline void Properties::setFontNameForChat(const QString &newFontNameForChat)
{
    if (m_fontNameForChat == newFontNameForChat)
        return;
    m_fontNameForChat = newFontNameForChat;
    emit fontNameForChatChanged();
}

inline quint32 Properties::listenPort() const
{
    return m_listenPort;
}

inline void Properties::setListenPort(quint32 newListenPort)
{
    if (m_listenPort == newListenPort)
        return;
    m_listenPort = newListenPort;
    emit listenPortChanged();
}

inline const QString &Properties::listenHost() const
{
    return m_listenHost;
}

inline void Properties::setListenHost(const QString &newListenHost)
{
    if (m_listenHost == newListenHost)
        return;
    m_listenHost = newListenHost;
    emit listenHostChanged();
}

inline quint32 Properties::maxTimestampDiff() const
{
    return m_maxTimestampDiff;
}

inline void Properties::setMaxTimestampDiff(quint32 newMaxTimestampDiff)
{
    if (m_maxTimestampDiff == newMaxTimestampDiff)
        return;
    m_maxTimestampDiff = newMaxTimestampDiff;
    emit maxTimestampDiffChanged();
}

inline int Properties::windowHeight() const
{
    return m_windowHeight;
}

inline void Properties::setWindowHeight(int newWindowHeight)
{
    if (m_windowHeight == newWindowHeight)
        return;
    m_windowHeight = newWindowHeight;
    emit windowHeightChanged();
}

inline int Properties::windowWidth() const
{
    return m_windowWidth;
}

inline void Properties::setWindowWidth(int newWindowWidth)
{
    if (m_windowWidth == newWindowWidth)
        return;
    m_windowWidth = newWindowWidth;
    emit windowWidthChanged();
}

inline int Properties::windowY() const
{
    return m_windowY;
}

inline void Properties::setWindowY(int newWindowY)
{
    if (m_windowY == newWindowY)
        return;
    m_windowY = newWindowY;
    emit windowYChanged();
}

inline int Properties::windowX() const
{
    return m_windowX;
}

inline void Properties::setWindowX(int newWindowX)
{
    if (m_windowX == newWindowX)
        return;
    m_windowX = newWindowX;
    emit windowXChanged();
}

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
