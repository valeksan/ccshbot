#include "console.h"
#include <QDebug>
#include <QStringList>

Console::Console(QObject *parent) : QObject(parent) {
    m_lastCommandsBufferMaxSize = 20;
    m_currentIndexCommandsBuffer = -1;
    m_enableCommandStack = false;
}

QString Console::getProgressLine(double value, double size, int progressSize, QChar stepSym, QChar spaceSym) {
    QString strProgressLine = "";
    QString result = "";
    double percent = value*100.0/size;
    double percentStepSize = 100.0/progressSize;
    int numSteps = static_cast<int>(percent/percentStepSize);
    for(int i=0; i<numSteps; i++) strProgressLine += stepSym;
    for(int i=numSteps; i<progressSize; i++) strProgressLine += spaceSym;
    result = QString("[%2][%3]").arg(strProgressLine).arg(QString::number(percent,'f',2));
    return result;
}

QStringList Console::findItems(QString findText)
{
    // !!! функция не доделана !!!
    QStringList args = getArgsFromString(findText);
    QStringList result;
    if(args.empty()) {
        return args;
    }

    if(args.size() == 1) {
        // если аргумент 1 то перечисление схожих команд
        QList<QString> cmds = m_commandTypes.keys();
        for(int i = 0; i < cmds.size(); i++) {
            bool find = true;
            QString cmd = cmds.at(i);
            if(cmd.length() < findText.length()) {
                continue;
            }
            if(cmd.mid(0,findText.length()) == findText) {
                result.append(cmd);
            }
        }
    } else {
        // иначе берется последний аргумент и определяется типовое назначение аргумента по формату
        QString fullPath = args.last();
        QString lastSectionPath = fullPath.section(QDir::separator(),0,-2);
        QString fileName = fullPath.section(QDir::separator(),-1,-1);
        QDir dir(lastSectionPath);
        /*
        if(dir.exists()) {
            // корневая директория существует, значит это путь
            // поиск конца существующего пути
            QStringList dirNodes = fullPath.split(QDir::separator());
            int indexDirNotFound = 0;
            for(int lenNodes=2; lenNodes<=dirNodes.size(); lenNodes++) {
                QString testPath = dirNodes.mid(0,lenNodes).join(QDir::separator());
                QDir testDir(testPath);
                if(!testDir.exists()) {
                    indexDirNotFound = lenNodes - 1;
                    break;
                }
            }
        }
        */
    }
    return result;
}

void Console::_addCommandToBuffer(QString text)
{
    if(m_lastCommandsBuffer.contains(text)) {
        m_lastCommandsBuffer.removeAll(text);
    }
    m_lastCommandsBuffer.append(text);
    m_currentIndexCommandsBuffer = m_lastCommandsBuffer.size()-1;
}

bool Console::_takeInlineStringInCommandString(QString& text, QString& findStr, int& n)
{
    int indexSearchAA = -1;
    int indexSearchA = -1;
    int indexStart = -1;
    int indexStop = -1;
    QChar searchSymbol;
    indexSearchAA = text.indexOf(QChar('\"'));
    indexSearchA = text.indexOf(QChar('\''));
    findStr = "";
    n = -1;
    bool isFoundAA = (indexSearchAA != -1);
    bool isFoundA = (indexSearchA != -1);
    // поиск начала и выбор символа начала строки
    if(isFoundAA && isFoundA) {
        if(indexSearchA < indexSearchAA) {
            indexStart = indexSearchA;
            searchSymbol = QChar('\'');
        } else {
            indexStart = indexSearchAA;
            searchSymbol = QChar('\"');
        }
    } else if(isFoundAA) {
        indexStart = indexSearchAA;
        searchSymbol = QChar('\"');
    } else if(isFoundA) {
        indexStart = indexSearchA;
        searchSymbol = QChar('\'');
    } else {
        return false;
    }
    // поиск конца по выбранному символу
    indexStop = text.indexOf(searchSymbol, indexStart+1);
    if(indexStop != -1) {
        findStr += text.mid(indexStart+1, indexStop-indexStart-1);
        text.remove(indexStart, findStr.length()+2);
    } else {
        findStr += text.mid(indexStart+1);
        text.remove(indexStart, findStr.length()+1);
    }
    text.insert(indexStart," ");
    //qDebug() << text;
    QString leftPartText = text.simplified().mid(0,indexStart-2);
    n = leftPartText.count(QChar(' '))+1;
    text = text.simplified();
    return true;
}

void Console::loadCommandBufferStackSaves()
{
    QSettings bank;     //(QString(QDir::homePath()+QDir::separator()+".clearsky.ini"), QSettings::IniFormat);//(ORGANIZATION_STRING,APPLICATION_STRING);
    bank.beginGroup("command_stack");
    int size = bank.beginReadArray("commands");
    m_lastCommandsBuffer.clear(); // очистить устаревшие данные (если есть)
    for(int i = 0; i < size; ++i) {
        bank.setArrayIndex(i);
        m_lastCommandsBuffer.append(bank.value("command").toString());
    }
    bank.endArray();
    bank.endGroup();
}

