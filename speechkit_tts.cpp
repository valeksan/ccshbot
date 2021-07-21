#include "speechkit_tts.h"

int SpeechkitTTS::timeoutGetIamToken = defaultTimeoutGetIamToken;
int SpeechkitTTS::timeoutGetAudio = defaultTimeoutGetAudio;
QString SpeechkitTTS::hostGetImToken = defaultSpeechkitGetIamTokenHost;
QString SpeechkitTTS::hostSpeechKit = defaultSpeechkitHost;

SpeechkitTTS::SpeechkitTTS(QObject *parent, QString folderId, QString tokenOAuth)
    : QObject(parent), m_tokenOAuth(tokenOAuth), m_folderId(folderId),
      m_format(DefaultFormat), m_sampleRateHertz(DefaultSampleRateHertz)
{
    m_pManager = new QNetworkAccessManager(this);

    connect(m_pManager, &QNetworkAccessManager::finished,
            this, &SpeechkitTTS::replyFinished);
}

void SpeechkitTTS::voiceText(const QString text, const SpeechkitTTS::Options &options)
{
    bool tokenExpiry = (QDateTime::currentDateTime() >= m_tokenExpiry);

    // checking that the token has expired
    if (tokenExpiry) {
        // Update token and Requesting a sound file from the server to text after
        updateImTokenRequest(text, options);
        return;
    } else {
        // Requesting a sound file from the server to text
        voiceLoadRequest(text, options);
    }
}

void SpeechkitTTS::replyFinished(QNetworkReply *reply)
{
    auto err = reply->error();
    if (err) {
        const QString errInfo = QString("Error network reply(%1) - %2")
                .arg(QString::number(err), reply->errorString());
        emit voiceFail(err, errInfo);
        reply->deleteLater();
        return;
    }

    QVariant userAttr = reply->request().attribute(QNetworkRequest::User);

    SpeechkitTTS::Task task = userAttr.value<SpeechkitTTS::Task>();

    switch (int type = task.type; type) {
    case SpeechkitTTS::UpdateToken:
        if (completeUpdateImTokenRequest(reply)) {
            if (!task.text.isEmpty()) {
                voiceText(task.text, task.options);
            }
        }
        break;

    case SpeechkitTTS::VoiceLoad:
        completeLoadVoiceRequest(reply);
        break;

    default:
        break;
    }
}

SpeechkitTTS::SampleRateHertz SpeechkitTTS::sampleRateHertz() const
{
    return static_cast<SpeechkitTTS::SampleRateHertz>(m_sampleRateHertz);
}

const QString SpeechkitTTS::sampleRateHertz()
{
    return getSampleRateHertzOption();
}

void SpeechkitTTS::setSampleRateHertz(SpeechkitTTS::SampleRateHertz newSampleRateHertz)
{
    m_sampleRateHertz = newSampleRateHertz;
}

void SpeechkitTTS::setSampleRateHertz(const QString newSampleRateHertz)
{
    if (newSampleRateHertz == "48000") {
        m_format = SpeechkitTTS::sr48000hz;
    } else if (newSampleRateHertz == "16000") {
        m_format = SpeechkitTTS::sr16000hz;
    } else if (newSampleRateHertz == "8000") {
        m_format = SpeechkitTTS::sr8000hz;
    } else {
        m_format = SpeechkitTTS::DefaultSampleRateHertz;
    }
}

SpeechkitTTS::Format SpeechkitTTS::format() const
{
    return static_cast<SpeechkitTTS::Format>(m_format);
}

const QString SpeechkitTTS::format()
{
    return getFormatOption();
}

void SpeechkitTTS::setFormat(SpeechkitTTS::Format newFormat)
{
    m_format = newFormat;
}

