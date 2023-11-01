#ifndef LOGRECORD_H
#define LOGRECORD_H

#include <QObject>

class LogRecord : public QObject
{
    Q_OBJECT
public:
    explicit LogRecord();
    QString getNameTable();
    int getSeqence();
    char getId();
    char getStatus();
    char* getLiter_1();
    char* getUnitPrice_1();
    char* getMoney_1();
    char* getLiter_2();
    char* getUnitPrice_2();
    char* getMoney_2();
    char* getLiter_3();
    char* getUnitPrice_3();
    char* getMoney_3();
    char* getLiter_4();
    char* getUnitPrice_4();
    char* getMoney_4();
    char getChecksum();

    void setSequent(int);
    void setId(QByteArray);
    void setStatus(QByteArray);
    void setLiter_1(QByteArray);
    void setUnitPrice_1(QByteArray);
    void setMoney_1(QByteArray);
    void setLiter_2(QByteArray);
    void setUnitPrice_2(QByteArray);
    void setMoney_2(QByteArray);
    void setLiter_3(QByteArray);
    void setUnitPrice_3(QByteArray);
    void setMoney_3(QByteArray);
    void setLiter_4(QByteArray);
    void setUnitPrice_4(QByteArray);
    void setMoney_4(QByteArray);
    void setChecksum(QByteArray);

    void parseData(QByteArray* const p_data );
    char calcChecksum(char*);
private:
    QString nameTable;
    char SoF;
    char sequence;
    char id;
    char status;
    char liter_1[8];
    char unitPrice_1[6];
    char money_1[8];
    char liter_2[8];
    char unitPrice_2[6];
    char money_2[8];
    char liter_3[8];
    char unitPrice_3[6];
    char money_3[8];
    char liter_4[8];
    char unitPrice_4[6];
    char money_4[8];
    char checksum;
    char EoF;
};

#endif // LOGRECORD_H
