#include "speechkit_tts.h"

int SpeechkitTTS::timeoutGetIamToken = defaultTimeoutGetIamToken;
int SpeechkitTTS::timeoutGetAudio = defaultTimeoutGetAudio;
QString SpeechkitTTS::hostGetImToken = defaultSpeechkitGetIamTokenHost;
QString SpeechkitTTS::hostSpeechKit = defaultSpeechkitHost;

SpeechkitTTS::SpeechkitTTS(QObject *parent)
    : QObject(parent),
      m_format(""), m_sampleRateHertz("")
{
    m_pManager = new QNetworkAccessManager(this);

    loadLastImToken();

    connect(m_pManager, &QNetworkAccessManager::finished,
            this, &SpeechkitTTS::replyFinished);
}

void SpeechkitTTS::makeAudioFile(quint64 id, const QString text, const SpeechkitTTS::Options &options)
{
    bool tokenExpiry = (QDateTime::currentDateTime() >= m_tokenExpiry);

    // checking that the token has expired
    if (tokenExpiry) {
        // Update token and Requesting a sound file from the server to text after
        updateImTokenRequest(id, text, options);
        return;
    } else {
        // Requesting a sound file from the server to text
        voiceLoadRequest(id, text, options);
    }
}

void SpeechkitTTS::replyFinished(QNetworkReply *reply)
{
    QVariant userAttr = reply->request().attribute(QNetworkRequest::User);
    SpeechkitTTS::Task task = userAttr.value<SpeechkitTTS::Task>();

    auto err = reply->error();
    if (err) {
        const QString errInfo = QString("Error network reply(%1) - %2")
                .arg(QString::number(err), reply->errorString());
        emit fail(task.id, err, errInfo);
        reply->deleteLater();
        return;
    }

    switch (int type = task.type; type) {
    case SpeechkitTTS::UpdateToken:
        if (completeUpdateImTokenRequest(reply, task)) {
            saveLastImToken();
            if (!task.text.isEmpty()) {
                makeAudioFile(task.id, task.text, task.options);
            }
        }
        break;

    case SpeechkitTTS::VoiceLoad:
        completeLoadVoiceRequest(reply, task);
        break;

    default:
        break;
    }
}

const QString SpeechkitTTS::sampleRateHertz()
{
    return m_sampleRateHertz;
}

void SpeechkitTTS::setSampleRateHertz(const QString newSampleRateHertz)
{
    if (isValidOption(SampleRateHertz, newSampleRateHertz)) {
        m_sampleRateHertz = newSampleRateHertz;
    }
}

SpeechkitTTS::Options SpeechkitTTS::makeOptions(const QString &voice, const QString &lang, const QString &speed, const QString &emotion, bool ssml)
{
    Options options = {voice, lang, speed, emotion, ssml};
    return options;
}

const QString SpeechkitTTS::format()
{
    return m_format;
}

void SpeechkitTTS::setFormat(const QString newFormat)
{
    if (isValidOption(Format, newFormat)) {
        m_format = newFormat;
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

void SpeechkitTTS::updateImTokenRequest(quint64 id, const QString text, const SpeechkitTTS::Options &options)
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

    SpeechkitTTS::Task task = {id, UpdateToken, text, options};
    requestGetIamToken.setAttribute(QNetworkRequest::User, QVariant::fromValue(task));

    m_pManager->post(requestGetIamToken, data);
}

void SpeechkitTTS::updateImTokenRequest(quint64 id, const SpeechkitTTS::Options &options)
{
    updateImTokenRequest(id, "", options);
}

void SpeechkitTTS::voiceLoadRequest(quint64 id, const QString text, const Options &options)
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
    postDataEncoded.addQueryItem(!options.ssml ? "text" : "ssml", safeTextValue);
    postDataEncoded.addQueryItem("folderId", m_folderId);

    if (isValidOption(SpeechkitTTS::Lang, options.lang)) {
        postDataEncoded.addQueryItem("lang", options.lang);
    }
    if (isValidOption(SpeechkitTTS::Voice, options.voice)) {
        postDataEncoded.addQueryItem("voice", options.voice.section(":", -1, -1));
    }
    if (isValidOption(SpeechkitTTS::Emotion, options.emotion)) {
        postDataEncoded.addQueryItem("emotion", options.emotion);
    }
    if (isValidOption(SpeechkitTTS::Speed, options.speed)) {
        float fspeed = options.speed.toFloat();
        postDataEncoded.addQueryItem("speed", QString::number(fspeed, 'f', 1));
    }
    if (!m_format.isEmpty()) {
        postDataEncoded.addQueryItem("format", m_format);
    }
    if (!m_sampleRateHertz.isEmpty()) {
        postDataEncoded.addQueryItem("sampleRateHertz", m_sampleRateHertz);
    }
    SpeechkitTTS::Task task = {id, VoiceLoad, text, options};
    requestGetAudio.setAttribute(QNetworkRequest::User, QVariant::fromValue(task));

    // * request
    m_pManager->setTransferTimeout(timeoutGetAudio);
    requestGetAudio.setUrl(url);
    m_pManager->post(requestGetAudio, postDataEncoded.toString(QUrl::FullyEncoded).toUtf8());
}

