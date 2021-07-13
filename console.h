#ifndef CONSOLE_H
#define CONSOLE_H

#include <QObject>
#include <QHash>
#include <QPair>
#include <QVariant>
#include <QSettings>
#include <QDir>

class Console: public QObject
{
    Q_OBJECT

    Q_PROPERTY(int lastCommandsBufferMaxSize READ lastCommandsBufferMaxSize WRITE setLastCommandsBufferMaxSize NOTIFY lastCommandsBufferMaxSizeChanged)
    Q_PROPERTY(int currentIndexCommandsBuffer READ currentIndexCommandsBuffer WRITE setCurrentIndexCommandsBuffer NOTIFY currentIndexCommandsBufferChanged)

    QStringList m_textBufferStack;
    QHash<QString,int> m_commandTypes;
    QHash<QString,QVariant> m_attributes;

    QStringList m_lastCommandsBuffer;
    int m_lastCommandsBufferMaxSize;
    int m_currentIndexCommandsBuffer;

    void _addCommandToBuffer(QString text);
    static bool _takeInlineStringInCommandString(QString &text, QString &findStr, int &n); // выявление первой вложенной строки в строке и вырезание ее в переданную переменную

    bool m_enableCommandStack;

public:
    explicit Console(QObject *parent = 0);

    static QString getProgressLine(double value, double size, int progressSize = 32, QChar stepSym = QChar('#'), QChar spaceSym = QChar('_'));

    int getType(QString alias);
    void registerTaskTypeAlias(QString alias, int type); // зарегистритовать id - комманды по строке
    void removeTaskType(int type);
    void removeAlias(QString alias);
    void setAttribute(QString attrName, QVariant value);
    QString textBuffer() const;
    int currentIndexCommandsBuffer() const;
    bool enableCommandStack() const;

public slots:
    void loadCommandBufferStackSaves();
    void saveCommandBufferStack();

    QString upCommandInBuffer();
    QString downCommandInBuffer();

    void exec(QString sender, QString text);
    void appendTextBufferStack(QString text);
    int currentBufferPos();
    void displayMessage(QString msg);
    void replaceMessage(QString msg, int nlines = 1);
    void removeLastMessageLines(int nlines = 1);
    void clearDisplay();
    void clearBufferStack();

    static QStringList getArgsFromString(QString str);              // получить из строки список аргументов (для работы терминала)
    static int searchKeyIndex(QString key, QStringList args, QString initSymbols = "="); // поиск ключа и возврат индекса ключа
    static QStringList argsToStringList(QVariantList args);         // поиск ключа и возврат индекса ключа
    static QString getArgKeyValue(QString key, QVariantList args, QString initSymbol = "=");  // поиск ключа и возврат "undefined" если ключ найден но не инициализирован, если не найден то пустая строка, иначе значение
    static QByteArray getArgDataValue(QVariantList args, QString key = "d", QString initSymbols = ":"); // поиск данных (с суфиксом ":[suffix] XX XX XX XX")

    int lastCommandsBufferMaxSize() const;
    void setLastCommandsBufferMaxSize(int lastCommandsBufferMaxSize);
    void setCurrentIndexCommandsBuffer(int currentIndexCommandsBuffer);
    void setEnableCommandStack(bool enableCommandStack);

signals:
    void textBufferChanged(QString textBuffer);
    void appendBufferChanged(QString text);
    void runCommand(QString sender, QString cmd, QStringList args);
    void displayTextBuffer(QString text);
    void lastCommandsBufferMaxSizeChanged(int lastCommandsBufferMaxSize);
    void currentIndexCommandsBufferChanged(int currentIndexCommandsBuffer);
    void onTestChadged(QVariant arg);
    void message(QString msg);
    void messageReplace(QString msg, int nlines);
    void removeLastLines(int nlines);
    void clear();
    void enableCommandStackChanged(bool enableCommandStack);
};

inline QString Console::textBuffer() const
{
    QString buffer = QString(QStringList(m_textBufferStack).join("\n"));
    return buffer;
}

inline int Console::currentIndexCommandsBuffer() const
{
    return m_currentIndexCommandsBuffer;
}

inline bool Console::enableCommandStack() const
{
    return m_enableCommandStack;
}

inline int Console::lastCommandsBufferMaxSize() const
{
    return m_lastCommandsBufferMaxSize;
}

inline void Console::setLastCommandsBufferMaxSize(int lastCommandsBufferMaxSize)
{
    if (m_lastCommandsBufferMaxSize == lastCommandsBufferMaxSize)
        return;

    m_lastCommandsBufferMaxSize = lastCommandsBufferMaxSize;
    emit lastCommandsBufferMaxSizeChanged(lastCommandsBufferMaxSize);
}

inline void Console::setCurrentIndexCommandsBuffer(int currentIndexCommandsBuffer)
{
    if (m_currentIndexCommandsBuffer == currentIndexCommandsBuffer)
        return;

    m_currentIndexCommandsBuffer = currentIndexCommandsBuffer;
    emit currentIndexCommandsBufferChanged(currentIndexCommandsBuffer);
}

inline void Console::setEnableCommandStack(bool enableCommandStack)
{
    if (m_enableCommandStack == enableCommandStack)
        return;

    m_enableCommandStack = enableCommandStack;
    emit enableCommandStackChanged(enableCommandStack);
}

Q_DECLARE_METATYPE(QList<QString>)

#endif // CONSOLE_H
