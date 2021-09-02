#pragma once

#include <QString>
#include <iostream>
#include <string>

#include "cicero_imp/log.hpp"
#include "cicero_imp/md5.hpp"
#include "cicero_imp/util.hpp"
#include "cicero_imp/version.hpp"

#if defined( SYSTEM_WINDOWS )
#include "cicero_imp/windows/windows_manager.hpp"
#elif defined( SYSTEM_LINUX )
#include "cicero_imp/linux/linux_manager.hpp"
#elif defined( SYSTEM_MAC )
#include "cicero_imp/mac/mac_manager.hpp"
#endif

#include <qrsaencryption.h>

class Cicero
{
 public:
    Cicero();
    const QString getHardwareID(const QString algVer = "1.4");

 private:
    const QByteArray keygPubKey = "2ca16caa7b5c04e0fafc46573e98a7634a7bb6333730a00f93bb1ad842b5ffb7";
};
