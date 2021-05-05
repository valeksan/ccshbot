#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <QObject>

#include "enums.h"

class Properties : public QObject
{
    Q_OBJECT

public:
    explicit Properties(QObject *parent = nullptr) : QObject(parent)
    {
        //
    }

public slots:

signals:

};

#endif // PROPERTIES_H