void Console::saveCommandBufferStack()
{
    QSettings bank;     //(QString(QDir::homePath()+QDir::separator()+".clearsky.ini"), QSettings::IniFormat);//ORGANIZATION_STRING,APPLICATION_STRING);
    bank.beginGroup("command_stack");
    bank.beginWriteArray("commands");
    for(int i = 0; i < m_lastCommandsBuffer.size(); ++i) {
        bank.setArrayIndex(i);
        bank.setValue("command", m_lastCommandsBuffer.value(i));
    }
    bank.endArray();
    bank.endGroup();
}

void Console::test(QString text) {
    Q_UNUSED(text)
}

QString Console::upCommandInBuffer()
{
    if(m_lastCommandsBufferMaxSize == 0 || m_lastCommandsBuffer.isEmpty()) {
        return "";
    }
    if(m_currentIndexCommandsBuffer == -1) {
        setCurrentIndexCommandsBuffer(m_lastCommandsBuffer.size()-1);
    } else if(m_currentIndexCommandsBuffer > 0) {
        setCurrentIndexCommandsBuffer(m_currentIndexCommandsBuffer-1);
    }
    return m_lastCommandsBuffer.value(m_currentIndexCommandsBuffer,"");
}

QString Console::downCommandInBuffer()
{
    if(m_lastCommandsBufferMaxSize == 0 || m_lastCommandsBuffer.isEmpty() || m_currentIndexCommandsBuffer == -1) {
        return "";
    }
    if(m_currentIndexCommandsBuffer >= m_lastCommandsBuffer.size()-1) {
        setCurrentIndexCommandsBuffer(m_lastCommandsBuffer.size()-1);
        return m_lastCommandsBuffer.last();
    } else {
        setCurrentIndexCommandsBuffer(m_currentIndexCommandsBuffer+1);
    }
    return m_lastCommandsBuffer.value(m_currentIndexCommandsBuffer,"");
}

void Console::exec(QString sender, QString text)
{
    QStringList args;
    QString simplText = text.simplified();
    bool isArgsFound = simplText.contains(QChar(' '));
    QString cmd;
    if(isArgsFound) {
        cmd = simplText.section(QChar(' '), 0, 0);
        simplText.remove(0, cmd.length()+1);
        args = getArgsFromString(simplText);
    } else {
        cmd = simplText;
    }
    if(m_lastCommandsBufferMaxSize != 0) {
        _addCommandToBuffer(text);
    }
    qDebug() << sender << cmd << args;
    emit runCommand(sender, cmd, args);
}

void Console::appendTextBufferStack(QString text) {
    if(m_textBufferStack.size() < m_lastCommandsBufferMaxSize) {
        m_textBufferStack.append(text);
        emit appendBufferChanged(text);
    } else if(m_textBufferStack.size() > 0) {
        m_textBufferStack.removeAt(0);
        m_textBufferStack.append(text);
        if(m_currentIndexCommandsBuffer > 0) {
            m_currentIndexCommandsBuffer -= 1;
        }
        emit appendBufferChanged(text);
    }
}

int Console::getType(QString alias)
{
    return m_commandTypes.value(alias, -1);
}

QStringList Console::getArgsFromString(QString str)
{
    QStringList args;
    QList<QPair<QString,int> > strArgs;
    QString findStr = "";
    QPair<QString,int> argv;
    int n;
    while(Console::_takeInlineStringInCommandString(str, findStr, n)) {
        //strArgs.append(findStr);
        argv.first = findStr;
        argv.second = n+strArgs.size();
        strArgs.append(argv);
    }
    str = str.simplified();
    args = str.split(" ");
    for(int i=0; i<strArgs.size(); i++) {
        args.insert(strArgs[i].second, strArgs[i].first);
    }
    return args;
}

int Console::searchKeyIndex(QString key, QStringList args, QString initSymbols)
{
    int index = -1;
    for(int i = 0; i < args.size(); i++) {
        if(args[i].section(initSymbols,0,0) == key) {
            index = i;
            break;
        }
    }
    return index;
}

QStringList Console::argsToStringList(QVariantList args)
{
    QStringList list;
    for(int i = 0; i < args.size(); i++) {
        list << args.value(i,"").toString();
    }
    return list;
}

QString Console::getArgKeyValue(QString key, QVariantList args, QString initSymbol)
{
    int index = Console::searchKeyIndex(key, Console::argsToStringList(args), initSymbol);
    if(index == -1) return "";
    QString value = args.value(index).toString().section(initSymbol,-1,-1);
    if(value.isEmpty()) return "undefined";
    return value;
}

QByteArray Console::getArgDataValue(QVariantList args, QString key, QString initSymbols)
{
    QByteArray data;
    QString value = Console::getArgKeyValue(key, args, initSymbols);
    QStringList dataValueList;
    if(value != "undefined" && !value.isEmpty()) {
        dataValueList = value.simplified().split(' ');        
        for(int i = 0; i < dataValueList.size(); i++) {
            data.append((unsigned char)dataValueList.value(i,0).toULong(0,16));
        }        
    }    
    return data;
}

void Console::registerTaskTypeAlias(QString alias, int type)
{
    m_commandTypes.insert(alias, type);
}

void Console::removeTaskType(int type)
{
    while (m_commandTypes.values().contains(type)) m_commandTypes.remove(m_commandTypes.key(type));
}

void Console::removeAlias(QString alias)
{
    m_commandTypes.remove(alias);
}

void Console::setAttribute(QString attrName, QVariant value)
{
    m_attributes.insert(attrName, value);
}
