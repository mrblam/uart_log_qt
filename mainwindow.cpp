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
#include <QCloseEvent>

static bool s_ErrMissLog = false;
static bool s_ErrDisconnect = false;
static bool s_ErrStartup = false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->btnScada->setEnabled(false);
    ui->pushQuery->setEnabled(false);
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
        ui->portList->setEnabled(false);
        ui->statusPort->setText("Opened");
        ui->btnScada->setEnabled(true);
        ui->pushQuery->setEnabled(true);
        ui->statusPort->setStyleSheet("color: blue;");
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("mydatabase.db");
        if (!db.open()) {
            qDebug() << "Cannot careate new database. Error:" << db.lastError().text();
        } else {
            qDebug() << QObject::tr("Database is open!");
            QSqlQuery query;
            query.exec("create table LogRS232 (Vòi INT,"
                       "[Trạng thái] TEXT,"
                       "[Lượng lít(bắt đầu)] TEXT,"
                       "[Đơn giá(bắt đầu)] TEXT,"
                       "[Thành tiền(bắt đầu)] TEXT,"
                       "[Lượng lít(kết thúc)] TEXT,"
                       "[Đơn giá(kết thúc)] TEXT,"
                       "[Thành tiền(kết thúc)] TEXT,"
                       "[Lượng lít(gác cò)] TEXT,"
                       "[Đơn giá(gác cò)] TEXT,"
                       "[Thành tiền(gác cò)] TEXT,"
                       "[Thời gian] DATETIME)");
            query.exec("create table err_log (ID INT,MissLog TEXT,Disconnect TEXT,Startup TEXT,Time DATETIME)");
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
    query.bindValue(":time", currentTime.toString("dd/MM/yyyy hh:mm:ss"));
    qDebug()<< qry_cmd;
    if (!query.exec()) {
        qDebug() << "Insert failed:" << query.lastError();
    }
    /*Insert table error begin*/
    QString qry_cmd_table2 = "insert into err_log values(";
    qry_cmd_table2.append(QString::number(_port.nozzleMsg.Id));
    qry_cmd_table2.append(",");
    switch (_port.nozzleMsg.Status){
    case 0:
        break;
    case 1:
        s_ErrMissLog = true;
        break;
    case 2:
        s_ErrDisconnect = true;
        break;
    case 3:
        s_ErrStartup = true;
        break;
    default:
        break;
    }
    qry_cmd_table2.append(QString::number(s_ErrMissLog));
    qry_cmd_table2.append(",");
    qry_cmd_table2.append(QString::number(s_ErrDisconnect));
    qry_cmd_table2.append(",");
    qry_cmd_table2.append(QString::number(s_ErrStartup));
    qry_cmd_table2.append(",");
    qry_cmd_table2.append(":time)");
    currentTime = QDateTime::currentDateTime();
    query.prepare(qry_cmd_table2);
    query.bindValue(":time", currentTime.toString("dd/MM/yyyy hh:mm:ss"));
    if (!query.exec()) {
        qDebug() << "Insert Err_log failed:" << query.lastError();
    }
    s_ErrMissLog = false;
    s_ErrDisconnect = false;
    s_ErrStartup = false;
    /*Insert table error end*/
    model_1->setTable("LogRS232");
    model_1->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model_1->select();
    ui->tableViewDb->setModel(model_1);
    ui->tableViewDb->resizeColumnsToContents();
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
    Filter *Filter_window = Filter::getFilter();
    Filter_window->showMaximized();
}

void MainWindow::closeEvent (QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Peco Log",
                                                               tr("Are you sure?\n"),
                                                               QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                               QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
        QApplication::closeAllWindows();
        event->accept();
    }
}
