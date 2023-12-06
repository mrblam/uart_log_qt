#include "serialport485.h"
#include "serialport.h"
#include <QDebug>

SerialPort485::SerialPort485(QObject *parent)
    : QObject{parent}
{

}

bool SerialPort485::connectPort(QString portName, int baud)
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
    if(serialPort->open(QIODevice::ReadWrite)){
        connect(serialPort,&QSerialPort::readyRead,this,&SerialPort485::dataReady);
        connect(&pollingDataReceived,&QTimer::timeout,this,&SerialPort485::processReceivedData);
        qDebug()<<"Openning";
        pollingDataReceived.start(50);
    }
    return serialPort->isOpen();
}

void SerialPort485::dataReady()
{
    receiveData(serialPort->readAll());
}

void SerialPort485::processReceivedData()
{
    pack_found= getRxDataPack();
    switch(pack_found){
    case 9:
        msg485.SA = packReady[1];
        msg485.UA = packReady[2];
        msg485.opcode = packReady.mid(3,2);
        msg485.state = packReady[5];
        emit handleMsg485(msg485);
        qDebug()<< "SA:" <<msg485.SA <<"UA:"<<msg485.UA <<"opcode:"<<msg485.opcode <<"state:"<<msg485.state;
        break;
    case 28:
        msg485.SA = packReady[1];
        msg485.UA = packReady[2];
        msg485.opcode = packReady.mid(3,2);
        msg485.state = packReady[5];
        msg485.liter = packReady.mid(6,6);
        msg485.unitPriceState = packReady[12];
        msg485.unitPrice =packReady.mid(13,4);
        msg485.totalMoney = packReady.mid(17,6);
        msg485.no = packReady[23];
        qDebug()<< "SA:" <<msg485.SA <<"UA:"<<msg485.UA <<"opcode:"<<msg485.opcode <<"state:"<<msg485.state <<"liter:"<<msg485.liter <<"unitPriceState:"<<msg485.unitPriceState<<"unitPrice:"<<msg485.unitPrice <<"totalMoney:"<<msg485.totalMoney <<"no:"<<msg485.no;
        break;
    case 34:
        msg485.SA = packReady[1];
        msg485.UA = packReady[2];
        msg485.opcode = packReady.mid(3,2);
        msg485.state = packReady[5];
        msg485.liter = packReady.mid(6,8);
        msg485.unitPriceState = packReady[14];
        msg485.unitPrice =packReady.mid(15,6);
        msg485.totalMoney = packReady.mid(21,8);
        msg485.no = packReady[29];
        qDebug()<< "SA:" <<msg485.SA <<"UA:"<<msg485.UA <<"opcode:"<<msg485.opcode <<"state:"<<msg485.state <<"liter:"<<msg485.liter <<"unitPriceState:"<<msg485.unitPriceState<<"unitPrice:"<<msg485.unitPrice <<"totalMoney:"<<msg485.totalMoney <<"no:"<<msg485.no;
        break;
    default:
        break;
    }
}

void SerialPort485::receiveData(const QByteArray &new_data)
{
    rxBuffer.append(new_data);
}

int8_t SerialPort485::getRxDataPack()
{
    qDebug()<<"rxBuffer485 ="+rxBuffer;
    if(rxBuffer.length()<=0) return (-1);
    int32_t start=rxBuffer.indexOf(STX);
    if(start < 0) return(-1);
    int32_t finish=rxBuffer.indexOf(ETX);
    if((finish < start) || (finish < 0)){
        qDebug()<< "remove:" +rxBuffer.left(start);
        rxBuffer.remove(0,start);
        return (-1);
    }
    int8_t msg_type = finish-start+2;
    if(msg_type >= 9){
        packReady=rxBuffer.mid(start,finish-start+2);
        if(packReady.lastIndexOf(STX)){
            int8_t lastStart = packReady.lastIndexOf(STX);
            packReady=rxBuffer.mid(lastStart,finish-lastStart+2);
            msg_type = finish-lastStart+2;
        }
        qDebug()<< "packFound --> " + packReady.toHex();
    }
    rxBuffer.remove(start,finish - start + 2);
    return msg_type;
}

