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
        qDebug()<<"[COM1]Openning";
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
        msg485.liter = "0x00";
        msg485.unitPriceState = 0xff;
        msg485.unitPrice = "0x00";
        msg485.totalMoney = "0x00";
        msg485.no = 0xff;
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
        emit handleMsg485(msg485);
        qDebug()<< "SA:" <<msg485.SA <<"UA:"<<msg485.UA <<"opcode:"<<msg485.opcode <<"state:"<<msg485.state <<"liter:"<<msg485.liter <<"unitPriceState:"<<msg485.unitPriceState<<"unitPrice:"<<msg485.unitPrice <<"totalMoney:"<<msg485.totalMoney <<"no:"<<msg485.no;
        break;
    case 34:
    case 41:
        msg485.SA = packReady[1];
        msg485.UA = packReady[2];
        msg485.opcode = packReady.mid(3,2);
        msg485.state = packReady[5];
        msg485.liter = packReady.mid(6,8);
        msg485.unitPriceState = packReady[14];
        msg485.unitPrice =packReady.mid(15,6);
        msg485.totalMoney = packReady.mid(21,8);
        msg485.no = packReady[29];
        emit handleMsg485(msg485);
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
    qDebug()<<"[com1]rxBuffer485 ="+rxBuffer;
    if(rxBuffer.length()<=0) return (-1);
    int32_t start=rxBuffer.indexOf(STX);
    if(start < 0){
        qDebug()<< "[com1]clear rxBufer485: Not found STX";
        rxBuffer.clear();
        return(-1);
    }
    int32_t finish=rxBuffer.indexOf(ETX);
    if((finish < start) || (finish < 0)){
        qDebug()<< "[com1]remove:" +rxBuffer.left(start);
        rxBuffer.remove(0,start);
        return (-1);
    }
    int8_t msg_type = finish-start+2;
    if(msg_type >= 9){
        packReady=rxBuffer.mid(start,finish-start+2);
        qDebug()<< "[com1]packFound --> " + packReady.toHex();
        int8_t packReadyETX = packReady.indexOf(ETX);
        int8_t lastStart = packReady.lastIndexOf(STX,packReadyETX);//duplicate 0x02 in packready,except 0x03 0x02
        if(lastStart){
            packReady=packReady.mid(lastStart,packReadyETX-lastStart+2);
            msg_type = packReadyETX-lastStart+2;
        }
        qDebug()<< "[com1]packFound --> " + packReady.toHex();
    }
    rxBuffer.remove(start,finish - start + 2);
    return msg_type;
}

