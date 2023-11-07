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



void LogRecord::parseData(QByteArray * const p_data)
{
#if 1
    if(1){
        char *l_data = p_data->data();
        checksum = l_data[91];
        if(1){ //checksum == calcChecksum(l_data)
            sequence = l_data[2];
            id = l_data[3];
            status = l_data[4];
            if(status == 0 || status == 1){
                strncpy((char *)liter_1,(const char *)&l_data[5],8);
                strncpy((char *)unitPrice_1,(const char *)&l_data[13],6);
                strncpy((char *)money_1,(const char *)&l_data[19],8);
                strncpy((char *)liter_2,(const char *)&l_data[27],8);
                strncpy((char *)unitPrice_2,(const char *)&l_data[35],6);
                strncpy((char *)money_2,(const char *)&l_data[41],8);
                strncpy((char *)liter_3,(const char *)&l_data[49],8);
                strncpy((char *)unitPrice_3,(const char *)&l_data[57],6);
                strncpy((char *)money_3,(const char *)&l_data[63],8);
                strncpy((char *)liter_4,(const char *)&l_data[71],8);
                strncpy((char *)unitPrice_4,(const char *)&l_data[79],6);
                strncpy((char *)money_4,(const char *)&l_data[85],8);
            }else{
                if(status == 2) disconnect++;
                if(status == 3) shutdown++;
            }

        }else{
            qDebug() << "checksum error";
        }

    }
#endif
}

char LogRecord::calcChecksum(char *data)
{
    char l_result;
    l_result = data[91];
    return l_result;
}
//        QString l_data;
//        QString data_part;
//        l_data = p_data->toHex();
//        data_part = l_data.mid(0,16);
//        uint8_t a = '\0' ;
//        uint64_t c;
//        char b;
//        b = p_data->at(3);
//        a = (int)b;
//        bool ok;
//        c = data_part.toULongLong(&ok,16);

//        qDebug()<< c <<a;
//        id = p_data[1].toUInt();
//        status = p_data[2].toInt();
//        liter_1 = p_data[3].toInt();
