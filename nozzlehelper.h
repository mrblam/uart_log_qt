#ifndef NOZZLEHELPER_H
#define NOZZLEHELPER_H
#include <QObject>
#include <QDateTime>

#define NOZZLE_NUM      40

typedef struct nozzle{
    QString name;
    uint8_t id485;
    uint8_t no;
    uint64_t unitPrice;
    uint64_t liter;
    uint64_t totalMoney;
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
typedef enum Status{
    NORMAL,
    MISSLOG,
    DISCONNECT,
    STARTUP
} Status;
typedef struct NozzleMessage{
    uint8_t Id;
    uint8_t Status;

    QByteArray liter_now;
    QByteArray unitPrice_now;
    QByteArray money_now;

    QByteArray liter_begin;
    QByteArray unitPrice_begin;
    QByteArray money_begin;

    QByteArray liter_finish;
    QByteArray unitPrice_finish;
    QByteArray money_finish;

    QByteArray liter_idle;
    QByteArray unitPrice_idle;
    QByteArray money_idle;
}NozzleMessage;
#endif // NOZZLEHELPER_H
