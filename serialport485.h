#ifndef SERIALPORT485_H
#define SERIALPORT485_H

#include <QObject>
#include <QSerialPort>
#include <QTimer>
#include <nozzlehelper.h>

class SerialPort485 : public QObject
{
    Q_OBJECT
public:
    explicit SerialPort485(QObject *parent = nullptr);
    bool connectPort(QString portName,int baud);
    void receiveData(const QByteArray &new_data);
    int8_t getRxDataPack();
    Msg485 msg485;
signals:
     void handleMsg485(Msg485 &data);
private slots:
    void dataReady();
    void processReceivedData();
private:
    QSerialPort *serialPort = nullptr;
    QByteArray rxBuffer;
    QByteArray packReady;
    QTimer pollingDataReceived;
    int8_t pack_found;
};

#endif // SERIALPORT485_H
