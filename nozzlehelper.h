#ifndef NOZZLEHELPER_H
#define NOZZLEHELPER_H
#include <QObject>
#include <QDateTime>

#define NOZZLE_NUM      10

typedef struct nozzle{
    QByteArray unitPrice;
    QByteArray liter;
    QByteArray totalMoney;
    uint64_t disconnect = 0;
    uint64_t lostLog = 0;
    uint64_t shutdown = 0;
    QString time;
}nozzle;
typedef struct NozzleData{
    uint8_t liter[8];
    uint8_t unitPrice[6];
    uint8_t totalMoney[8];
}NozzleData;
typedef struct NozzleMessage{
    uint8_t Id;
    uint8_t Status;
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
}NozzleMessage;
#endif // NOZZLEHELPER_H
