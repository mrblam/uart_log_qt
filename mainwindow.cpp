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
#include <nozzlehelper.h>
#include "filter.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    for(int i = 0;i < NOZZLE_NUM;i++){
        ui->nozzleID->addItem(QString::number(i));
    }
    ui->nozzleID->addItem("*");
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
    query.bindValue(":time", currentTime.toString());
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
    static QSqlTableModel *model_2 = new QSqlTableModel;
    QSqlQuery query1;
    QSqlQuery query2;
    QString qry_cmd2 ;//= ui->cmdSQL->text()
    QString qry_cmd;
    qry_cmd2.append("SELECT * FROM LogRS232 where Time between '");
    qry_cmd2.append(ui->dateTimeBegin->dateTime().toString());
    qry_cmd2.append("' and '");
    qry_cmd2.append(ui->dateTimeFinish->dateTime().toString());
    qry_cmd2.append("'");
    //
    qry_cmd.append("select ID,Status,Count(*) as SoLan from LogRS232 where Time between '");
    qry_cmd.append(ui->dateTimeBegin->dateTime().toString());
    qry_cmd.append("' and '");
    qry_cmd.append(ui->dateTimeFinish->dateTime().toString());
    qry_cmd.append("'");
    //
    QString aa = ui->nozzleID->currentText();
    if(aa == "*"){
        qry_cmd.append(" group by ID,Status");
    }else{
        qry_cmd.append(" and ID = ");
        qry_cmd.append(ui->nozzleID->currentText());
        qry_cmd.append(" group by Status");
        qry_cmd2.append(" and ID = ");
        qry_cmd2.append(ui->nozzleID->currentText());
    }
    query1.prepare(qry_cmd);
    if (!query1.exec(qry_cmd)) {
        qDebug() << "1Insert failed:" << query1.lastError();
    }
    model_1->setQuery(std::move(query1));
    model_1->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model_1->select();
    ui->tableViewfilter->setModel(model_1);
    ui->tableViewfilter->show();
    //SELECT ID,Status,VolumeIdle,UnitPriceIdle,CostIdle FROM LogRS232 where Time between 'Mon Nov 13 00:03:25 2023' and 'Mon Nov 13 15:16:11 2023' and ID = 1 ORDER BY Time DESC
//    LIMIT 1

    query2.prepare(qry_cmd2);
    if (!query2.exec(qry_cmd2)) {
        qDebug() << "2Insert failed:" << query2.lastError();
    }
    model_2->setQuery(std::move(query2));
    model_2->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model_2->select();
    ui->tableViewNewest->setModel(model_2);
    ui->tableViewNewest->show();
    Filter *Filter_window = Filter::getFilter();
    Filter_window->showMaximized();
}