void SpeechkitTTS::setFormat(const QString newFormat)
{
    if (newFormat == "oggopus") {
        m_format = SpeechkitTTS::OggOpus;
    } else if (newFormat == "lpcm") {
        m_format = SpeechkitTTS::LPCM;
    } else {
        m_format = SpeechkitTTS::DefaultFormat;
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

const QString &SpeechkitTTS::folderId() const
{
    return m_folderId;
}

void SpeechkitTTS::setFolderId(const QString &newFolderId)
{
    m_folderId = newFolderId;
}

const QString &SpeechkitTTS::tokenOAuth() const
{
    return m_tokenOAuth;
}

void SpeechkitTTS::setTokenOAuth(const QString &newTokenOAuth)
{
    m_tokenOAuth = newTokenOAuth;
}

void SpeechkitTTS::updateImTokenRequest(const QString text, const SpeechkitTTS::Options &options)
{
    QNetworkRequest requestGetIamToken;
    requestGetIamToken.setUrl(QUrl(hostGetImToken));
    requestGetIamToken.setHeader(
                QNetworkRequest::ContentTypeHeader,
                "application/json");
    QJsonObject obj;
    obj["yandexPassportOauthToken"] = m_tokenOAuth;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    // * todo request
    m_pManager->setTransferTimeout(timeoutGetIamToken);

    SpeechkitTTS::Task task = {UpdateToken, text, options};
    requestGetIamToken.setAttribute(QNetworkRequest::User, QVariant::fromValue(task));

    m_pManager->post(requestGetIamToken, data);
}

void SpeechkitTTS::updateImTokenRequest(const SpeechkitTTS::Options &options)
{
    updateImTokenRequest("", options);
}

void SpeechkitTTS::voiceLoadRequest(const QString text, const Options &options)
{
    QNetworkRequest requestGetAudio;
    QUrl url(hostSpeechKit);
    QUrlQuery postDataEncoded;
    // * add header
    requestGetAudio.setHeader(QNetworkRequest::ContentTypeHeader,
                              "application/x-www-form-urlencoded");
    requestGetAudio.setRawHeader("Authorization",
                                 QString("Bearer %1")
                                 .arg(m_tokenIm)
                                 .toUtf8());
    // * add data
    QString safeTextValue = text;
    safeTextValue.replace(';', "%3B");
    safeTextValue.replace(' ', '+');
    postDataEncoded.addQueryItem("text", safeTextValue);
    postDataEncoded.addQueryItem("folderId", m_folderId);

    if (isValidOption(SpeechkitTTS::Lang, options.lang)) {
        postDataEncoded.addQueryItem("lang", options.lang);
    }
    if (isValidOption(SpeechkitTTS::Voice, options.voice)) {
        postDataEncoded.addQueryItem("voice", options.voice);
    }
    if (isValidOption(SpeechkitTTS::Emotion, options.emotion)) {
        postDataEncoded.addQueryItem("emotion", options.emotion);
    }
    if (isValidOption(SpeechkitTTS::Speed, options.speed)) {
        float fspeed = options.speed.toFloat();
        postDataEncoded.addQueryItem("speed", QString::number(fspeed, 'f', 1));
    }
    if (QString formatOption = getFormatOption();
            !formatOption.isEmpty())
    {
        postDataEncoded.addQueryItem("format", formatOption);
    }
    if (QString sampleRateHertzOption = getSampleRateHertzOption();
            !sampleRateHertzOption.isEmpty())
    {
        postDataEncoded.addQueryItem("sampleRateHertz", sampleRateHertzOption);
    }
    SpeechkitTTS::Task task = {VoiceLoad, text, options};
    requestGetAudio.setAttribute(QNetworkRequest::User, QVariant::fromValue(task));
    // * request
    m_pManager->setTransferTimeout(timeoutGetAudio);
    requestGetAudio.setUrl(url);
    m_pManager->post(requestGetAudio,
                     postDataEncoded
                     .toString(QUrl::FullyEncoded)
                     .toUtf8());
}

bool SpeechkitTTS::completeUpdateImTokenRequest(QNetworkReply *reply)
{
    QByteArray response = reply->readAll();
    QJsonDocument jsonResp =  QJsonDocument::fromJson(response);

    if (jsonResp.isNull()) {
        QString info = "Unknown error, the answer is not JSON type!";
        emit voiceFail(SpeechkitTTS::ServiceError, info);
        reply->deleteLater();
        return false;
    }

    QJsonObject responseObj = jsonResp.object();

    if (responseObj.contains("error_code")) {
        int code = responseObj.value("error_code").toInt();
        if (code > 200) {
            QString info = "";
            switch (code) {
            case 400:
                info.append("Syntax error in the request.");
                break;
            case 401:
                info.append("Authorisation Error.");
                break;
            case 429:
                info.append("Request limit exceeded.");
                break;
            case 500:
                info.append("Server side error.");
                break;
            default:
                QString extInfo = responseObj.value("error_message").toString();
                if (!extInfo.isEmpty()) {
                    info.append(extInfo);
                } else {
                    info.append("Unknown error.");
                }
                break;
            }
            emit voiceFail(SpeechkitTTS::ServiceError, info);
            reply->deleteLater();
            return false;
        }
    }

    m_tokenIm = responseObj.value("iamToken").toString();
    m_tokenExpiry = QDateTime::fromString(
                responseObj.value("expiresAt").toString(),
                Qt::ISODateWithMs);

    reply->deleteLater();
    return true;
}

bool SpeechkitTTS::completeLoadVoiceRequest(QNetworkReply *reply)
{
    QByteArray response = reply->readAll();
    QJsonDocument jsonResp =  QJsonDocument::fromJson(response);

    if (jsonResp.isObject()) {
        QJsonObject responseObj = jsonResp.object();
        int code = responseObj.value("error_code").toInt();
        QString info = "";
        switch (code) {
        case 400:
            info.append("Syntax error in the request.");
            break;
        case 401:
            info.append("Authorisation Error.");
            break;
        case 429:
            info.append("Request limit exceeded.");
            break;
        case 500:
            info.append("Server side error.");
            break;
        default:
            QString extInfo = responseObj.value("error_message").toString();
            if (!extInfo.isEmpty()) {
                info.append(extInfo);
            } else {
                info.append("Unknown error.");
            }
            break;
        }
        emit voiceFail(SpeechkitTTS::ServiceError, info);
        reply->deleteLater();
        return false;
    }

    QTemporaryFile tmpfile;
    if (tmpfile.open()) {
        tmpfile.setAutoRemove(false);
        QDataStream ostream(&tmpfile);
        ostream.writeRawData(response, response.size());
        tmpfile.close();
    } else {
        QString info = "File write error, possibly there is no write access to the temporary directory!";
        emit voiceFail(SpeechkitTTS::SystemError, info);
        reply->deleteLater();
        return false;
    }

    emit voiceComplete(tmpfile.fileName());

    reply->deleteLater();
    return true;
}

const QString SpeechkitTTS::getFormatOption()
{
    switch (m_format) {
    case DefaultFormat:
        return "";
    case OggOpus:
        return "oggopus";
    case LPCM:
        return "lpcm";
    }
    return "";
}

QString SpeechkitTTS::getSampleRateHertzOption()
{
    switch (m_sampleRateHertz) {
    case DefaultSampleRateHertz:
        return "";
    case sr48000hz:
        return "48000";
    case sr16000hz:
        return "16000";
    case sr8000hz:
        return "8000";
    }
    return "";
}

bool SpeechkitTTS::isValidOption(int type, QString value)
{
    switch (type) {
    case SpeechkitTTS::Lang:
        if (value == "ru-RU"
                || value == "en-US"
                || value == "tr-TR")
        {
            return true;
        }
        return false;
    case SpeechkitTTS::Voice:
        if (value == "oksana"
                || value == "filipp"
                || value == "alena"
                || value == "jane"
                || value == "omazh"
                || value == "zahar"
                || value == "ermil"
                || value == "silaerkan"
                || value == "erkanyavas"
                || value == "alyss"
                || value == "nick")
        {
            return true;
        }
        return false;
    case SpeechkitTTS::Emotion:
        if (value == "good"
                || value == "evil"
                || value == "neutral")
        {
            return true;
        }
        return false;
    case SpeechkitTTS::Speed:
        if (auto fvalue = value.toDouble();
                fvalue >= 0.1 && fvalue <= 3.0)
        {
            return true;
        }
        return false;
    }
    return false;
}

QString SpeechkitTTS::getValidLangByVoice(QString voice)
{
    if (voice == "oksana")
        return "ru-RU";
    if (voice == "filipp")
        return "ru-RU";
    if (voice == "alena")
        return "ru-RU";
    if (voice == "jane")
        return "ru-RU";
    if (voice == "omazh")
        return "ru-RU";
    if (voice == "zahar")
        return "ru-RU";
    if (voice == "ermil")
        return "ru-RU";
    if (voice == "silaerkan")
        return "tr-TR";
    if (voice == "erkanyavas")
        return "tr-TR";
    if (voice == "alyss")
        return "en-US";
    if (voice == "nick")
        return "en-US";
    return "";
}

const QDateTime &SpeechkitTTS::tokenExpiry() const
{
    return m_tokenExpiry;
}

void SpeechkitTTS::setTokenExpiry(const QDateTime &newTokenExpiry)
{
    m_tokenExpiry = newTokenExpiry;
}
