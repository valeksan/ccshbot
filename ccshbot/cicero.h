#pragma once

#include <QString>
#include <iostream>
#include <string>

#include <libhid.h>
#include <qrsaencryption.h>

#define KEYGEN_PUB_KEY "490c54e81f7c73c2802fb48ad3e16b3b4a7bb6333730a00f93bb1ad842b5ffb7"
#define PROGRA_PRI_KEY "0b5044f49020d691f47e23907ba42139869a7da03943ad5915a2e1ce533d0e0b"

class Cicero
{
 public:
    Cicero();

    static const QByteArray convToKeyFormat(const QByteArray &binKey);
    static const QByteArray clearKeyFormat(const QByteArray &fmtKey);
    static const QByteArray getHID();
    static const QByteArray makeRegistrationKey();
    static const QByteArray makeActivationKey(QByteArray registrationKey, QByteArray keygenPriKey, QByteArray prograPubKey);
    static const QByteArray getHIDFromKey(QByteArray fmtKey, QByteArray priKey);
    static bool verifyActivation(const QByteArray &activationKey);
};
