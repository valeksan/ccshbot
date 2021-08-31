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

//using namespace system_info;

class Cicero
{
 public:
    Cicero();
    const QString getHardwareID(const QString algVer = "1.4");
};