bool SpeechkitTTS::completeUpdateImTokenRequest(QNetworkReply *reply, const SpeechkitTTS::Task &task)
{
    QByteArray response = reply->readAll();
    QJsonDocument jsonResp =  QJsonDocument::fromJson(response);

    if (jsonResp.isNull()) {
        QString info = "Unknown error, the answer is not JSON type!";
        emit fail(task.id, SpeechkitTTS::ServiceError, info);
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
            emit fail(task.id, SpeechkitTTS::ServiceError, info);
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

bool SpeechkitTTS::completeLoadVoiceRequest(QNetworkReply *reply, const Task &task)
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
        emit fail(task.id, SpeechkitTTS::ServiceError, info);
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
        emit fail(task.id, SpeechkitTTS::SystemError, info);
        reply->deleteLater();
        return false;
    }

    emit complete(task.id, tmpfile.fileName());

    reply->deleteLater();
    return true;
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
        if (value == "ya:oksana"
                || value == "ya:filipp"
                || value == "ya:alena"
                || value == "ya:jane"
                || value == "ya:omazh"
                || value == "ya:zahar"
                || value == "ya:ermil"
                || value == "ya:silaerkan"
                || value == "ya:erkanyavas"
                || value == "ya:alyss"
                || value == "ya:nick")
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
    case SpeechkitTTS::Format:
        if (value == "oggopus" || value == "lpcm") {
            return true;
        }
        return false;
    case SpeechkitTTS::SampleRateHertz:
        if (value == "48000" || value == "16000" || value == "8000") {
            return true;
        }
        return false;
    }
    return false;
}

QString SpeechkitTTS::getValidLangByVoice(QString voice)
{
    if (voice == "ya:oksana")
        return "ru-RU";
    if (voice == "ya:filipp")
        return "ru-RU";
    if (voice == "ya:alena")
        return "ru-RU";
    if (voice == "ya:jane")
        return "ru-RU";
    if (voice == "ya:omazh")
        return "ru-RU";
    if (voice == "ya:zahar")
        return "ru-RU";
    if (voice == "ya:ermil")
        return "ru-RU";
    if (voice == "ya:silaerkan")
        return "tr-TR";
    if (voice == "ya:erkanyavas")
        return "tr-TR";
    if (voice == "ya:alyss")
        return "en-US";
    if (voice == "ya:nick")
        return "en-US";
    return "";
}

QStringList SpeechkitTTS::availableVoices(QString locale)
{
    QStringList voices;
    if (locale == "ru_RU" || locale.isEmpty()) {
        voices << "ya:oksana" << "ya:jane" << "ya:omazh" << "ya:zahar" << "ya:ermil" << "ya:alena" << "ya:filipp";
    }
    if (locale == "en_US" || locale.isEmpty()) {
        voices << "ya:alyss" << "ya:nick";
    }
    if (locale == "tr_TR" || locale.isEmpty()) {
        voices << "ya:silaerkan" << "ya:erkanyavas";
    }
    return voices;
}

void SpeechkitTTS::saveLastImToken()
{
    QSettings cfg;
    cfg.beginGroup("Speechkit");
    cfg.setValue("ImToken", m_tokenIm);
    cfg.setValue("ImTokenExpiry", m_tokenExpiry);
    cfg.endGroup();
}

void SpeechkitTTS::loadLastImToken()
{
    QSettings cfg;
    cfg.beginGroup("Speechkit");
    m_tokenIm = cfg.value("ImToken", "").toString();
    m_tokenExpiry = cfg.value("ImTokenExpiry", QDateTime::currentDateTime()).toDateTime();
    cfg.endGroup();
}

const QDateTime &SpeechkitTTS::tokenExpiry() const
{
    return m_tokenExpiry;
}

void SpeechkitTTS::setTokenExpiry(const QDateTime &newTokenExpiry)
{
    m_tokenExpiry = newTokenExpiry;
}
