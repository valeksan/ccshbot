#ifndef ENUMS_H
#define ENUMS_H

#include <QObject>

namespace PageTypeEnums {
    Q_NAMESPACE
    enum PageTypeEnum {
        NoPage = -1,
        AuthPage,
        RoomPage,
    };
    Q_ENUMS(PageTypeEnum)
}

namespace CCBotTaskEnums {
    Q_NAMESPACE
    enum CCBotTaskEnum {
        Auth = 0,
        ConnectToRoom,
        ParseChat,
        Voice
    };
    Q_ENUMS(CCBotTaskEnum)
}

#endif // ENUMS_H
