#ifndef SPEECHKITTTS_H
#define SPEECHKITTTS_H

#include <QObject>
#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariant>
#include <QUrlQuery>
#include <QTemporaryFile>
#include <QDataStream>

#define defaultTimeoutGetIamToken       5000
#define defaultTimeoutGetAudio          25000
#define defaultSpeechkitHost            "https://tts.api.cloud.yandex.net/speech/v1/tts:synthesize"
#define defaultSpeechkitGetIamTokenHost "https://iam.api.cloud.yandex.net/iam/v1/tokens"


class SpeechkitTTS : public QObject
{
    Q_OBJECT

public:
    static int timeoutGetIamToken;
    static int timeoutGetAudio;
    static QString hostGetImToken;
    static QString hostSpeechKit;

    explicit SpeechkitTTS(QObject *parent = nullptr, QString folderId = "", QString tokenOAuth = "");

    enum VoiceTaskType {
        UpdateToken,
        VoiceLoad,
    };

    enum ErrorType {
        NetworkError,
        ServiceError,
        SystemError
    };

    enum OptionType {
        Lang,
        Voice,
        Emotion,
        Speed,
    };

    enum Format {
        DefaultFormat = -1,
        OggOpus,
        LPCM
    };

    enum SampleRateHertz {
        DefaultSampleRateHertz = -1,
        sr48000hz,
        sr16000hz,
        sr8000hz
    };

    struct Options {
        QString voice = "";
        QString lang = "";
        QString speed = "";
        QString emotion = "";
        bool ssml = false;
    };

    struct Task {
        int type;
        QString text;
        Options options;
    };

    const QDateTime &tokenExpiry() const;
    void setTokenExpiry(const QDateTime &newTokenExpiry);

    const QString &tokenOAuth() const;
    void setTokenOAuth(const QString &newTokenOAuth);

    const QString &folderId() const;
    void setFolderId(const QString &newFolderId);

    const QString &tokenIm() const;
    void setTokenIm(const QString &newTokenIm);

    Format format() const;
    void setFormat(SpeechkitTTS::Format newFormat);

    SampleRateHertz sampleRateHertz() const;
    void setSampleRateHertz(SpeechkitTTS::SampleRateHertz newSampleRateHertz);

public slots:
    void voiceText(const QString text, const SpeechkitTTS::Options &options);
    void replyFinished(QNetworkReply *reply);

signals:
    void voiceComplete(QString filename);
    void voiceFail(int type, const QString info);

private:
    QNetworkAccessManager *m_pManager;
    QDateTime m_tokenExpiry;
    QString m_tokenIm;
    QString m_tokenOAuth;
    QString m_folderId;
    int m_format;
    int m_sampleRateHertz;

    void updateImTokenRequest(const QString text, const SpeechkitTTS::Options &options);
    void updateImTokenRequest(const SpeechkitTTS::Options &options);
    void voiceLoadRequest(const QString text, const SpeechkitTTS::Options &options);
    bool completeUpdateImTokenRequest(QNetworkReply *reply);
    void loadVoiceRequest(const QString text, const SpeechkitTTS::Options &options);
    bool completeLoadVoiceRequest(QNetworkReply *reply);

    QString getFormatOption();
    QString getSampleRateHertzOption();

    inline static bool isValidOption(int type, QString value);
    inline static QString getValidLangByVoice(QString voice);
};

Q_DECLARE_METATYPE(SpeechkitTTS::Options)
Q_DECLARE_METATYPE(SpeechkitTTS::Task)

#endif // SPEECHKITTTS_H
