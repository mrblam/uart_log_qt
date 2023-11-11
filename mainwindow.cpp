#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QTimer>
#include <QDateTime>
#include <scada.h>
#include <QDebug>
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadPort();
    connect(&_port,&SerialPort::showDataReceived,this,&MainWindow::showDataReceived);
    connect(&_port,&SerialPort::insertDataToDb,this,&MainWindow::insertDataToDb);
    connect(&_port,&SerialPort::updateNozzleData,Scada::getScada(),&Scada::updateNozzleData);
    this->setWindowTitle("Peco Log");
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
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("mydatabase.db");
        if (!db.open()) {
            qDebug() << "Cannot careate new database. Error:" << db.lastError().text();
        } else {
            qDebug() << QObject::tr("Database is open!");
            QSqlQuery query;
            query.exec("create table LogRS232 (ID TEXT,"
                       "Status TEXT,"
                       "VolumeStart TEXT,"
                       "UnitPriceStart TEXT,"
                       "CostStart TEXT,"
                       "VolumeStop TEXT,"
                       "UnitPriceStop TEXT,"
                       "CostStop TEXT,"
                       "VolumeIdle TEXT,"
                       "UnitPriceIdle TEXT,"
                       "CostIdle TEXT,"
                       "Time DATETIME)");
        }
        QSqlTableModel *model = new QSqlTableModel;
        model->setTable("LogRS232");
        model->setEditStrategy(QSqlTableModel::OnManualSubmit);
        model->select();
        ui->tableViewDb->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableViewDb->setModel(model);
        ui->tableViewDb->show();
        delete(model);
    }
}
void MainWindow::insertDataToDb(NozzleMessage &data)
{
    static uint64_t msgcounter = 0;
    msgcounter++;
    ui->msgCounter->display(QString::number(msgcounter));
    static QSqlTableModel *model_1 = new QSqlTableModel;
    QDateTime currentTime;
    QString qry_cmd = "insert into LogRS232 values(";
    QSqlQuery query;
    qry_cmd.append(QString::number(_port.nozzleMsg.Id));
    qry_cmd.append(",");
    qry_cmd.append(QString::number(_port.nozzleMsg.Status));
    qry_cmd.append(",");
    qry_cmd.append(_port.nozzleMsg.liter_2);
    qry_cmd.append(",");
    qry_cmd.append(_port.nozzleMsg.unitPrice_2);
    qry_cmd.append(",");
    qry_cmd.append(_port.nozzleMsg.money_2);
    qry_cmd.append(",");
    qry_cmd.append(_port.nozzleMsg.liter_3);
    qry_cmd.append(",");
    qry_cmd.append(_port.nozzleMsg.unitPrice_3);
    qry_cmd.append(",");
    qry_cmd.append(_port.nozzleMsg.money_3);
    qry_cmd.append(",");
    qry_cmd.append(_port.nozzleMsg.liter_4);
    qry_cmd.append(",");
    qry_cmd.append(_port.nozzleMsg.unitPrice_4);
    qry_cmd.append(",");
    qry_cmd.append(_port.nozzleMsg.money_4);
    qry_cmd.append(",");
    qry_cmd.append(":time)");
    currentTime = QDateTime::currentDateTime();
    query.prepare(qry_cmd);
    query.bindValue(":time", currentTime);
    qDebug()<< qry_cmd;
    if (!query.exec()) {
        qDebug() << "Insert failed:" << query.lastError();
    }
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
void MainWindow::on_btnScada_clicked()
{
    Scada *Scada_window = Scada::getScada();
    Scada_window->showMaximized();
}
void MainWindow::showDataReceived(QByteArray data)
{
    ui->listMessage->addItem(QString (data.toHex()));
}
void MainWindow::on_pushQuery_clicked()
{
    static QSqlTableModel *model_1 = new QSqlTableModel;
    QSqlQuery query;
    QString qry_cmd = ui->cmdSQL->text();
    query.prepare(qry_cmd);
    if (!query.exec(qry_cmd)) {
        qDebug() << "Insert failed:" << query.lastError();
    }
    model_1->setQuery(std::move(query));
    model_1->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model_1->select();
    ui->tableViewDb->setModel(model_1);
    ui->tableViewDb->show();
}
