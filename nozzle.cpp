#include "nozzle.h"

Nozzle::Nozzle(QObject *parent)
    : QObject{parent}
{

}

QString Nozzle::getName()
{
    return name;
}

uint8_t Nozzle::getId485()
{
    return id485;
}

uint8_t Nozzle::getStatus()
{
    return Status;
}

uint64_t Nozzle::getUnitPrice()
{
    return unitPrice;
}

uint64_t Nozzle::getLiter()
{
    return liter;
}

uint64_t Nozzle::getTotalMoney()
{
    return totalMoney;
}

uint64_t Nozzle::getDisconnect()
{
    return disconnect;
}

uint64_t Nozzle::getLostLog()
{
    return lostLog;
}

uint64_t Nozzle::getShutDown()
{
    return shutdown;
}

QString Nozzle::getTime()
{
    return time;
}

Nozzle *Nozzle::findNozzle(Nozzle* src,uint8_t Id485, uint8_t No,uint8_t nozzNum)
{
    for(int i = 0; i < nozzNum;i++){
        if(src[i].id485 == Id485 && src[i].no == No){
            return &src[i];
        }
    }
    return nullptr;
}

void Nozzle::setName(QString data)
{
    name = data;
}

void Nozzle::setId485(uint8_t id)
{
    id485 = id;
}

void Nozzle::setNo(uint8_t data)
{
    no = data;
}

void Nozzle::setStatus(uint8_t stt)
{
    Status = stt;
}

void Nozzle::setUnitPrice(uint64_t data)
{
    unitPrice = data;
}

void Nozzle::setTime(QString data)
{
    time = data;
}

void Nozzle::setLiter(uint64_t data)
{
    liter = data;
}

void Nozzle::setTotalMoney(uint64_t data)
{
    totalMoney = data;
}

void Nozzle::setDisconnect()
{
    disconnect++;
}

void Nozzle::setLostLog()
{
    lostLog++;
}

void Nozzle::setShutdown()
{
    shutdown++;
}
