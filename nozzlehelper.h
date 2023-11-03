#ifndef NOZZLEHELPER_H
#define NOZZLEHELPER_H
#include <QObject>
#define NOZZLE_NUM      10

typedef struct nozzle{
    uint64_t unitPrice = 0;
    uint64_t liter =0;
    uint64_t totalMoney =0;
    uint64_t disconnect = 0;
    uint64_t lostLog = 0;
    uint64_t shutdown = 0;
}nozzle;

#endif // NOZZLEHELPER_H
