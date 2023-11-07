#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QSerialPort>
#include <QTimer>
#include <nozzlehelper.h>

#define STX                         0X02
#define ETX                         0x03
#define POLLING_RXBUFFER_mS         200
typedef enum COM_STATE{
    COM_STATE_IDLE = 0,
    COM_STATE_RECEIVE_DATA_LEN = 1,
    COM_STATE_RECEIVE_DATA = 2,
    COM_STATE_DEFAULT = 0xFF
}COM_STATE;
class SerialPort : public QObject
{
    Q_OBJECT
public:
    NozzleMessage nozzleMsg;
    explicit SerialPort();
    bool connectPort(QString portName);
    qint64 writeSerialPort(QByteArray data);
    void receiveData(const QByteArray &new_data);
    uint8_t getRxDataPack();
signals:
    void insertDataToDb(NozzleMessage &data);
    void showDataReceived(QByteArray data);
private:
    QSerialPort *serialPort = nullptr;
    QByteArray rxBuffer;
    QByteArray packReady;
    uint8_t    data_len;
    uint32_t heartbeatCounter;
    QTimer heartbeatTicker;
    COM_STATE com_state;
    uint8_t   data_len_index = 0;
    QTimer pollingDataReceived;
    uint8_t pack_found;

private slots:
    void dataReady();
    void timeOut();
    void processReceivedData();
};

#endif // SERIALPORT_H
