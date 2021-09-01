#include "cicero.h"

#include <QDebug>

Cicero::Cicero()
{
    qDebug() << getHardwareID("1.0");
    qDebug() << getHardwareID("1.1");
    qDebug() << getHardwareID("1.2");
    qDebug() << getHardwareID("1.3");
    qDebug() << getHardwareID("1.4");
}

const QString Cicero::getHardwareID(const QString algVer) {
    const system_info::Version version{algVer.toStdString()};
    return QString::fromStdString(system_info::NativeOSManager::GetHardwareProperties(version));
}
