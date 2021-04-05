#ifndef DESKTOP_H
#define DESKTOP_H

/*
 * Класс для считывания геометрии рабочего стола операционной системы, пробрасываемый как единый экземпляр класса в QML (синглтон)
*/

#include <QGuiApplication>
#include <QApplication>
#include <QQmlEngine>
#include <QRect>
#include <QScreen>

class Desktop : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Desktop)

public:
    Desktop(){}

    // получить доступную для позиционирования геометрию экрана (с вычетом закрепленных панелей на экране)
    Q_INVOKABLE QRect availableGeometry(QString name = QGuiApplication::applicationDisplayName()) {
        if(!name.isEmpty()) {
            QList<QScreen*> screenlist = QGuiApplication::screens();
            for(QScreen* scr : screenlist) {
                if(scr->name() == name) {
                    return scr->availableGeometry();
                }
            }
        }
        return QGuiApplication::primaryScreen()->availableGeometry();
    }

    // получить общую геометрию экрана
    Q_INVOKABLE QRect screenGeometry(QString name = QGuiApplication::applicationDisplayName()) {
        if(!name.isEmpty()) {
            QList<QScreen*> screenlist = QGuiApplication::screens();
            for(QScreen* scr : screenlist) {
                if(scr->name() == name) {
                    return scr->geometry();
                }
            }
        }
        return QGuiApplication::primaryScreen()->geometry();
    }

    static QObject *desktopSingletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
    {
        // для того чтобы не создавать экземпляры классов в QML, а просто вызывать методы этих классов создается метод-провайдер
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        //Desktop *singletonDesktopClass = new Desktop();
        //return singletonDesktopClass;
        return new Desktop;
    }
};

#endif // DESKTOP_H
