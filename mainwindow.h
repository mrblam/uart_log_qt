#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "serialport.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushOpen_pressed();

    void readDataSerial(QByteArray data);

    void on_btnSend_clicked();

private:
    Ui::MainWindow *ui;
    SerialPort _port;
    bool loadPort();
};
#endif // MAINWINDOW_H
