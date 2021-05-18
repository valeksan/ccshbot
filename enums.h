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

#endif // ENUMS_H
