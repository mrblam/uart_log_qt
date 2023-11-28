#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTimer>
#include <QDateTime>
#include <scada.h>
#include <QDebug>
#include <QString>
#include <nozzlehelper.h>
#include "filter.h"
#include <QCloseEvent>
#include <QFile>

static bool s_ErrMissLog = false;
static bool s_ErrDisconnect = false;
static bool s_ErrStartup = false;
static uint64_t msgCounter = 0;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->btnScada->setEnabled(false);
    ui->pushQuery->setEnabled(false);
    ui->pushOpen->setEnabled(false);
    loadPort();
    connect(&_port,&SerialPort::showDataReceived,this,&MainWindow::showDataReceived);
    connect(&_port,&SerialPort::handleMsgType1,this,&MainWindow::handleMsgType1);
    connect(&_port,&SerialPort::handleMsgType2,this,&MainWindow::handleMsgType2);
    connect(&_port,&SerialPort::updateState,Scada::getScada(),&Scada::updateNozzleData);
    connect(&_port,&SerialPort::disconnectToMCU,Scada::getScada(),&Scada::setDisconnectToMCU);
    this->setWindowTitle("Peco Log");
    this->setWindowIcon(QIcon(":/UI/Icon/p.ico"));
    nozzleNum = 0;
    nozzlePtr = nozzleArr;
    /**/
    QDateTime currentTime;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
//    bool dbExists = QFile::exists("mydatabase.db");
//    if(!dbExists){
    db.setDatabaseName("mydatabase.db");
