#include "serialport.h"
#include <QDebug>

static uint8_t checksumXOR(uint8_t *pu8Buffer, uint32_t u32Offset, uint32_t u32Length);
static QByteArray convertDataReceived(QByteArray &datau,uint8_t len);
SerialPort::SerialPort()
{

}

bool SerialPort::connectPort(QString portName)
{
    if(serialPort != nullptr){
        serialPort->close();
        delete serialPort;
    }
    serialPort = new QSerialPort;
    serialPort->setPortName(portName);
    serialPort->setBaudRate(QSerialPort::BaudRate::Baud115200);
    serialPort->setParity(QSerialPort::Parity::NoParity);
    serialPort->setDataBits(QSerialPort::DataBits::Data8);
    serialPort->setStopBits(QSerialPort::OneStop);
    com_state = COM_STATE_DEFAULT;
    connect(&heartbeatTicker,&QTimer::timeout,this,&SerialPort::timeOut);
    if(serialPort->open(QIODevice::ReadWrite)){
        connect(serialPort,&QSerialPort::readyRead,this,&SerialPort::dataReady);
        connect(&pollingDataReceived,&QTimer::timeout,this,&SerialPort::processReceivedData);
        qDebug()<<"openning";
        pollingDataReceived.start(POLLING_RXBUFFER_mS);
    }
    return serialPort->isOpen();
}

qint64 SerialPort::writeSerialPort(QByteArray data)
{
    if(serialPort == nullptr || !serialPort->isOpen()){
        return -1;
    }
    return serialPort->write(data);
}

void SerialPort::receiveData(const QByteArray &new_data)
{
    emit showDataReceived(new_data); ///show all data receive on terminal
    rxBuffer.append(new_data);
}
uint8_t SerialPort::getRxDataPack()
{
    if(rxBuffer.length()<=0) return (-1);
    int32_t start=rxBuffer.indexOf(STX);
    if(start < 0) return(-1);
    int32_t finish=rxBuffer.indexOf(ETX);
    if(finish < 0) return(-1);
    if((finish < start)){
        rxBuffer.remove(0,start-1);
        return (-1);
    }
    uint8_t msg_type = finish-start;
    packReady=rxBuffer.mid(start+1,finish-start-1);
    qDebug()<< "packready --> " + packReady.toHex();
    packReady = convertDataReceived(packReady,finish-start-1);
    qDebug()<< "packconvert --> " + packReady.toHex();
    rxBuffer.remove(start,finish - start + 1);
    return msg_type;
}
void SerialPort::dataReady()
{
    if(serialPort->bytesAvailable() > 0){
        receiveData(serialPort->readAll());
    }
}
void SerialPort::timeOut()
{
    com_state = COM_STATE_DEFAULT;
    heartbeatTicker.stop();
}
void SerialPort::processReceivedData()
{
    QByteArray l_buffer;
    pack_found=getRxDataPack();//polling
    if(pack_found<=0) return;

    switch (pack_found) {
    case 5:
        nozzleMsg.Id            = packReady[0];
        nozzleMsg.Status        = packReady[1];
        nozzleMsg.liter_1       = "0xB9";
        nozzleMsg.unitPrice_1   = "0xB9";
        nozzleMsg.money_1       = "0xB9";
        nozzleMsg.liter_2       = "0xB9";
        nozzleMsg.unitPrice_2   = "0xB9";
        nozzleMsg.money_2       = "0xB9";
        nozzleMsg.liter_3       = "0xB9";
        nozzleMsg.unitPrice_3   = "0xB9";
        nozzleMsg.money_3       = "0xB9";
        nozzleMsg.liter_4       = "0xB9";
        nozzleMsg.unitPrice_4   = "0xB9";
        nozzleMsg.money_4       = "0xB9";
        emit updateNozzleData(nozzleMsg);
        emit insertDataToDb(nozzleMsg);
        break;
    case 92:
        nozzleMsg.Id            = packReady[0];
        nozzleMsg.Status        = packReady[1];
        nozzleMsg.liter_1       = packReady.mid(2,8);
        nozzleMsg.unitPrice_1   = packReady.mid(10,6);
        nozzleMsg.money_1       = packReady.mid(16,8);
        nozzleMsg.liter_2       = packReady.mid(24,8);
        nozzleMsg.unitPrice_2   = packReady.mid(32,6);
        nozzleMsg.money_2       = packReady.mid(38,8);
        nozzleMsg.liter_3       = packReady.mid(46,8);
        nozzleMsg.unitPrice_3   = packReady.mid(54,6);
        nozzleMsg.money_3       = packReady.mid(60,8);
        nozzleMsg.liter_4       = packReady.mid(68,8);
        nozzleMsg.unitPrice_4   = packReady.mid(76,6);
        nozzleMsg.money_4       = packReady.mid(82,8);
        emit updateNozzleData(nozzleMsg);
        emit insertDataToDb(nozzleMsg);
        break;
    default:
        return;
    }
}
uint8_t checksumXOR(uint8_t *pu8Buffer, uint32_t u32Offset, uint32_t u32Length){
    uint8_t bCRC = 0x00;
    for (u32Offset = 0; u32Offset <= u32Length; u32Offset++) {
        bCRC ^= pu8Buffer[u32Offset];
    }
    return bCRC;
}
static QByteArray convertDataReceived(QByteArray &data,uint8_t len)
{
    for(uint8_t i = 0;i < len;i++){
        data[i] = data[i] & 0x7F;
    }
    return data;
}
//daht-phatsinh-hoanthanhmucdolab-trienkhaiduocodau
//    if(*new_data == STX){
//        com_state = COM_STATE_IDLE;
//    }
//    switch(com_state){
//    case COM_STATE_IDLE:
//        com_state = COM_STATE_RECEIVE_DATA_LEN;
//        break;
//    case COM_STATE_RECEIVE_DATA_LEN:
//        com_state = COM_STATE_RECEIVE_DATA;
//        data_len = *new_data;
//        heartbeatTicker.start(10000);
//        break;
//    case COM_STATE_RECEIVE_DATA:
//        data_len_index ++;
//        if(data_len_index == data_len){
//            data_len_index = 0;
//            emit dataReceived(rxBuffer);
//            com_state = COM_STATE_IDLE;
//            rxBuffer.clear();
//        }
//        heartbeatTicker.start(10000);
//        break;
//    case COM_STATE_DEFAULT:
//        break;
//    default:
//        break;
//    }
