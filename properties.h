#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <QObject>

class Properties : public QObject
{
    Q_OBJECT

public:
    explicit Properties(QObject *parent = nullptr) : QObject(parent)
    {
        //
    }

signals:

};

#endif // PROPERTIES_H