//    }
    if (!db.open()) {
        qDebug() << "Cannot careate new database. Error:" << db.lastError().text();
    } else {
        qDebug() << QObject::tr("Database is open!");
        QSqlQuery query;
        query.exec("create table LogRS232 (Vòi TEXT,"
                   "Id485 TEXT,"
                   "No TEXT,"
                   "[Trạng thái] TEXT,"
                   "[Lượng lít(bắt đầu)] TEXT,"
                   "[Đơn giá(bắt đầu)] TEXT,"
                   "[Thành tiền(bắt đầu)] TEXT,"
                   "[Lượng lít(kết thúc)] TEXT,"
                   "[Đơn giá(kết thúc)] TEXT,"
                   "[Thành tiền(kết thúc)] TEXT,"
                   "[Thời gian] DATETIME)");
        query.exec("create table err_log (Vòi TEXT,MissLog TEXT,Disconnect TEXT,Startup TEXT,Time DATETIME)");
        query.exec("create table Nozzle_Assign (Vòi TEXT,ID485 INT,No INT)");
        query.exec("create table Log_State (Time DATETIME,State TEXT)");

        currentTime = QDateTime::currentDateTime();
        query.prepare("insert into Log_State values (:time,'Khởi động')");
        query.bindValue(":time", currentTime.toString("dd/MM/yyyy hh:mm:ss"));
        if (!query.exec()) {
            qDebug() << "Insert Log_State failed:" << query.lastError();
        }
    }
    /**/
    QSqlTableModel *model11 = new QSqlTableModel;
    model11->setTable("Nozzle_Assign");
    model11->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model11->select();
    ui->assignResult->setModel(model11);
    ui->assignResult->resizeColumnsToContents();
    ui->assignResult->show();
    /**/
    MainWindow::on_pushOpen_pressed();
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
    auto isConnected = _port.connectPort("COM4");
    if(!isConnected){
        QMessageBox::critical(this,"Error","There is a problem connect to port");
    }else{
//        QMessageBox::information(this,"Result","Port open");
        ui->pushOpen->setEnabled(false);
        ui->portList->setEnabled(false);
        ui->statusPort->setText("Opened");
        ui->btnScada->setEnabled(true);
        ui->pushQuery->setEnabled(true);
        ui->statusPort->setStyleSheet("color: blue;");
        /**/
        static QSqlTableModel *model = new QSqlTableModel;
        model->setTable("LogRS232");
        model->setEditStrategy(QSqlTableModel::OnManualSubmit);
        model->select();
        ui->tableViewDb->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableViewDb->setModel(model);
        ui->tableViewDb->show();
//        delete(model);
    }
}
void MainWindow::handleMsgType1(NozzleMessage &data)
{
    msgCounter++;
    ui->msgCounter->display(QString::number(msgCounter));
    static QSqlTableModel *modelLogRS232 = new QSqlTableModel;
    QDateTime currentTime;
    QString qryCmdLogRS232 = "insert into LogRS232 values(";
    QSqlQuery query;
    Nozzle *nozzleTarget = nozzlePtr->findNozzle(nozzlePtr,_port.nozzleMsg.Id,_port.nozzleMsg.No);
    if(nozzleTarget == nullptr){
        qDebug() << "Not found Nozzle";
        return;
    }
    /*fill data vao cac doi tuong voi bom*/
    currentTime = QDateTime::currentDateTime();
    qDebug() << currentTime.toString("dd/MM/yyyy hh:mm:ss");
    nozzleTarget->setStatus(_port.nozzleMsg.Status);
    nozzleTarget->setTime(currentTime.toString("dd/MM/yyyy hh:mm:ss"));
    nozzleTarget->setLiter(_port.nozzleMsg.liter_finish.toLongLong());
    nozzleTarget->setTotalMoney(_port.nozzleMsg.money_finish.toLongLong());
    nozzleTarget->setUnitPrice(_port.nozzleMsg.unitPrice_finish.toLongLong());
    /**/
    qryCmdLogRS232.append("'");
    qryCmdLogRS232.append(nozzleTarget->getName());
    qryCmdLogRS232.append("'");
    qryCmdLogRS232.append(",");
    qryCmdLogRS232.append(QString::number(_port.nozzleMsg.Id));
    qryCmdLogRS232.append(",");
    qryCmdLogRS232.append(QString::number(_port.nozzleMsg.No));
    qryCmdLogRS232.append(",");
    qryCmdLogRS232.append(QString::number(_port.nozzleMsg.Status));
    qryCmdLogRS232.append(",");
    qryCmdLogRS232.append(_port.nozzleMsg.liter_begin);
    qryCmdLogRS232.append(",");
    qryCmdLogRS232.append(_port.nozzleMsg.unitPrice_begin);
    qryCmdLogRS232.append(",");
    qryCmdLogRS232.append(_port.nozzleMsg.money_begin);
    qryCmdLogRS232.append(",");
    qryCmdLogRS232.append(_port.nozzleMsg.liter_finish);
    qryCmdLogRS232.append(",");
    qryCmdLogRS232.append(_port.nozzleMsg.unitPrice_finish);
    qryCmdLogRS232.append(",");
    qryCmdLogRS232.append(_port.nozzleMsg.money_finish);
    qryCmdLogRS232.append(",");
    qryCmdLogRS232.append("'");
    qryCmdLogRS232.append(nozzleTarget->getTime());
    qryCmdLogRS232.append("'");
    qryCmdLogRS232.append(")");
    query.prepare(qryCmdLogRS232);
    qDebug()<< qryCmdLogRS232;
    if (!query.exec()) {
        qDebug() << "Insert failed:" << query.lastError();
    }
    /*Insert table error begin*/
    QString qry_cmd_table2 = "insert into err_log values(";
    qry_cmd_table2.append("'");
    qry_cmd_table2.append(nozzleTarget->getName());
    qry_cmd_table2.append("'");
    qry_cmd_table2.append(",");
    switch (_port.nozzleMsg.Status){
    case 0:
        break;
    case 1:
        nozzleTarget->setLostLog();
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
    modelLogRS232->setTable("LogRS232");
    modelLogRS232->setEditStrategy(QSqlTableModel::OnManualSubmit);
    modelLogRS232->select();
    ui->tableViewDb->setModel(modelLogRS232);
    ui->tableViewDb->resizeColumnsToContents();
    ui->tableViewDb->show();
}

void MainWindow::handleMsgType2(NozzleMessage &data)
{
    QDateTime currentTime;
    QSqlQuery query;
    QString qryCmdErrLog = "insert into err_log values(";
    QString qryCmdLogRS232 = "insert into LogRS232 values(";
    msgCounter++;
    ui->msgCounter->display(QString::number(msgCounter));
    for(int i = 0 ; i < nozzleNum;i++){
        if(nozzlePtr[i].getId485() == data.Id){
            /**/
            nozzlePtr[i].setStatus(data.Status);
            qryCmdErrLog.append("'");
            qryCmdErrLog.append(nozzlePtr[i].getName());
            qryCmdErrLog.append("'");
            qryCmdErrLog.append(",");
            switch (data.Status){
            case 0:
                break;
            case 1:
                s_ErrMissLog = true;
                break;
            case 2:
                nozzlePtr[i].setDisconnect();
                s_ErrDisconnect = true;
                break;
            case 3:
                nozzlePtr[i].setShutdown();
                s_ErrStartup = true;
                break;
            default:
                break;
            }
            qryCmdErrLog.append(QString::number(s_ErrMissLog));
            qryCmdErrLog.append(",");
            qryCmdErrLog.append(QString::number(s_ErrDisconnect));
            qryCmdErrLog.append(",");
            qryCmdErrLog.append(QString::number(s_ErrStartup));
            qryCmdErrLog.append(",");
            qryCmdErrLog.append(":time)");
            currentTime = QDateTime::currentDateTime();
            nozzlePtr[i].setTime(currentTime.toString("dd/MM/yyyy hh:mm:ss"));
            query.prepare(qryCmdErrLog);
            query.bindValue(":time", currentTime.toString("dd/MM/yyyy hh:mm:ss"));
            qDebug() << qryCmdErrLog;
            if (!query.exec()) {
                qDebug() << "Insert Err_log failed:" << query.lastError();
            }
            qryCmdErrLog.clear();
            s_ErrMissLog = false;
            s_ErrDisconnect = false;
            s_ErrStartup = false;
            /*main table*/
            qryCmdLogRS232.append("'");
            qryCmdLogRS232.append(nozzlePtr[i].getName());
            qryCmdLogRS232.append("'");
            qryCmdLogRS232.append(",");
            qryCmdLogRS232.append(QString::number(_port.nozzleMsg.Id));
            qryCmdLogRS232.append(",");
            qryCmdLogRS232.append(QString::number(_port.nozzleMsg.No));
            qryCmdLogRS232.append(",");
            qryCmdLogRS232.append(QString::number(_port.nozzleMsg.Status));
            qryCmdLogRS232.append(",");
            qryCmdLogRS232.append(_port.nozzleMsg.liter_begin);
            qryCmdLogRS232.append(",");
            qryCmdLogRS232.append(_port.nozzleMsg.unitPrice_begin);
            qryCmdLogRS232.append(",");
            qryCmdLogRS232.append(_port.nozzleMsg.money_begin);
            qryCmdLogRS232.append(",");
            qryCmdLogRS232.append(_port.nozzleMsg.liter_finish);
            qryCmdLogRS232.append(",");
            qryCmdLogRS232.append(_port.nozzleMsg.unitPrice_finish);
            qryCmdLogRS232.append(",");
            qryCmdLogRS232.append(_port.nozzleMsg.money_finish);
            qryCmdLogRS232.append(",");
            qryCmdLogRS232.append(":time)");
            query.prepare(qryCmdLogRS232);
            query.bindValue(":time", currentTime.toString("dd/MM/yyyy hh:mm:ss"));
            qDebug() << qryCmdLogRS232;
            if (!query.exec()) {
                qDebug() << "Insert Err_log failed:" << query.lastError();
            }
            qryCmdLogRS232.clear();
        }
    }
}

void MainWindow::on_btnScada_clicked()
{
    static bool isInit = false;
    Scada *Scada_window = Scada::getScada();
    if(isInit == false){
        Scada_window->initListNozzle(nozzlePtr,nozzleNum);
        isInit = true;
    }
    Scada_window->showMaximized();
}

void MainWindow::showDataReceived(QByteArray data)
{
    ui->plainTextEdit->insertPlainText(data.toHex());
}

void MainWindow::on_pushQuery_clicked()
{
    static bool isInit = false;
    Filter *Filter_window = Filter::getFilter();
    if(isInit == false){
        Filter_window->initListNozzle(nozzlePtr,nozzleNum);
        isInit = true;
    }
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

void MainWindow::on_assignNozzle_clicked()
{
    if(nozzleNum > 31){
        QMessageBox::critical(this,"Error",tr("Số vòi đã đạt giới hạn"));
        return;
    }
    QSqlQuery query;
    /*Check valid duplicate begin*/
//    uint8_t i = 0;
//    QString qry1 = "SELECT * FROM Nozzle_Assign";
//    query.prepare(qry1);
//    if (!query.exec()) {
//        qDebug() << "Insert failed:" << query.lastError();
//    }
//    while(query.next()){
//        nozzlePtr[i].setName(query.value("Vòi").toString());
//            nozzlePtr[i].setId485(query.value("ID485").toInt());
//        nozzlePtr[i].setNo(query.value("No").toInt());
//        nozzlePtr[i].setLiter(0);
//        nozzlePtr[i].setTotalMoney(0);
//        nozzlePtr[i].setUnitPrice(0);
//        nozzlePtr[i].setStatus(0);
//        i++;
//    }
    /*Check valid duplicate end*/
    /*insert assign nozzle to db*/

    QString qry_cmd = "insert into Nozzle_Assign values('";
    qry_cmd.append(ui->nameNozzle->text());
    qry_cmd.append("'");
    qry_cmd.append(",");
    qry_cmd.append(ui->ID485->text());
    qry_cmd.append(",");
    qry_cmd.append(ui->No->text());
    qry_cmd.append(")");
    /**/
    query.prepare(qry_cmd);
    qDebug()<< qry_cmd;
    if (!query.exec()) {
        qDebug() << "Insert failed:" << query.lastError();
    }
    /*Show db*/
    static QSqlTableModel *assignNozzleModel = new QSqlTableModel;
    assignNozzleModel->setTable("Nozzle_Assign");
    assignNozzleModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    assignNozzleModel->select();
    ui->assignResult->setModel(assignNozzleModel);
    ui->assignResult->resizeColumnsToContents();
    ui->assignResult->show();
    /**/
#if 0
    ui->assignResult->model()->setData(ui->assignResult->model()->index(nozzleNum, 0),ui->nameNozzle->text(),Qt::EditRole);
    ui->assignResult->model()->setData(ui->assignResult->model()->index(nozzleNum, 1),ui->ID485->text(),Qt::EditRole);
    ui->assignResult->model()->setData(ui->assignResult->model()->index(nozzleNum, 2),ui->No->text(),Qt::EditRole);
    nozzlePtr[nozzleNum].setName(ui->nameNozzle->text());
    nozzlePtr[nozzleNum].setId485(ui->ID485->text().toUInt());
    nozzlePtr[nozzleNum].setNo(ui->No->text().toUInt());
#endif
    ui->nameNozzle->clear();
    ui->ID485->clear();
    ui->No->clear();
    nozzleNum++;
//    ui->assignResult->setRowCount(nozzleNum+1);
}

void MainWindow::on_assignFinish_clicked()
{
    QSqlQuery query ;
    uint8_t i = 0;
    QString qry_cmd = "SELECT * FROM Nozzle_Assign";
    query.prepare(qry_cmd);
    if (!query.exec()) {
        qDebug() << "Insert failed:" << query.lastError();
    }
    while(query.next()){
        nozzlePtr[i].setName(query.value("Vòi").toString());
        nozzlePtr[i].setId485(query.value("ID485").toInt());
        nozzlePtr[i].setNo(query.value("No").toInt());
        nozzlePtr[i].setLiter(0);
        nozzlePtr[i].setTotalMoney(0);
        nozzlePtr[i].setUnitPrice(0);
        nozzlePtr[i].setStatus(0);
        i++;
    }
    nozzleNum = i;
    ui->assignNozzle->setEnabled(false);
//    ui->pushOpen->setEnabled(true);
}

