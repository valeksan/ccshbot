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

#define KEYGEN_PUB_KEY "490c54e81f7c73c2802fb48ad3e16b3b4a7bb6333730a00f93bb1ad842b5ffb7"
#define PROGRA_PRI_KEY "0b5044f49020d691f47e23907ba42139869a7da03943ad5915a2e1ce533d0e0b"

class Cicero
{
 public:
    Cicero();

 private:
    const QByteArray convToKeyFormat(const QByteArray &binKey);
    const QByteArray clearKeyFormat(const QByteArray &fmtKey);
    const QByteArray getHID(const QString algVer = "1.4");
    const QByteArray makeRegistrationKey(const QString algVer = "1.4");
    const QByteArray makeActivationKey(QByteArray registrationKey, QByteArray keygenPriKey, QByteArray prograPubKey);
    const QByteArray getHIDFromKey(QByteArray fmtKey, QByteArray priKey);
    bool verifyActivation(const QByteArray &activationKey);

};
