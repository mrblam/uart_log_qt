#ifndef LOGRECORD_H
#define LOGRECORD_H

#include <QObject>

class LogRecord : public QObject
{
    Q_OBJECT
public:
    explicit LogRecord();
    QString getNameTable();
    QByteArray getSeqence();
    QByteArray getId();
    QByteArray getStatus();
    QByteArray getLiter_1();
    QByteArray getUnitPrice_1();
    QByteArray getMoney_1();
    QByteArray getLiter_2();
    QByteArray getUnitPrice_2();
    QByteArray getMoney_2();
    QByteArray getLiter_3();
    QByteArray getUnitPrice_3();
    QByteArray getMoney_3();
    QByteArray getLiter_4();
    QByteArray getUnitPrice_4();
    QByteArray getMoney_4();
    QByteArray getChecksum();
    void setSequent(QByteArray);
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
private:
    QString nameTable;
    QByteArray SoF;
    QByteArray seqence;
    QByteArray id;
    QByteArray status;
    QByteArray liter_1;
    QByteArray unitPrice_1;
    QByteArray money_1;
    QByteArray liter_2;
    QByteArray unitPrice_2;
    QByteArray money_2;
    QByteArray liter_3;
    QByteArray unitPrice_3;
    QByteArray money_3;
    QByteArray liter_4;
    QByteArray unitPrice_4;
    QByteArray money_4;
    QByteArray checksum;
    QByteArray EoF;
};

#endif // LOGRECORD_H
