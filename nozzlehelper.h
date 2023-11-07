#ifndef NOZZLEHELPER_H
#define NOZZLEHELPER_H
#include <QObject>
#include <QDateTime>

#define NOZZLE_NUM      10

typedef struct nozzle{
    uint64_t unitPrice = 0;
    uint64_t liter =0;
    uint64_t totalMoney =0;
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
    NozzleData nozzleDataCurrent;
    NozzleData nozzleDataStart;
    NozzleData nozzleDataStop;
    NozzleData nozzleDataIdle;
}NozzleMessage;
#endif // NOZZLEHELPER_H
