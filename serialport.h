#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QSerialPort>

class SerialPort : public QObject
{
    Q_OBJECT
public:
    SerialPort();
    bool connectPort(QString portName);
    qint64 writeSerialPort(QByteArray data);
signals:
    void dataReceived(QByteArray b);
private:
    QSerialPort *serialPort = nullptr;
private slots:
    void dataReady();
};

#endif // SERIALPORT_H
