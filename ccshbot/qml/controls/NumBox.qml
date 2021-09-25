import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

NumBoxForm {
    id: control

    property int precision: 2
    property int decimalPlaces: precision
    property double value: 0.0
    property var memory: NaN
    property double step: 0.0
    property bool enableSequenceGrid: false
    property double from: -3.40282e+038/2.0
    property alias minimumValue: control.from
    property double to: 3.40282e+038/2.0
    property alias maximumValue: control.to
    property bool fixed: false

    property string suffix: ""
    property string prefix: ""
    property bool strongIntegerPartOfNumber: false
    property color colorMainPartOfValue: "transparent"
    property color colorFractionPartOfValue: "transparent"
    property color colorDotePartOfValue: colorFractionPartOfValue
    property color colorSuffix: "transparent"
    property color colorPrefix: "transparent"
    property bool visibleSuffixInEdit: true
    property color backgroundColor: "#4E4E4E"   //"white"

    property bool editable: false
    property bool doubleClickEdit: false
    property bool enableEditPanel: false
    property string displayTextValue: viewRegimeMethods.getDisplayValueString()

    signal finishEdit(double number);
    signal showCustomEditPanel(string name, double current);
    signal clicked(QtObject mouse);
    signal doubleClicked(QtObject mouse);
    signal editStart();
    signal editEnd();
    signal up();
    signal down();


    padding: 3
    wheelEnabled: editable

    background: Rectangle {
        opacity: enabled ? 1 : 0.3
        Material.elevation: 4
        color: backgroundColor//"#EEEEEE"
        radius: 5
    }

    //Material.foreground: Material.Pink

    function saveValueInMemory() {
        memory = value;
    }
    function clearValueInMemory() {
        memory = NaN;
    }
    function loadValueFromMemory() {
        control.finishEdit(memory);
    }
    function copy() {
        clipboard.copy(textFromValue(value, precision));
    }
    function past() {
        if (clipboard.canPast()) {
            let number = valueFromText(clipboard.past())
            if (valueInRange(number)) {
                if (mathMethods.isNumberInSequenceGrid())
                    control.finishEdit(mathMethods.adj(number));
                else
                    control.finishEdit(number);
            }
        }
    }

    TextEdit {
        id: clipboard
        text: ""
        visible: false

        function copy(_text) {
            clipboard.text = _text;
            selectAll();
            copy();
        }

        function cut(_text) {
            clipboard.text = _text;
            selectAll();
            cut();
        }

        function canPast() {
            return canPaste;
        }

        function past() {
            if (canPaste) {
                clipboard.text = " ";
                selectAll();
                paste();
                return clipboard.text;
            }
            return "";
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        enabled: wheelEnabled
        onWheel: {
            if (wheel.angleDelta.y < 0) {
                if (!inputText.activeFocus)
                    control.down();
                else
                    inputRegimeMethods.decrease();
            } else {
                if (!inputText.activeFocus)
                    control.up();
                else
                    inputRegimeMethods.increase();
            }
        }
    }

    displayText.text: displayTextValue
    displayTextMouseArea.onClicked: {
        control.clicked(mouse);
        if (!control.doubleClickEdit) {
            if (control.editable) {
                if (!control.enableEditPanel) {
                    inputText.forceActiveFocus();
                    control.editStart();
                } else {
                    if (control.editable) {
                        control.editStart();
                        control.showCustomEditPanel(control.name, control.value);
                    }
                }
            }
        } else {
            mouse.accepted = false;
        }
    }
    displayTextMouseArea.onDoubleClicked: {
        control.doubleClicked(mouse);
        if (control.doubleClickEdit) {
            if (control.editable) {
                if (!control.enableEditPanel) {
                    inputText.forceActiveFocus();
                    control.editStart();
                } else {
                    control.editStart();
                    control.showCustomEditPanel(control.name, control.value);
                }
            }
        } else {
            mouse.accepted = false;
        }
    }

    inputTextMouseArea.onClicked: {
        inputRegimeMethods.counterNumPutSymbols = 0;
        displayText.forceActiveFocus();
        control.editEnd();
    }
    inputText.validator: DoubleValidator {
        bottom: minimumValue
        top: maximumValue
        decimals: control.precision
        notation: DoubleValidator.StandardNotation
    }
    inputText.readOnly: !control.editable
    inputText.inputMethodHints: Qt.ImhFormattedNumbersOnly
    inputText.selectByMouse: true
    inputText.onTextChanged: {
        const number = controlInMethods.valueFromText(inputText.text);
        if (number > maximumValue) {
            inputRegimeMethods.counterToUpErrors += 1;
            if (inputRegimeMethods.counterToUpErrors < 2) {
                inputText.text = inputText.text.substring(0, inputText.text.length - 1);
            } else {
                inputRegimeMethods.counterToUpErrors = 0;
                inputText.text = controlInMethods.textFromValue(maximumValue, precision);
            }
        } else if (number < minimumValue) {
            inputRegimeMethods.counterToDownErrors += 1;
            if (inputRegimeMethods.counterToDownErrors < 2) {
                inputText.text = inputText.text.substring(0, inputText.text.length - 1);
            } else {
                inputRegimeMethods.counterToDownErrors = 0;
                inputText.text = controlInMethods.textFromValue(minimumValue, precision);
            }
        }
        inputRegimeMethods.counterNumPutSymbols += 1;
        if (inputRegimeMethods.counterNumPutSymbols < inputText.text.length) {
            while (inputRegimeMethods.fixInput());
        }
    }
    inputText.onEditingFinished: {
        inputRegimeMethods.counterNumPutSymbols = 0;
        inputRegimeMethods.valueEditFinisher();
    }

    placeholderText.text: controlInMethods.textFromValue(value, precision)

    sufInfoInEdit.text: control.suffix

    Keys.onEscapePressed: {
        if (inputText.activeFocus) {
            inputRegimeMethods.counterNumPutSymbols = 0;
            inputText.text = "";
            displayText.forceActiveFocus();
            control.editEnd();
        }
    }
    Keys.onReleased: {
        if (inputText.activeFocus) {
            if (event.key === Qt.Key_M) {
                inputText.text = controlInMethods.textFromValue(memory, precision);
            } else if (event.key === Qt.Key_C) {
                inputText.text = "";
            } else if (event.key === 46 || event.key === 44) {
                if (controlInMethods.getSystemLocaleDecimalChar() === ',' && event.key === 46) {
                    inputText.insert(inputText.cursorPosition, ',');
                } else if (controlInMethods.getSystemLocaleDecimalChar() === '.' && event.key === 44) {
                    inputText.insert(inputText.cursorPosition, '.');
                }
            }
        }
    }

    QtObject {
        id: controlInMethods
        function getSystemLocaleDecimalChar() {
            return Qt.locale("").decimalPoint;
        }
        function isColorEmpty(_color) {
            return Qt.colorEqual(_color, "transparent");
        }
        function textFromValue(_number, _precision) {
            const doteSymbol = getSystemLocaleDecimalChar();
            let index_dote = -1;
            let text = _number.toFixed(_precision);
            if (doteSymbol !== '.') {
                text = text.replace('.', doteSymbol);
            }
            index_dote = text.indexOf(doteSymbol);
            if (!fixed && _precision > 0) {
                for (let i = 0; i < (_precision + 1); i++) {
                    if (text.charAt(text.length - i - 1) !== '0') {
                        text = text.substr(0, text.length - i);
                        break;
                    }
                }
                if (text.charAt(text.length - 1) === doteSymbol) {
                    text = text.substring(0, text.length - 1);
                }
            }
            if (strongIntegerPartOfNumber && text.length > 0) {
                if (index_dote !== -1) {
                    text = [ text.slice(0, index_dote), "</strong>", text.slice(index_dote) ].join('');
                    text = "<strong>" + text;
                } else {
                    text = "<strong>" + text + "</strong>";
                }
            }
            if (!isColorEmpty(colorMainPartOfValue)) {
                index_dote = text.indexOf(doteSymbol);
                if (index_dote !== -1) {
                    text = [ text.slice(0, index_dote), "</font>", text.slice(index_dote) ].join('');
                    text = "<font color=\"" + colorMainPartOfValue + "\">" + text;
                } else {
                    text = "<font color=\"" + colorMainPartOfValue + "\">" + text + "</strong>";
                }
            }
            if (!isColorEmpty(colorFractionPartOfValue)) {
                index_dote = text.indexOf(doteSymbol);
                if (index_dote !== -1) {
                    text = [ text.slice(0, index_dote + 1), "<font color=\"" + colorFractionPartOfValue + "\">", text.slice(index_dote + 1), "</font>" ].join('');
                }
            }
            if (!isColorEmpty(colorDotePartOfValue)) {
                index_dote = text.indexOf(doteSymbol);
                if (index_dote !== -1) {
                    text = [ text.slice(0, index_dote), "<font color=\"" + colorDotePartOfValue + "\">", doteSymbol , text.slice(index_dote + 1), "</font>" ].join('');
                }
            }
            if (control.name === "testOutput") {
                console.log(text);
            }
            return text;
        }
        function valueFromText(_text) {
            let index_dote = -1;
            let textConv = _text;
            index_dote = textConv.indexOf(".");
            if (index_dote === -1) {
                index_dote = textConv.indexOf(",");
            }
            if (index_dote !== -1) {
                textConv = textConv.replace(',', '.');
            }
            return parseFloat(textConv);
        }
        function valueInRange(_x) {
            if (_x >= minimumValue && _x <= maximumValue) {
                return true;
            }
            return false;
        }
    }

    QtObject {
        id: mathMethods
        function fixValue(_x, _precision) {
            return Math.round(_x * Math.pow(10, _precision)) / Math.pow(10, _precision);
        }
        function isNumberInSequenceGrid(_fstep, _number, _precision) {
            if (isNaN(_number)) {
                return false;
            }
            let tmp_value = fixValue(_number, _precision);
            let valPr = parseInt((tmp_value * Math.pow(10, _precision)).toFixed(0));
            let valArgPr = parseInt((_fstep * Math.pow(10, _precision)).toFixed(0));
            if ((valPr % valArgPr) === 0) {
                return true;
            }
            return false;
        }
        function adj(_x) {
            return (Math.round((_x - minimumValue) / step) * step + minimumValue);
        }
    }

    QtObject {
        id: inputRegimeMethods

        property int counterToUpErrors: 0
        property int counterToDownErrors: 0
        property int counterNumPutSymbols: 0

        function fixInput() {
            const number = controlInMethods.valueFromText(inputText.text);
            if (inputText.text.length === 0) {
                return false;
            }
            if (number > maximumValue || number < minimumValue) {
                inputText.text = inputText.text.substring(0, inputText.text.length - 1);
                return true;
            }
            return false;
        }
        function valueEditFinisher() {
            if (inputText.text.length > 0) {
                const rValue = controlInMethods.valueFromText(inputText.text);
                const newValue = controlInMethods.valueFromText(placeholderText.text);
                if (rValue !== newValue) {
                    if (control.enableSequenceGrid) {
                        if (mathMethods.isNumberInSequenceGrid(step, rValue, precision)) {
                            control.finishEdit(rValue);
                        } else {
                            let conv_value = mathMethods.adj(rValue);
                            if (conv_value > maximumValue) {
                                conv_value -= step;
                            }
                            conv_value = mathMethods.fixValue(conv_value, precision);
                            control.finishEdit(conv_value);
                        }
                    } else {
                        control.finishEdit(rValue);
                    }
                }
                control.editEnd();
                displayText.forceActiveFocus();
                inputText.clear();
            }
        }
        function decrease() {
            let numberStr;
            if (inputText.text.length === 0) {
                numberStr = placeholderText.text;
            } else {
                numberStr = inputText.text;
            }
            const number = mathMethods.fixValue((controlInMethods.valueFromText(numberStr) - step), precision);
            if (number >= minimumValue) {
                inputText.text = controlInMethods.textFromValue(number, precision);
            }
        }
        function increase() {
            let numberStr;
            if (inputText.text.length === 0) {
                numberStr = placeholderText.text;
            } else {
                numberStr = inputText.text;
            }
            const number = mathMethods.fixValue((controlInMethods.valueFromText(numberStr) + step), precision);
            if (number <= maximumValue) {
                inputText.text = controlInMethods.textFromValue(number, precision);
            }
        }
    }

    QtObject {
        id: viewRegimeMethods
        function increase() {
            if (mathMethods.fixValue((value + step), precision) <= maximumValue) {
                control.finishEdit(mathMethods.fixValue((value + step), precision));
            }
        }
        function decrease() {
            if (mathMethods.fixValue((value - step), precision) >= minimumValue) {
                control.finishEdit(mathMethods.fixValue((value - step), precision));
            }
        }
        function getDisplayValueString() {
            if (value.toString().length > 0) {
                const prefixFull = controlInMethods.isColorEmpty(colorPrefix) ? prefix : "<font color=\"" + colorPrefix + "\">" + prefix + "</font>";
                const suffixFull = controlInMethods.isColorEmpty(colorSuffix) ? suffix : "<font color=\"" + colorSuffix + "\">" + suffix + "</font>";
                return (prefixFull + controlInMethods.textFromValue(value, decimalPlaces) + suffixFull);
            }
            return "";
        }
    }

    onUp: {
        viewRegimeMethods.increase();
    }
    onDown: {
        viewRegimeMethods.decrease();
    }

    onEnableSequenceGridChanged: {
        if (enableSequenceGrid) {
            if (!mathMethods.isNumberInSequenceGrid(step, value, precision)) {
                control.finishEdit(mathMethods.adj(value));
            }
        }
    }
}
