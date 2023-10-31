#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadPort();
    connect(&_port,&SerialPort::dataReceived,this,&MainWindow::readDataSerial);
}
MainWindow::~MainWindow()
{
    delete ui;
}
bool MainWindow::loadPort()
{
    foreach(auto &port,QSerialPortInfo::availablePorts()){
        ui->portList->addItem( port.portName());
    }
    return 1;
}
void MainWindow::on_pushOpen_pressed()
{
    auto isConnected = _port.connectPort(ui->portList->currentText());
    if(!isConnected){
        QMessageBox::critical(this,"Error","There is a problem connect to port");
    }else{
        QMessageBox::information(this,"Result","Port open");
        ui->pushOpen->setEnabled(false);
        //*//
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("mydatabase.db");
        if (!db.open()) {
            qDebug() << "Cannot careate new database. Lỗi:" << db.lastError().text();
        } else {
            qDebug() << QObject::tr("Cơ sở dữ liệu đã được mở!");
            QSqlQuery query;
            query.exec("create table LogRS232 (Sequent INT,"
                       "ID INT,"
                       "Status INT,"
                       "Liter1 INT,"
                       "UnitPrice1 INT,"
                       "MoneyTotal1 INT,"
                       "Liter2 INT,"
                       "UnitPrice2 INT,"
                       "MoneyTotal2 INT,"
                       "Liter3 INT,"
                       "UnitPrice3 INT,"
                       "MoneyTotal3 INT,"
                       "Liter4 INT,"
                       "UnitPrice4 INT,"
                       "MoneyTotal4 INT,"
                       "End INT)");
        }
        QSqlTableModel *model = new QSqlTableModel;
        model->setTable("LogRS232");
        model->setEditStrategy(QSqlTableModel::OnManualSubmit);
        model->select();
        ui->tableViewDb->setModel(model);
        ui->tableViewDb->show();
        delete(model);
        //*//
    }
}
void MainWindow::readDataSerial(QByteArray data)
{
    ui->listMessage->addItem(QString (data));
    QString qry_cmd;
    QSqlQuery query;
    qry_cmd.append("insert into LogRS232 values(105,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5)");
    query.exec(qry_cmd);
    static QSqlTableModel *model_1 = new QSqlTableModel;
    model_1->setTable("LogRS232");
    model_1->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model_1->select();
    ui->tableViewDb->setModel(model_1);
    ui->tableViewDb->show();
}
void MainWindow::on_btnSend_clicked()
{
    auto numBytes = _port.writeSerialPort(ui->lnMessage->text().toUtf8());
    if (numBytes == -1) {
        QMessageBox::critical(this,"Error","Something went wrong!!");
    }
}

