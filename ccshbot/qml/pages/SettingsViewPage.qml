import QtQuick 2.12

import ccbot.enums 1.0

SettingsViewPageForm {
    id: page

    title: qsTr("View")

    fontSelector.model: Qt.fontFamilies()
    fontSelector.onActivated: {
        properties.fontNameForChat =
                fontSelector.textAt(fontSelector.currentIndex);
    }

    fontSelectSize {
        value: properties.fontPointSizeForChat
        from: 1.0
        to: 32.0
        step: 0.1
        precision: 1
        enableSequenceGrid: true
        editable: true
        suffix: "pt"
    }
    fontSelectSize.onFinishEdit: {
        properties.fontPointSizeForChat = number;
    }

    fontSelectTest {
        text: qsTr("That's how it will be!") + "\n"
                                + String.fromCodePoint(0x1F601) //😁
                                + String.fromCodePoint(0x1F602) //😂
                                + String.fromCodePoint(0x1F603) //😃
                                + String.fromCodePoint(0x1F604) //😄
                                + String.fromCodePoint(0x1F605) //😅
                                + String.fromCodePoint(0x1F606) //😆
                                + String.fromCodePoint(0x1F609) //😉
                                + String.fromCodePoint(0x1F60A) //😊
                                + String.fromCodePoint(0x1F60B) //😋
                                + String.fromCodePoint(0x1F60C) //😌
                                + String.fromCodePoint(0x1F60D) //😍
                                + String.fromCodePoint(0x1F60F) //😏
                                + String.fromCodePoint(0x1F612) //😒
                                + String.fromCodePoint(0x1F613) + "\n" //😓
                                + String.fromCodePoint(0x1F614) //😔
                                + String.fromCodePoint(0x1F616) //😖
                                + String.fromCodePoint(0x1F618) //😘
                                + String.fromCodePoint(0x1F61A) //😚
                                + String.fromCodePoint(0x1F61C) //😜
                                + String.fromCodePoint(0x1F61D) //😝
                                + String.fromCodePoint(0x1F61E) //😞
                                + String.fromCodePoint(0x1F620) //😠
                                + String.fromCodePoint(0x1F621) //😡
                                + String.fromCodePoint(0x1F622) //😢
                                + String.fromCodePoint(0x1F623) //😣
                                + String.fromCodePoint(0x1F624) //😤
                                + String.fromCodePoint(0x1F625) //😥
                                + String.fromCodePoint(0x1F628) + "\n" //😨
                                + String.fromCodePoint(0x1F629) //😩
                                + String.fromCodePoint(0x1F62A) //😪
                                + String.fromCodePoint(0x1F62B) //😫
                                + String.fromCodePoint(0x1F62D) //😭
                                + String.fromCodePoint(0x1F630) //😰
                                + String.fromCodePoint(0x1F631) //😱
                                + String.fromCodePoint(0x1F632) //😲
                                + String.fromCodePoint(0x1F633) //😳
                                + String.fromCodePoint(0x1F635) //😵
                                + String.fromCodePoint(0x1F637) //😷
                                + String.fromCodePoint(0x1F638) //😸
                                + String.fromCodePoint(0x1F639) //😹
                                + String.fromCodePoint(0x1F63A) //😺
                                + String.fromCodePoint(0x1F63B) + "\n" //😻
                                + String.fromCodePoint(0x1F63C) //😼
                                + String.fromCodePoint(0x1F63D) //😽
                                + String.fromCodePoint(0x1F63E) //😾
                                + String.fromCodePoint(0x1F63F) //😿
                                + String.fromCodePoint(0x1F640) //🙀
                                + String.fromCodePoint(0x1F645) //🙅
                                + String.fromCodePoint(0x1F646) //🙆
                                + String.fromCodePoint(0x1F647) //🙇
                                + String.fromCodePoint(0x1F648) //🙈
                                + String.fromCodePoint(0x1F649) //🙉
                                + String.fromCodePoint(0x1F64A) //🙊
                                + String.fromCodePoint(0x1F64B) //🙋
                                + String.fromCodePoint(0x1F64C) + "\n" //🙌
                                + String.fromCodePoint(0x1F64D) //🙍
                                + String.fromCodePoint(0x1F64E) //🙎
                                + String.fromCodePoint(0x1F64F) //🙏
                                + String.fromCodePoint(0x1F600) //😀
                                + String.fromCodePoint(0x1F607) //😇
                                + String.fromCodePoint(0x1F608) //😈
                                + String.fromCodePoint(0x1F60E) //😎
                                + String.fromCodePoint(0x1F610) //😐
                                + String.fromCodePoint(0x1F611) //😑
                                + String.fromCodePoint(0x1F615) //😕
                                + String.fromCodePoint(0x1F617) //😗
                                + String.fromCodePoint(0x1F619) //😙
                                + String.fromCodePoint(0x1F61B) //😛
                                + String.fromCodePoint(0x1F61F) + "\n" //😟
                                + String.fromCodePoint(0x1F626) //😦
                                + String.fromCodePoint(0x1F627) //😧
                                + String.fromCodePoint(0x1F62C) //😬
                                + String.fromCodePoint(0x1F62E) //😮
                                + String.fromCodePoint(0x1F62F) //😯
                                + String.fromCodePoint(0x1F634) //😴
                                + String.fromCodePoint(0x1F636) //😶

        font.pointSize: properties.fontPointSizeForChat
        font.family: properties.fontNameForChat
    }

    Component.onCompleted: {
        fontSelector.currentIndex = fontSelector.find(properties.fontNameForChat);
    }

    Component.onDestruction: {
        ccbot.saveSettings(SaveSection.View);
    }
}
