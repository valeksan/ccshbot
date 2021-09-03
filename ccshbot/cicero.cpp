#include "cicero.h"

#include <QDebug>

Cicero::Cicero()
{
    auto hid = getHID("1.4");
    auto rkey = makeRegistrationKey("1.4");
    auto akey = makeActivationKey(rkey, QByteArray::fromHex("2ca16caa7b5c04e0fafc46573e98a7634a7bb6333730a00f93bb1ad842b5ffb7"), QByteArray::fromHex("4f549a2085f54ae8c042a0952f0b1db1869a7da03943ad5915a2e1ce533d0e0b"));
    qDebug() << "HID:" << hid;
    qDebug() << "REG_KEY:" << rkey;
    //qDebug() << "CHK_REG_KEY:" << getHIDFromKey(rkey, QByteArray::fromHex("2ca16caa7b5c04e0fafc46573e98a7634a7bb6333730a00f93bb1ad842b5ffb7"));
    qDebug() << "ACT_KEY:" << akey;
    qDebug() << "VERIFY:" << verifyActivation(akey);
    //qDebug() << QByteArray::fromHex(KEYGEN_PUB_KEY).toHex(' ');
}

const QByteArray Cicero::convToKeyFormat(const QByteArray &binKey)
{
    auto rkeyb64 = binKey.toBase64();
    for (int i = 8; i < rkeyb64.size(); i += 8) {
        rkeyb64.insert(i, '-');
    }
    return rkeyb64;
}

const QByteArray Cicero::clearKeyFormat(const QByteArray &fmtKey)
{
    return QByteArray::fromBase64(QString::fromLatin1(fmtKey).remove('-').toLatin1());
}

const QByteArray Cicero::getHID(const QString algVer)
{
    const system_info::Version version{algVer.toStdString()};
    return QByteArray::fromStdString(system_info::NativeOSManager::GetHardwareProperties(version));
}

const QByteArray Cicero::makeRegistrationKey(const QString algVer)
{
    QRSAEncryption e;
    QByteArray rkey = e.encode(getHID(algVer), QByteArray::fromHex(KEYGEN_PUB_KEY), QRSAEncryption::Rsa::RSA_128);
    //qDebug() << "make_rkey:" << rkey.toHex(' ');
    return convToKeyFormat(rkey);
}

const QByteArray Cicero::getHIDFromKey(QByteArray fmtKey, QByteArray priKey)
{
    QRSAEncryption e;
    // normalize reg key
    QByteArray binKey = clearKeyFormat(fmtKey);
    //qDebug() << "getFromFmtKey_BinKey:" << binKey.toHex(' ');
    // decode hardware id data
    QByteArray hid = e.decode(binKey, priKey, QRSAEncryption::Rsa::RSA_128);
    //qDebug() << "getFromRegKey_HID:" << hid;
    return hid;
}

bool Cicero::verifyActivation(const QByteArray &activationKey)
{
    return (getHID("1.4") == getHIDFromKey(activationKey, QByteArray::fromHex(PROGRA_PRI_KEY)));
}

const QByteArray Cicero::makeActivationKey(QByteArray registrationKey, QByteArray keygenPriKey, QByteArray prograPubKey)
{
    QRSAEncryption e;
    QByteArray hid = getHIDFromKey(registrationKey, keygenPriKey);
    //qDebug() << "make_akey_hid:" << hid;
    QByteArray activationKey = e.encode(hid, prograPubKey, QRSAEncryption::Rsa::RSA_128);
    //qDebug() << "make_akey:" << activationKey.toHex(' ');
    return convToKeyFormat(activationKey);
}
