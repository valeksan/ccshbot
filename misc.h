#ifndef MISC_H
#define MISC_H

#include <QEventLoop>
#include <QTimer>

// Форматирование для текста в фрмате RichText
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


#endif // MISC_H
