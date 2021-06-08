#include "logmaker.h"

LogMaker::LogMaker(QObject *parent) : QObject(parent),
    m_lastLogName("")
{}

QString LogMaker::lastLogName()
{
    return m_lastLogName;
}

void LogMaker::endLogSession()
{
    m_lastLogName.clear();
}

bool LogMaker::lastLogNameExists()
{
    if (m_lastLogName.isEmpty()) {
        return false;
    }

    QDir d{getAppLogDirPath()};

    if (!d.exists()) {
        if (!d.mkpath(d.absolutePath())) {
            qDebug() << QString("Can't to create path: %1").arg(d.absolutePath());
            return false;
        }
    }

    QString logFullName = d.absolutePath() + QDir::separator() + m_lastLogName;

    return QFile::exists(logFullName);
}

bool LogMaker::makeLogFromHtml(QString text)
{
    if (!logDirExist())
        if (!makeLogDir())
            return false;

    QDir d{getAppLogDirPath()};

    if (!d.exists()) {
        qDebug() << QString("this path not exist: %1").arg(d.absolutePath());
        return false;
    }

    QString logName = genarateNewLogName();
    QString logFullName = d.absolutePath() + QDir::separator() + logName;

    QFile log(logFullName);
    if (!log.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "fail make log!";
        return false;
    }
    m_lastLogName = logName;
    log.write(htmlToPlainText(text));
    log.close();
    return true;
}

bool LogMaker::appendLastLogFromHtml(QString text)
{
    if (m_lastLogName.isEmpty()) {
        return false;
    }

    QDir d{getAppLogDirPath()};

    if (!d.exists()) {
        qDebug() << QString("this path not exist: %1").arg(d.absolutePath());
        return false;
    }

    QString logFullName = d.absolutePath() + QDir::separator() + m_lastLogName;

    QFile log(logFullName);
    if (!log.open(QIODevice::WriteOnly |
                  QIODevice::Append |
                  QIODevice::Text))
    {
        return false;
    }
    QByteArray simpleText = htmlToPlainText(text);
    log.write(simpleText);
    log.close();
    return true;
}

bool LogMaker::makeLog(QString text)
{
    if (!logDirExist())
        if (!makeLogDir())
            return false;

    QDir d{getAppLogDirPath()};

    if (!d.exists()) {
        qDebug() << QString("this path not exist: %1").arg(d.absolutePath());
        return false;
    }

    QString logName = genarateNewLogName();
    QString logFullName = d.absolutePath() + QDir::separator() + logName;

    QFile log(logFullName);
    if (!log.open(QIODevice::Append | QIODevice::Text)) {
        return false;
    }
    m_lastLogName = logName;
    log.write(text.toUtf8());
    log.close();

    return true;
}

bool LogMaker::appendLastLog(QString text)
{
    if (m_lastLogName.isEmpty()) {
        return false;
    }

    QDir d{getAppLogDirPath()};

    if (!d.exists()) {
        qDebug() << QString("this path not exist: %1").arg(d.absolutePath());
        return false;
    }

    QString logFullName = d.absolutePath() + QDir::separator() + m_lastLogName;

    QFile log(logFullName);
    if (!log.open(QIODevice::WriteOnly |
                  QIODevice::Append |
                  QIODevice::Text))
    {
        return false;
    }
    log.write(text.toUtf8());
    log.close();
    return true;
}

bool LogMaker::openLastLogFile()
{
    if (!lastLogNameExists()) {
        return false;
    }

    QDir d{getAppLogDirPath()};

    if (!d.exists()) {
        qDebug() << QString("this path not exist: %1").arg(d.absolutePath());
        return false;
    }

    QString logFullName = d.absolutePath() +
            QDir::separator() + m_lastLogName;

    QDesktopServices::openUrl(QUrl::fromLocalFile(logFullName));
    return true;
}

bool LogMaker::openLogDir()
{
    if (!logDirExist())
        if (!makeLogDir())
            return false;

    QDir d{getAppLogDirPath()};

    if (!d.exists()) {
        qDebug() << QString("this path not exist: %1").arg(d.absolutePath());
        return false;
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(d.absolutePath()));
    return true;
}

QString LogMaker::genarateNewLogName()
{
    QDateTime UTC(QDateTime::currentDateTimeUtc());
    QDateTime local(UTC.toLocalTime());
    QString logName = QString("%1.log").arg(local.toString("dd-MM-yyyy-hh-mm-ss"));
    return logName;
}

bool LogMaker::makeLogDir()
{
    QDir d{getAppLogDirPath()};

    return d.mkdir("logs");
}

bool LogMaker::logDirExist()
{
    QDir d{getAppLogDirPath()};

    return d.exists("logs");
}

QByteArray LogMaker::htmlToPlainText(const QString &styled_text)
{
    QTextDocument doc;
    doc.setHtml(styled_text);
    QString text = doc.toPlainText();
    text.append("\n");
    return text.toUtf8();
}
