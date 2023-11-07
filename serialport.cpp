#include "serialport.h"
#include <QDebug>

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
    int32_t finish=rxBuffer.indexOf(ETX);
    if(finish < start){
        rxBuffer.remove(0,start);
        return (-1);
    }
    QByteArray pack=rxBuffer.mid(start+1,finish-start-1);
    qDebug()<< "--> " + pack;
    rxBuffer.remove(start,finish - start + 1);

//    QList<QByteArray> fields=pack.at()

//    if(fields.at(0).at(0) != cmdHeader.cmdCode||(fields.at(1).at(0) != cmdHeader.objMux.code) ||
//        (fields.at(2) !=cmdHeader.objMux.index) ||
//        (fields.at(3).at(0) !=cmdHeader.objMux.sub)){
//        return -1;
//    }

//    if(cmdHeader.cmdCode==CMD_WRITE){
//        if(fields.at(4).at(0)!='O'){
//            setProtocolFailState(COM_ERR_WRITE);
//            return (0);
//        }

//        switch (cmdHeader.objMux.code) {
//        case BSS_DATA_CODE:
//            if(cmdHeader.objMux.sub==BSS_STATE_DATA_CODE){
//                localBss->setState(remoteBSS.getState());
//                break;
//            }
//            if(cmdHeader.objMux.sub==BSS_ID_ASSIGN_CODE){
//                localCabinet->setOpState(remoteCabinet.getOpState());
//                break;
//            }
//            if(cmdHeader.objMux.sub==BSS_AUTH_CODE){
//                localCabinet->setOpState(remoteCabinet.getOpState());
//                break;
//            }
//            break;
//        case CAB_DATA_CODE:
//            if(cmdHeader.objMux.sub==CAB_OP_CODE){
//                localCabinet->setOpState(remoteCabinet.getOpState());
//                break;
//            }
//            if(cmdHeader.objMux.sub==CAB_DOOR_CODE){
//                localCabinet->setDoorState(remoteCabinet.getDoorState());
//                break;
//            }
//            break;
//        case BP_DATA_CODE:
//            break;
//        default:
//            setProtocolFailState(COM_ERR_CMD_NO_SUPPORT);
//            return (0);
//        }
//        return (0);
//    }
    return 0;
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
    pack_found=getRxDataPack();
    if(pack_found<=0) return;

    switch (pack_found) {
    case 1:

        break;
    case 2:

        break;
    default:
        return;
    }
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
