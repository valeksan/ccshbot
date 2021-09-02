#include "cicero.h"

#include <QDebug>

Cicero::Cicero()
{
    //
}

const QString Cicero::getHardwareID(const QString algVer) {
    const system_info::Version version{algVer.toStdString()};
    return QString::fromStdString(system_info::NativeOSManager::GetHardwareProperties(version));
}
