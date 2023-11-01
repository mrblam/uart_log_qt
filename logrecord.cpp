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

int LogRecord::getSeqence()
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

void LogRecord::setSequent(int data)
{
    sequence = data;
}

void LogRecord::parseData(QByteArray * const p_data)
{
#if 1
    char *l_data = p_data->data();
    checksum = l_data[91];
    if(1){ //checksum == calcChecksum(l_data)
        sequence = l_data[0];
        id = l_data[1];
        status = l_data[2];
        strncpy((char *)liter_1,(const char *)&l_data[3],8);
        strncpy((char *)unitPrice_1,(const char *)&l_data[11],6);
        strncpy((char *)money_1,(const char *)&l_data[17],8);
        strncpy((char *)liter_2,(const char *)&l_data[25],8);
        strncpy((char *)unitPrice_2,(const char *)&l_data[33],6);
        strncpy((char *)money_2,(const char *)&l_data[39],8);
        strncpy((char *)liter_3,(const char *)&l_data[47],8);
        strncpy((char *)unitPrice_3,(const char *)&l_data[55],6);
        strncpy((char *)money_3,(const char *)&l_data[61],8);
        strncpy((char *)liter_4,(const char *)&l_data[69],8);
        strncpy((char *)unitPrice_4,(const char *)&l_data[77],6);
        strncpy((char *)money_4,(const char *)&l_data[83],8);
    }else{
        qDebug() << "checksum error";
    }
#endif
}

char LogRecord::calcChecksum(char *data)
{
    char l_result;
    l_result = data[91];
    return l_result;
}
