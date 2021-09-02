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
#include <QSettings>

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

    explicit SpeechkitTTS(QObject *parent = nullptr);

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
        Format,
        SampleRateHertz
    };

    struct Options {
        QString voice = "";
        QString lang = "";
        QString speed = "";
        QString emotion = "";
        bool ssml = false;
    };

    struct Task {
        quint64 id;
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

    const QString format();
    void setFormat(const QString newFormat);

    const QString sampleRateHertz();
    void setSampleRateHertz(const QString newSampleRateHertz);

    static Options makeOptions(const QString &voice, const QString &lang, const QString &speed, const QString &emotion, bool ssml = false);
    static QStringList availableVoices(QString locale = "ru_RU");

public slots:
    void makeAudioFile(quint64 id, const QString text, const SpeechkitTTS::Options &options);
    void replyFinished(QNetworkReply *reply);

signals:
    void complete(quint64 id, QString filename);
    void fail(quint64 id, int type, const QString info);

private:
    QNetworkAccessManager *m_pManager;
    QDateTime m_tokenExpiry;
    QString m_tokenIm;
    QString m_tokenOAuth;
    QString m_folderId;
    QString m_format;
    QString m_sampleRateHertz;

    void updateImTokenRequest(quint64 id, const QString text, const SpeechkitTTS::Options &options);
    void updateImTokenRequest(quint64 id, const SpeechkitTTS::Options &options);
    void voiceLoadRequest(quint64 id, const QString text, const SpeechkitTTS::Options &options);
    bool completeUpdateImTokenRequest(QNetworkReply *reply, const Task &task);
    bool completeLoadVoiceRequest(QNetworkReply *reply, const Task &task);

    const QString getFormatOption();
    QString getSampleRateHertzOption();

    inline static bool isValidOption(int type, QString value);
    inline static QString getValidLangByVoice(QString voice);

    void saveLastImToken();
    void loadLastImToken();
};

Q_DECLARE_METATYPE(SpeechkitTTS::Options)
Q_DECLARE_METATYPE(SpeechkitTTS::Task)

#endif // SPEECHKITTTS_H
