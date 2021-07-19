#ifndef SPEECHKITTTS_H
#define SPEECHKITTTS_H

#include <QObject>
#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

struct SpeakOptions {
    QString voice = "";
    QString lang = "";
    QString speed = "";
    QString emotion = "";
    QString rate = "";
    QString format = "";
    QString folderId = "";
    QString tokenOAuth = "";
};

class SpeechkitTTS : public QObject
{
    Q_OBJECT
public:
    static int timeoutGetIamToken;
    static QString hostGetImToken;

    explicit SpeechkitTTS(QObject *parent = nullptr);

    const QDateTime &tokenExpiry() const;
    void setTokenExpiry(const QDateTime &newTokenExpiry);

    const QString &tokenIm() const;
    void setTokenIm(const QString &newTokenIm);

public slots:
    void voiceText(QString text, const SpeakOptions &options);
    void replyFinished(QNetworkReply *reply);

signals:
    void voiceComplete(QUrl uri);

private:
    QNetworkAccessManager *m_pManager;
    QDateTime m_tokenExpiry;
    QString m_tokenIm;
};

Q_DECLARE_METATYPE(SpeakOptions)

#endif // SPEECHKITTTS_H
