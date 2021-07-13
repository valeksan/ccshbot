#ifndef ENUMS_H
#define ENUMS_H

#include <QObject>

namespace CCBotTaskEnums {
    Q_NAMESPACE
    enum CCBotTaskEnum {
        MergeChat = 1,
        VoiceLoad,
        VoiceSpeech,
        OpenBase,
        CloseBase,
        SysCmdSetVoice,
        SysCmdDrink,
        SysCmdBalance,
    };
    Q_ENUMS(CCBotTaskEnum)
}

namespace SaveSectionEnums {
    Q_NAMESPACE
    enum SaveSectionEnum {
        All = -1,
        Window = 1,
        View = 2,
        Server = 4,
        SpeechKit = 8,
        ToReplaceForVoice = 16,
        Box = 32,
        Optimise = 64,
        CommandBuffer = 128
    };
    Q_ENUMS(SaveSectionEnum)
}

namespace CCBotErrEnums {
    Q_NAMESPACE

    enum CCBotErrEnum {
        NoInit = -1,
        Ok = 0,
        ParseJson,
        Sql,
        NetworkRequest
    };
    Q_ENUMS(CCBotErrEnum)
}

namespace BoxFlagsEnums {
    Q_NAMESPACE

    enum BoxFlagsEnum {
        FLAG_SPEECH_ON = 0,
        FLAG_SHOWED_MSG_NO_MONEY_FOR_SPEECH = 1,
        FLAG_DRUNK = 2,
    };
    Q_ENUMS(BoxFlagsEnum)
}

namespace SpeakReasonEnums {
    Q_NAMESPACE

    enum SpeakReasonEnum {
        DisableAll = 0,
        EnableAll,
        Donation,
        BalanceSpending
    };
    Q_ENUMS(SpeakReasonEnum)
}

#endif // ENUMS_H
