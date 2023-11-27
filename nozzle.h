#ifndef NOZZLE_H
#define NOZZLE_H

#include <QObject>

class Nozzle : public QObject
{
    Q_OBJECT
public:
    explicit Nozzle(QObject *parent = nullptr);
    QString getName();
    uint8_t getId485();
    uint8_t getNo();
    uint64_t getUnitPrice();
    uint64_t getLiter();
    uint64_t getTotalMoney();
    uint64_t getDisconnect();
    uint64_t getLostLog();
    uint64_t getShutDown();
    QString getTime();
    Nozzle *findNozzle(Nozzle* src,uint8_t Id485,uint8_t No);
    void setName(QString name);
    void setId485(uint8_t id);
    void setNo(uint8_t no);
    void setUnitPrice(uint64_t data);
    void setTime(QString data);
    void setLiter(uint64_t data);
    void setTotalMoney(uint64_t data);
    void setDisconnect();
    void setLostLog();
    void setShutdown();
private:
    QString name;
    uint8_t id485;
    uint8_t no;
    uint64_t unitPrice = 0;
    uint64_t liter = 0;
    uint64_t totalMoney = 0;
    uint64_t disconnect = 0;
    uint64_t lostLog = 0;
    uint64_t shutdown = 0;
    QString time;
};

#endif // NOZZLE_H
