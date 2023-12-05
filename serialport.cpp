#include "serialport.h"
#include <QDebug>

static uint8_t checksumXOR(uint8_t *pu8Buffer, uint32_t u32Offset, uint32_t u32Length);
static QByteArray convertDataReceived(QByteArray &data,uint8_t len);
SerialPort::SerialPort():pack_found(0)
{

}

bool SerialPort::connectPort(QString portName,int baud)
{
    if(serialPort != nullptr){
        serialPort->close();
        delete serialPort;
    }
    serialPort = new QSerialPort;
    serialPort->setPortName(portName);
    serialPort->setBaudRate(baud);
    serialPort->setParity(QSerialPort::Parity::NoParity);
    serialPort->setDataBits(QSerialPort::DataBits::Data8);
    serialPort->setStopBits(QSerialPort::OneStop);
    com_state = COM_STATE_DEFAULT;
    connect(&heartbeatTicker,&QTimer::timeout,this,&SerialPort::timeOut);
    if(serialPort->open(QIODevice::ReadWrite)){
        connect(serialPort,&QSerialPort::readyRead,this,&SerialPort::dataReady);
        connect(&pollingDataReceived,&QTimer::timeout,this,&SerialPort::processReceivedData);
        qDebug()<<"Openning";
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
int8_t SerialPort::getRxDataPack()
{
    qDebug()<<"rxBuffer="+rxBuffer;
    if(rxBuffer.length()<=0) return (-1);
    int32_t start=rxBuffer.indexOf(STX);
    if(start < 0) return(-1);
    int32_t finish=rxBuffer.indexOf(ETX);
    if((finish < start) || (finish < 0)){
        qDebug()<< "remove:" +rxBuffer.left(start);
        rxBuffer.remove(0,start);
        return (-1);
    }
    int8_t msg_type = finish-start;
    if(msg_type > 3){
        packReady=rxBuffer.mid(start+1,finish-start-1);
        qDebug()<< "packFound --> " + packReady.toHex();
        uint8_t checksum1 = checksumXOR((uint8_t *)&rxBuffer[start],0,msg_type-1)|0x80;
        uint8_t checksum2 = rxBuffer.at(finish-1);
        checksum1 ^= checksum2;
        if(checksum1){
            qDebug() << "Checksum Error";
            rxBuffer.remove(start,finish - start + 1);
            return(-1);
        }
        packReady = convertDataReceived(packReady,finish-start-1);
        qDebug()<< "packConvert --> " + packReady.toHex();
    }
    rxBuffer.remove(start,finish - start + 1);
    return msg_type;
}
void SerialPort::dataReady()
{
    timeoutSetDisconnect = 0;
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
    static bool isDisconnect = false;
    static bool isConnect = false;
    timeoutSetDisconnect ++;
    if(timeoutSetDisconnect > TIMEOUT_TO_RECEIVE_DATA && isDisconnect == false){
        isDisconnect =true;
        isConnect = false;
        emit disconnectToMCU();
        timeoutSetDisconnect = 0;
    }
    pack_found=getRxDataPack();//polling 200ms
    if(pack_found<=0) return;//is new msg
    isDisconnect = false;
    if(isConnect == false){
        isConnect = true;
        emit updateState(nozzleMsg);
    }
    switch (pack_found) {
    case MSG_TYPE_2:
        nozzleMsg.Id485             = packReady[0];
        nozzleMsg.No                = 0xFF;
        nozzleMsg.Status            = packReady[1];
        nozzleMsg.liter_now         = "0x00";
        nozzleMsg.unitPrice_now     = "0x00";
        nozzleMsg.money_now         = "0x00";
        nozzleMsg.liter_begin       = "0x00";
        nozzleMsg.unitPrice_begin   = "0x00";
        nozzleMsg.money_begin       = "0x00";
        nozzleMsg.liter_finish      = "0x00";
        nozzleMsg.unitPrice_finish  = "0x00";
        nozzleMsg.money_finish      = "0x00";
        nozzleMsg.liter_idle        = "0x00";
        nozzleMsg.unitPrice_idle    = "0x00";
        nozzleMsg.money_idle        = "0x00";
        emit handleMsgType2(nozzleMsg);//mainWindow
        break;
    case MSG_TYPE_1:
        nozzleMsg.Id485             = packReady[0];
        nozzleMsg.No                = packReady[1];
        nozzleMsg.Status            = packReady[2];
        nozzleMsg.liter_begin       = packReady.mid(3,8);
        nozzleMsg.unitPrice_begin   = packReady.mid(11,6);
        nozzleMsg.money_begin       = packReady.mid(17,8);
        nozzleMsg.liter_finish      = packReady.mid(25,8);
        nozzleMsg.unitPrice_finish  = packReady.mid(33,6);
        nozzleMsg.money_finish      = packReady.mid(39,8);
        nozzleMsg.liter_idle        = packReady.mid(47,8);
        nozzleMsg.unitPrice_idle    = packReady.mid(55,6);
        nozzleMsg.money_idle        = packReady.mid(61,8);
        nozzleMsg.liter_now         = packReady.mid(69,8);
        nozzleMsg.unitPrice_now     = packReady.mid(77,6);
        nozzleMsg.money_now         = packReady.mid(83,8);
        emit handleMsgType1(nozzleMsg); //mainWindow
        break;
    default:    
        return;
    }
}
uint8_t checksumXOR(uint8_t *pu8Buffer, uint32_t u32Offset, uint32_t u32Length){
    uint8_t bCRC = 0x00;
    for (u32Offset = 0; u32Offset < u32Length; u32Offset++) {
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
