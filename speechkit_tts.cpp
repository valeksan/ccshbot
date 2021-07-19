#include "speechkit_tts.h"

int SpeechkitTTS::timeoutGetIamToken = 5000;
QString SpeechkitTTS::hostGetImToken = QString("https://iam.api.cloud.yandex.net/iam/v1/tokens");

SpeechkitTTS::SpeechkitTTS(QObject *parent) : QObject(parent)
{
    m_pManager = new QNetworkAccessManager(this);

    connect(m_pManager, &QNetworkAccessManager::finished,
            this, &SpeechkitTTS::replyFinished);
}

void SpeechkitTTS::voiceText(QString text, const SpeakOptions &options)
{
    bool tokenExpiry = (QDateTime::currentDateTime() >= m_tokenExpiry);

    // 1. checking that the token has expired
    if (tokenExpiry) {
        // 1.1 get a new token and update token
        QNetworkRequest requestGetIamToken;
        requestGetIamToken.setUrl(QUrl(hostGetImToken));
        requestGetIamToken.setHeader(
                    QNetworkRequest::ContentTypeHeader,
                    "application/json");
        QJsonObject obj;
        obj["yandexPassportOauthToken"] = options.tokenOAuth;
        QJsonDocument doc(obj);
        QByteArray data = doc.toJson(QJsonDocument::Compact);
        // * todo request
        m_pManager->setTransferTimeout(timeoutGetIamToken);
        requestGetIamToken.setAttribute(QNetworkRequest::User, 0);
        m_pManager->post(requestGetIamToken, data);
        return;
    }
    // 2. Requesting a sound file from the server to text

}

void SpeechkitTTS::replyFinished(QNetworkReply *reply)
{
    bool isImToken = (reply->attribute(QNetworkRequest::User).toInt() == 0);
    if (isImToken) {
        QByteArray response = reply->readAll();
        QJsonObject responseObj =
                QJsonDocument::fromJson(response).object();
        m_tokenIm = responseObj.value("iamToken").toString();
        m_tokenExpiry = QDateTime::fromString(
                    responseObj.value("expiresAt").toString(),
                    Qt::ISODateWithMs);
    }
}

const QString &SpeechkitTTS::tokenIm() const
{
    return m_tokenIm;
}

void SpeechkitTTS::setTokenIm(const QString &newTokenIm)
{
    m_tokenIm = newTokenIm;
}

const QDateTime &SpeechkitTTS::tokenExpiry() const
{
    return m_tokenExpiry;
}

void SpeechkitTTS::setTokenExpiry(const QDateTime &newTokenExpiry)
{
    m_tokenExpiry = newTokenExpiry;
}
