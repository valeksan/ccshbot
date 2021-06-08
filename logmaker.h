#ifndef LOGMAKER_H
#define LOGMAKER_H

#include <QObject>
#include <QtQml>
#include <QTextDocument>
#include <QFile>
#include <QDir>
#include <QUrl>
#include <QDesktopServices>

class LogMaker : public QObject
{
    Q_OBJECT

    QString m_lastLogName;

public:
    explicit LogMaker(QObject *parent = 0);

public slots:
    QString lastLogName();

    bool lastLogNameExists();

    bool makeLogFromHtml(QString text);

    bool appendLastLogFromHtml(QString text);

    bool makeLog(QString text);

    bool appendLastLog(QString text);

    bool openLastLogFile();

    bool openLogDir();

private:
    inline const QString getAppLogDirPath()
    {
        QString path = QStandardPaths::writableLocation(
                    QStandardPaths::AppDataLocation);
        if (path.isEmpty()) {
            qDebug() << "Cannot determine settings storage location";
            path = QDir::homePath() + QDir::separator() + ".ccbot";
        }
        return (path + QDir::separator() + "logs");
    }

    QString genarateNewLogName();

    bool makeLogDir();

    bool logDirExist();

    QByteArray htmlToPlainText(const QString& styled_text);
};

#endif // LOGMAKER_H
