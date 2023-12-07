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
static void showTableLogRS232(Ui::MainWindow *ui)
{
    static QSqlTableModel *model = new QSqlTableModel;
    model->setTable("LogRS232");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    ui->tableViewDb->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewDb->setModel(model);
    ui->tableViewDb->resizeColumnsToContents();
    ui->tableViewDb->show();
    ui->tableViewDb->scrollToBottom();
}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QDateTime currentTime;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QSqlQuery query;
    ui->setupUi(this);
    ui->btnScada->setEnabled(false);
    ui->pushQuery->setEnabled(false);
    ui->pushOpen->setEnabled(false);
    loadPort();
    // connect(&_port,&SerialPort::showDataReceived,this,&MainWindow::showDataReceived);
    connect(&portRS232,&SerialPort::handleMsgType1,this,&MainWindow::handleMsgType1);
    connect(&portRS232,&SerialPort::handleMsgType2,this,&MainWindow::handleMsgType2);
    connect(&port485,&SerialPort485::handleMsg485,this,&MainWindow::handleMsg485);
    connect(&portRS232,&SerialPort::updateState,Scada::getScada(),&Scada::insertConnectedStateToDB);
    connect(&portRS232,&SerialPort::disconnectToMCU,Scada::getScada(),&Scada::setDisconnectToMCU);
    this->setWindowTitle("Peco Log");
    this->setWindowIcon(QIcon(":/UI/Icon/p.ico"));
    nozzleNum = 0;
    nozzlePtr = nozzleArr;
    /*check COM Config*/
    bool comConfig = QFile::exists("config/config_port.txt");
    if(!comConfig){
        QMessageBox::critical(this,"Warning","Chưa cấu hình cổng COM");
    }
    /*check db exists*/
    db.setDatabaseName("mydatabase.db");
    bool dbExists = QFile::exists("mydatabase.db");
    if(!dbExists){
        if (!db.open()) {
            qDebug() << "Cannot careate new database. Error:" << db.lastError().text();
        } else {
            qDebug() << QObject::tr("Database is open!");
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
            query.exec("create table com1_log (Time DATETIME,SA TEXT,UA TEXT,Opcode TEXT,State TEXT,Lít TEXT,[Đơn giá hợp lệ] TEXT,[Đơn giá] TEXT,[Thành tiền] TEXT,No TEXT)");
            currentTime = QDateTime::currentDateTime();
            query.prepare("insert into Log_State values (:time,'Khởi động')");
            query.bindValue(":time", currentTime.toString("dd/MM/yyyy hh:mm:ss"));
            if (!query.exec()) {
                qDebug() << "Insert Log_State failed:" << query.lastError();
            }
            ui->assignNozzle->setEnabled(true);
            ui->assignFinish->setEnabled(true);
            ui->labelAssignInfo->setText("Chưa gán mã vòi bơm");
            ui->labelAssignInfo->setStyleSheet("color: red;");
        }
    }else{
        if (!db.open()) {
            qDebug() << "Cannot open exist database. Error:" << db.lastError().text();
        }else{
            qDebug() << QObject::tr("Database is open!");
            currentTime = QDateTime::currentDateTime();
            query.prepare("insert into Log_State values (:time,'Khởi động')");
            query.bindValue(":time", currentTime.toString("dd/MM/yyyy hh:mm:ss"));
            if (!query.exec()) {
                qDebug() << "Insert Log_State failed:" << query.lastError();
            }
            MainWindow::on_assignFinish_clicked();
        }
    }
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
    QString portName;
    int baud = 0;
    uint8_t lineCount = 0;
    /*Get port attribute*/
    QFile file("config/config_port.txt");
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(0, "Lỗi", file.errorString()+ "\nKiểm tra lại file config_port.txt");
        return;
    }
    QTextStream in(&file);
    while(!in.atEnd()&& lineCount < 2) {
        QString line = in.readLine();
        QStringList parts = line.split(":");
        if (parts.length() == 2 && parts[0].trimmed() == "port_name") {
            portName = parts[1].trimmed();
        }else if(parts.length() == 2 && parts[0].trimmed() == "baud"){
            baud = parts[1].trimmed().toInt();
        }
        ++lineCount;
    }
    file.close();
    /**/
    if(portName != nullptr){
        auto isConnected = portRS232.connectPort(portName,baud);
        if(!isConnected){
            QMessageBox::critical(this,"Error","Không thể mở cổng COM,kiểm tra lại cấu hình trong file config_port.txt");
        }else{
            //        QMessageBox::information(this,"Result","Port open");
            ui->portList->setCurrentText(portName);
            ui->pushOpen->setEnabled(false);
            ui->portList->setEnabled(false);
            ui->statusPort->setText("Opened");
            ui->btnScada->setEnabled(true);
            ui->pushQuery->setEnabled(true);
            ui->statusPort->setStyleSheet("color: blue;");
            /**/
            showTableLogRS232(ui);
            //        delete(model);
        }
    }
    auto isConnect =  port485.connectPort("COM1",19200);
    if(!isConnect){
        qDebug()<< "error com1";
    }
}
void MainWindow::handleMsgType1()
{
    msgCounter++;
    ui->msgCounter->display(QString::number(msgCounter));
    QDateTime currentTime;
    QString qryCmdLogRS232 = "insert into LogRS232 values(";
    QSqlQuery query;
    Nozzle *nozzleTarget = nozzlePtr->findNozzle(nozzlePtr,portRS232.nozzleMsg.Id485,portRS232.nozzleMsg.No,nozzleNum);
    if(nozzleTarget == nullptr){
        qDebug() << "Not found Nozzle";
        return;
    }
    /*fill data vao cac doi tuong voi bom*/
    currentTime = QDateTime::currentDateTime();
    qDebug() << currentTime.toString("dd/MM/yyyy hh:mm:ss");
    nozzleTarget->setStatus(portRS232.nozzleMsg.Status);
    nozzleTarget->setTime(currentTime.toString("dd/MM/yyyy hh:mm:ss"));
    nozzleTarget->setLiter(portRS232.nozzleMsg.liter_finish.toLongLong());
    nozzleTarget->setTotalMoney(portRS232.nozzleMsg.money_finish.toLongLong());
    nozzleTarget->setUnitPrice(portRS232.nozzleMsg.unitPrice_finish.toLongLong());
    /**/
    qryCmdLogRS232.append("'");
    qryCmdLogRS232.append(nozzleTarget->getName());
    qryCmdLogRS232.append("'");
    qryCmdLogRS232.append(",");
    qryCmdLogRS232.append(QString::number(portRS232.nozzleMsg.Id485));
    qryCmdLogRS232.append(",");
    qryCmdLogRS232.append(QString::number(portRS232.nozzleMsg.No));
    qryCmdLogRS232.append(",");
    qryCmdLogRS232.append(QString::number(portRS232.nozzleMsg.Status));
    qryCmdLogRS232.append(",");
    qryCmdLogRS232.append(portRS232.nozzleMsg.liter_begin);
    qryCmdLogRS232.append(",");
    qryCmdLogRS232.append(portRS232.nozzleMsg.unitPrice_begin);
    qryCmdLogRS232.append(",");
    qryCmdLogRS232.append(portRS232.nozzleMsg.money_begin);
    qryCmdLogRS232.append(",");
    qryCmdLogRS232.append(portRS232.nozzleMsg.liter_finish);
    qryCmdLogRS232.append(",");
    qryCmdLogRS232.append(portRS232.nozzleMsg.unitPrice_finish);
    qryCmdLogRS232.append(",");
    qryCmdLogRS232.append(portRS232.nozzleMsg.money_finish);
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
    switch (portRS232.nozzleMsg.Status){
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
    // showTableLogRS232(ui);

}

void MainWindow::handleMsgType2(NozzleMessage &data)
{
    QDateTime currentTime;
    QSqlQuery query;
    QString qryCmdErrLog;
    QString qryCmdLogRS232;
    msgCounter++;
    ui->msgCounter->display(QString::number(msgCounter));
    for(int i = 0 ; i < nozzleNum;i++){
        if(nozzlePtr[i].getId485() == data.Id485){
            /**/
            nozzlePtr[i].setStatus(data.Status);
            qryCmdErrLog.clear();
            qryCmdErrLog.append("insert into err_log values('");
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
            qryCmdLogRS232.clear();
            qryCmdLogRS232.append("insert into LogRS232 values('");
            qryCmdLogRS232.append(nozzlePtr[i].getName());
            qryCmdLogRS232.append("'");
            qryCmdLogRS232.append(",");
            qryCmdLogRS232.append(QString::number(portRS232.nozzleMsg.Id485));
            qryCmdLogRS232.append(",");
            qryCmdLogRS232.append(QString::number(portRS232.nozzleMsg.No));
            qryCmdLogRS232.append(",");
            qryCmdLogRS232.append(QString::number(portRS232.nozzleMsg.Status));
            qryCmdLogRS232.append(",");
            qryCmdLogRS232.append(portRS232.nozzleMsg.liter_begin);
            qryCmdLogRS232.append(",");
            qryCmdLogRS232.append(portRS232.nozzleMsg.unitPrice_begin);
            qryCmdLogRS232.append(",");
            qryCmdLogRS232.append(portRS232.nozzleMsg.money_begin);
            qryCmdLogRS232.append(",");
            qryCmdLogRS232.append(portRS232.nozzleMsg.liter_finish);
            qryCmdLogRS232.append(",");
            qryCmdLogRS232.append(portRS232.nozzleMsg.unitPrice_finish);
            qryCmdLogRS232.append(",");
            qryCmdLogRS232.append(portRS232.nozzleMsg.money_finish);
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
    // showTableLogRS232(ui);
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
    ui->plainTextEdit->ensureCursorVisible();
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
    query.prepare(qry_cmd);
    qDebug()<< qry_cmd;
    if (!query.exec()) {
        qDebug() << "Insert failed:" << query.lastError();
    }
    /*Show db*/
    MainWindow::on_pushReloadAssignNozz_clicked();
    /**/
    ui->nameNozzle->clear();
    ui->ID485->clear();
    ui->No->clear();
    nozzleNum++;
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
    /*Clear old data*/
    for(int i =0;i < nozzleNum;i++){
        nozzlePtr[i].setName("");
        nozzlePtr[i].setTime("");
        nozzlePtr[i].setId485(0);
        nozzlePtr[i].setNo(0);
        nozzlePtr[i].setLiter(0);
        nozzlePtr[i].setTotalMoney(0);
        nozzlePtr[i].setUnitPrice(0);
        nozzlePtr[i].setStatus(0);
    }
    /**/
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
    ui->assignFinish->setEnabled(false);
    ui->labelAssignInfo->setText("");
    Scada *Scada_window = Scada::getScada();
    Scada_window->initListNozzle(nozzlePtr,nozzleNum);
    Filter *Filter_window = Filter::getFilter();
    Filter_window->initListNozzle(nozzlePtr,nozzleNum);
    MainWindow::on_pushOpen_pressed();
}


void MainWindow::on_pushReloadAssignNozz_clicked()
{
    static QSqlTableModel *assignNozzleModel = new QSqlTableModel;
    assignNozzleModel->setTable("Nozzle_Assign");
    assignNozzleModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    assignNozzleModel->select();
    ui->assignResult->setModel(assignNozzleModel);
    ui->assignResult->resizeColumnsToContents();
    ui->assignResult->show();
    ui->assignFinish->setEnabled(true);
    ui->assignNozzle->setEnabled(true);
}

void MainWindow::handleMsg485(Msg485 &data)
{
    QDateTime currentTime;
    QSqlQuery query;
    QString qryCmdErrLog;
    currentTime = QDateTime::currentDateTime();
    qryCmdErrLog.clear();
    qryCmdErrLog.append("insert into com1_log values(:time,'");
    qryCmdErrLog.append(QString::number(data.SA,16));
    qryCmdErrLog.append("','");
    qryCmdErrLog.append(QString::number(data.UA,16));
    qryCmdErrLog.append("',");
    qryCmdErrLog.append(data.opcode.toHex());
    qryCmdErrLog.append(",'");
    qryCmdErrLog.append(QString::number(data.state,16));
    qryCmdErrLog.append("',");
    qryCmdErrLog.append(data.liter);
    qryCmdErrLog.append(",'");
    qryCmdErrLog.append(QString::number(data.unitPriceState,16));
    qryCmdErrLog.append("',");
    qryCmdErrLog.append(data.unitPrice);
    qryCmdErrLog.append(",");
    qryCmdErrLog.append(data.totalMoney);
    qryCmdErrLog.append(",'");
    qryCmdErrLog.append(QString::number(data.no,16));
    qryCmdErrLog.append("')");
    query.prepare(qryCmdErrLog);
    query.bindValue(":time", currentTime.toString("dd/MM/yyyy hh:mm:ss"));
    if (!query.exec()) {
        qDebug() << "Insert com1_log failed:" << query.lastError();
    }
}
//(Time DATETIME,SA TEXT,UA TEXT,Opcode TEXT,State TEXT,Lít TEXT,[Đơn giá hợp lệ] TEXT,[Đơn giá] TEXT,[Thành tiền] TEXT,No TEXT)");
