#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "serialport.h"
#include "nozzle.h"
#include <QSqlTableModel>
#include "serialport485.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void closeEvent (QCloseEvent *event);
    uint8_t nozzleNum;
    Nozzle nozzleArr[32];
private slots:
    void on_pushOpen_pressed();

    void handleMsgType1();

    void handleMsgType2(NozzleMessage &data);

    void on_btnScada_clicked();

    void showDataReceived(QByteArray data);

    void on_pushQuery_clicked();

    void on_assignNozzle_clicked();

    void on_assignFinish_clicked();

    void on_pushReloadAssignNozz_clicked();

    void handleMsg485(Msg485 &data);

private:
    Ui::MainWindow *ui;
    SerialPort portRS232;
    SerialPort485 port485;
    bool loadPort();
    Nozzle *nozzlePtr;

//    LogRecord record;
signals:
    void prepareData();
};
#endif // MAINWINDOW_H
