#ifndef MESSAGEDATA_H
#define MESSAGEDATA_H

#include <QObject>
#include <QString>

struct MessageData {

    quint64 id;
    qint32 type;
    QString sender;
    QString nik_color = "";
    QString msg;
    float pay;
    quint32 timestamp;
    bool flagIsNewMsg = false;

};

Q_DECLARE_METATYPE(MessageData);

#endif // MESSAGEDATA_H
