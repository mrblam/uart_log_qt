#include "logrecord.h"
#include <QDebug>
#include <QDataStream>


LogRecord::LogRecord()
{
    setSequent(23);
}

QString LogRecord::getNameTable()
{
    return nameTable;
}

LogRecord *LogRecord::getRecord()
{
    static LogRecord* self;
    if(self==nullptr){
        self=new LogRecord();
    }
    return self;
}

char LogRecord::getSeqence()
{
    return sequence;
}

char LogRecord::getId()
{
    return id;
}

char LogRecord::getStatus()
{
    return status;
}

char* LogRecord::getLiter_1()
{
    return liter_1;
}

char* LogRecord::getUnitPrice_1()
{
    return unitPrice_1;
}

char* LogRecord::getMoney_1()
{
    return money_1;
}

char* LogRecord::getLiter_2()
{
    return liter_2;
}

char* LogRecord::getUnitPrice_2()
{
    return unitPrice_2;
}

char* LogRecord::getMoney_2()
{
    return money_2;
}

char* LogRecord::getLiter_3()
{
    return liter_3;
}

char* LogRecord::getUnitPrice_3()
{
    return unitPrice_3;
}

char* LogRecord::getMoney_3()
{
    return money_3;
}

char* LogRecord::getLiter_4()
{
    return liter_4;
}

char* LogRecord::getUnitPrice_4()
{
    return unitPrice_4;
}

char* LogRecord::getMoney_4()
{
    return money_4;
}

char LogRecord::getChecksum()
{
    return 1;
}

void LogRecord::setSequent(int)
{

}

char LogRecord::calcChecksum(char *data)
{
    char l_result;
    l_result = data[91];
    return l_result;
}
