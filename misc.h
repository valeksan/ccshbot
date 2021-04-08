#ifndef MISC_H
#define MISC_H

#include <QEventLoop>
#include <QTimer>

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


#endif // MISC_H
