#ifndef MISC_H
#define MISC_H

#include <QEventLoop>
#include <QTimer>
#include <QFile>
#include <QFileInfo>

// Formatting for Rich Text
inline const QString _clr_(const QString &str, const QString &color) {
    return QString("<span style=\"color:") + color + "\">" + str + "</span>";
}
inline const QString _bclr_(const QString &str, const QString &color) {
    return QString("<b style=\"color:") + color + "\">" + str + "</b>";
}

template<typename T>
QList<T> listRight(const QList<T> &list, int n) {
    int startIndex = list.size() - n;
    return list.mid(startIndex < 0 ? 0 : list.size() - n);
}

template<typename Func>
bool waitSignal(const typename QtPrivate::FunctionPointer<Func>::Object *sender, Func signal, int timeout = 30000)
{
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(sender, signal, &loop, &QEventLoop::quit);
    if(timeout > 0) {
        QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        timer.start(timeout);
    }
    loop.exec();
    if(timer.isActive()) return true;
    else return false;
}

template<typename Signal, typename Func>
bool waitSignalAfterFunction(const typename QtPrivate::FunctionPointer<Signal>::Object *sender, Signal signal, Func function, int timeout = 30000)
{
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(sender, signal, &loop, &QEventLoop::quit);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    function();
    timer.start(timeout);
    loop.exec();
    if(timer.isActive()) return true;
    else return false;
}

template <typename T>
QByteArray toByteArray(T obj)
{
    QByteArray ba;
    ba.resize(sizeof(T));
    memcpy(ba.data(), &obj, ba.size());
    return ba;
}

template <typename T>
T fromByteArray(QByteArray ba, bool *ok = nullptr)
{
    T obj;
    if(static_cast<std::size_t>(ba.size()) < sizeof(T) && ok) *ok = false;
    memcpy(&obj, ba.constData(), std::min(static_cast<size_t>(ba.size()), sizeof(T)));
    if(ok) *ok = true;
    return obj;
}

inline QString addUniqueSuffix(const QString &fileName)
{
    // If the file doesn't exist return the same name.
    if (!QFile::exists(fileName)) {
        return fileName;
    }

    QFileInfo fileInfo(fileName);
    QString ret;

    // Split the file into 2 parts - dot+extension, and everything else. For
    // example, "path/file.tar.gz" becomes "path/file"+".tar.gz", while
    // "path/file" (note lack of extension) becomes "path/file"+"".
    QString secondPart = fileInfo.completeSuffix();
    QString firstPart;
    if (!secondPart.isEmpty()) {
        secondPart = "." + secondPart;
        firstPart = fileName.left(fileName.size() - secondPart.size());
    } else {
        firstPart = fileName;
    }

    // Try with an ever-increasing number suffix, until we've reached a file
    // that does not yet exist.
    for (int ii = 1; ; ii++) {
        // Construct the new file name by adding the unique number between the
        // first and second part.
        ret = QString("%1 (%2)%3").arg(firstPart).arg(ii).arg(secondPart);
        // If no file exists with the new name, return it.
        if (!QFile::exists(ret)) {
            return ret;
        }
    }
}


#endif // MISC_H
