#ifndef ENUMS_H
#define ENUMS_H

#include <QObject>

namespace PageTypeEnums {
    Q_NAMESPACE
    enum PageTypeEnum {
        NoPage = -1,
        ChatPage,
    };
    Q_ENUMS(PageTypeEnum)
}

namespace CCBotTaskEnums {
    Q_NAMESPACE
    enum CCBotTaskEnum {
        MergeChat = 1,
        Voice,
        OpenBase,
        CloseBase,
        LoadChat
    };
    Q_ENUMS(CCBotTaskEnum)
}

namespace CCBotErrEnums {
    Q_NAMESPACE

    enum CCBotErrEnum {
        NoInit = -1,
        Ok = 0,
        ParseJson,
        Sql
    };
    Q_ENUMS(CCBotErrEnum)
}

#endif // ENUMS_H
