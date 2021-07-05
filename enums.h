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
        CloseBase
    };
    Q_ENUMS(CCBotTaskEnum)
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
    };
    Q_ENUMS(BoxFlagsEnum)
}

#endif // ENUMS_H
