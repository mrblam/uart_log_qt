#include "nozzle.h"

Nozzle::Nozzle(QObject *parent)
    : QObject{parent}
{

}

Nozzle *Nozzle::findNozzle(Nozzle* src,uint8_t Id485, uint8_t No)
{
    for(int i = 0; i < 32;i++){
        if(src[i].id485 == Id485 && src[i].no == No){
            return &src[i];
        }
    }
    return nullptr;
}
