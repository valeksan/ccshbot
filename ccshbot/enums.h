#ifndef ENUMS_H
#define ENUMS_H

#include <QObject>
#include <QQmlEngine>

#ifndef ENABLE_USE_ENUM_NAMESPACES
class CCBotTaskEnums : public QObject
{
    Q_OBJECT
    Q_ENUMS(CCBotTaskEnum)

 public:
    explicit CCBotTaskEnums(QObject *parent = nullptr): QObject(parent) {}
#else
namespace CCBotTaskEnums {
    Q_NAMESPACE
#endif
    enum CCBotTaskEnum {
        MergeChat = 1,
        SpeechkitVoice,
        VoiceSpeech,
        OpenBase,
        CloseBase,
        SysCmdSetVoice,
        SysCmdDrink,
        SysCmdBalance,
    };
#ifndef ENABLE_USE_ENUM_NAMESPACES
};
#else
    Q_ENUMS(CCBotTaskEnum)
}
#endif
// ----------------------------------------------------------------------------

#ifndef ENABLE_USE_ENUM_NAMESPACES
class SaveSectionEnums : public QObject
{
    Q_OBJECT
    Q_ENUMS(SaveSectionEnum)

 public:
    explicit SaveSectionEnums(QObject *parent = nullptr): QObject(parent) {}
#else
namespace SaveSectionEnums {
    Q_NAMESPACE
#endif
    enum SaveSectionEnum {
        All = -1,
        Window = 1,
        View = 2,
        Server = 4,
        SpeechKit = 8,
        ToReplaceForVoice = 16,
        Box = 32,
        Optimise = 64,
        CommandBuffer = 128,
        App = 256
    };
#ifndef ENABLE_USE_ENUM_NAMESPACES
};
#else
    Q_ENUMS(SaveSectionEnum)
}
#endif
// ----------------------------------------------------------------------------

#ifndef ENABLE_USE_ENUM_NAMESPACES
class CCBotErrEnums : public QObject
{
    Q_OBJECT
    Q_ENUMS(CCBotErrEnum)

 public:
    explicit CCBotErrEnums(QObject *parent = nullptr): QObject(parent) {}
#else
namespace CCBotErrEnums {
    Q_NAMESPACE
#endif
    enum CCBotErrEnum {
        NoInit = -1,
        Ok = 0,
        ParseJson,
        Sql,
        NetworkRequest
    };
#ifndef ENABLE_USE_ENUM_NAMESPACES
};
#else
    Q_ENUMS(CCBotErrEnum)
}
#endif
// ----------------------------------------------------------------------------

#ifndef ENABLE_USE_ENUM_NAMESPACES
class BoxFlagsEnums : public QObject
{
    Q_OBJECT
    Q_ENUMS(BoxFlagsEnum)

 public:
    explicit BoxFlagsEnums(QObject *parent = nullptr): QObject(parent) {}
#else
namespace BoxFlagsEnums {
    Q_NAMESPACE
#endif
    enum BoxFlagsEnum {
        FLAG_SPEECH_ON = 0,
        FLAG_SHOWED_MSG_NO_MONEY_FOR_SPEECH = 1,
        FLAG_DRUNK = 2,
    };
#ifndef ENABLE_USE_ENUM_NAMESPACES
};
#else
    Q_ENUMS(BoxFlagsEnum)
}
#endif
// ----------------------------------------------------------------------------

#ifndef ENABLE_USE_ENUM_NAMESPACES
class SpeakReasonEnums : public QObject
{
    Q_OBJECT
    Q_ENUMS(SpeakReasonEnum)

 public:
    explicit SpeakReasonEnums(QObject *parent = nullptr): QObject(parent) {}
#else
namespace SpeakReasonEnums {
    Q_NAMESPACE
#endif
    enum SpeakReasonEnum {
        DisableAll = 0,
        EnableAll,
        Donation,
        BalanceSpending
    };
#ifndef ENABLE_USE_ENUM_NAMESPACES
};
#else
    Q_ENUMS(SpeakReasonEnum)
}
#endif
// ----------------------------------------------------------------------------

#ifndef ENABLE_USE_ENUM_NAMESPACES
class TrialConstEnums : public QObject
{
    Q_OBJECT
    Q_ENUMS(TrialConstEnum)

 public:
    explicit TrialConstEnums(QObject *parent = nullptr): QObject(parent) {}
#else
namespace TrialConstEnums {
    Q_NAMESPACE
#endif
    enum TrialConstEnum {
        TrialStartProgramInSec = 60,
        TrialWorkQuotaInUSec = 30000,
        TrialRegenInSec = 120
    };
#ifndef ENABLE_USE_ENUM_NAMESPACES
};
#else
    Q_ENUMS(TrialConstEnum)
}
#endif
// ----------------------------------------------------------------------------

#endif // ENUMS_H
