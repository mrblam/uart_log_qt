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

static bool s_ErrMissLog = false;
static bool s_ErrDisconnect = false;
static bool s_ErrStartup = false;
static uint64_t msgcounter = 0;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QStringList horzHeaders;
    ui->setupUi(this);
    ui->btnScada->setEnabled(false);
    ui->pushQuery->setEnabled(false);
    ui->pushOpen->setEnabled(false);
    loadPort();
    connect(&_port,&SerialPort::showDataReceived,this,&MainWindow::showDataReceived);
    connect(&_port,&SerialPort::handleMsgType1,this,&MainWindow::handleMsgType1);
    connect(&_port,&SerialPort::handleMsgType2,this,&MainWindow::handleMsgType2);
    connect(&_port,&SerialPort::updateNozzleData,Scada::getScada(),&Scada::updateNozzleData);
    connect(&_port,&SerialPort::disconnectToMCU,Scada::getScada(),&Scada::setDisconnectToMCU);
    this->setWindowTitle("Peco Log");
    this->setWindowIcon(QIcon(":/UI/Icon/p.ico"));
    nozzleNum = 0;
    nozzlePtr = nozzleArr;
    /**/
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("mydatabase.db");
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
        }
    /**/
    /**/
    QSqlTableModel *model11 = new QSqlTableModel;
    model11->setTable("Nozzle_Assign");
    model11->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model11->select();
    ui->assignResult->setModel(model11);
    ui->assignResult->resizeColumnsToContents();
    ui->assignResult->show();
    /**/
#if 0
    ui->assignResult->setRowCount(1);
    ui->assignResult->setColumnCount(3);
    horzHeaders << QObject::tr("Tên vòi")
                << QObject::tr("ID485")
                << QObject::tr("No");
    ui->assignResult->setHorizontalHeaderLabels(horzHeaders);
    ui->assignResult->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->assignResult->show();
#endif
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
        /**/
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
    msgcounter++;
    ui->msgCounter->display(QString::number(msgcounter));
    static QSqlTableModel *model_1 = new QSqlTableModel;
    QDateTime currentTime;
    QString qry_cmd = "insert into LogRS232 values(";
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
    qry_cmd.append("'");
    qry_cmd.append(nozzleTarget->getName());
    qry_cmd.append("'");
    qry_cmd.append(",");
    qry_cmd.append(QString::number(_port.nozzleMsg.Id));
    qry_cmd.append(",");
    qry_cmd.append(QString::number(_port.nozzleMsg.No));
    qry_cmd.append(",");
    qry_cmd.append(QString::number(_port.nozzleMsg.Status));
    qry_cmd.append(",");
    qry_cmd.append(_port.nozzleMsg.liter_begin);
    qry_cmd.append(",");
    qry_cmd.append(_port.nozzleMsg.unitPrice_begin);
    qry_cmd.append(",");
    qry_cmd.append(_port.nozzleMsg.money_begin);
    qry_cmd.append(",");
    qry_cmd.append(_port.nozzleMsg.liter_finish);
    qry_cmd.append(",");
    qry_cmd.append(_port.nozzleMsg.unitPrice_finish);
    qry_cmd.append(",");
    qry_cmd.append(_port.nozzleMsg.money_finish);
//    qry_cmd.append(",");
//    qry_cmd.append(_port.nozzleMsg.liter_idle);
//    qry_cmd.append(",");
//    qry_cmd.append(_port.nozzleMsg.unitPrice_idle);
//    qry_cmd.append(",");
//    qry_cmd.append(_port.nozzleMsg.money_idle);
    qry_cmd.append(",");
//    qry_cmd.append(":time)");
    qry_cmd.append("'");
    qry_cmd.append(nozzleTarget->getTime());
    qry_cmd.append("'");
    qry_cmd.append(")");
    query.prepare(qry_cmd);
    qDebug()<< qry_cmd;
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
    model_1->setTable("LogRS232");
    model_1->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model_1->select();
    ui->tableViewDb->setModel(model_1);
    ui->tableViewDb->resizeColumnsToContents();
    ui->tableViewDb->show();
}

void MainWindow::handleMsgType2(NozzleMessage &data)
{
    QDateTime currentTime;
    QSqlQuery query;
    QString qry_cmd_table2 = "insert into err_log values(";
    QString qry_cmd_mainTable = "insert into LogRS232 values(";
    msgcounter++;
    ui->msgCounter->display(QString::number(msgcounter));
    for(int i = 0 ; i < nozzleNum;i++){
        if(nozzlePtr[i].getId485() == data.Id){
            /**/
            nozzlePtr[i].setStatus(data.Status);
            qry_cmd_table2.append("'");
            qry_cmd_table2.append(nozzlePtr[i].getName());
            qry_cmd_table2.append("'");
            qry_cmd_table2.append(",");
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
            qry_cmd_table2.append(QString::number(s_ErrMissLog));
            qry_cmd_table2.append(",");
            qry_cmd_table2.append(QString::number(s_ErrDisconnect));
            qry_cmd_table2.append(",");
            qry_cmd_table2.append(QString::number(s_ErrStartup));
            qry_cmd_table2.append(",");
            qry_cmd_table2.append(":time)");
            currentTime = QDateTime::currentDateTime();
            nozzlePtr[i].setTime(currentTime.toString("dd/MM/yyyy hh:mm:ss"));
            query.prepare(qry_cmd_table2);
            query.bindValue(":time", currentTime.toString("dd/MM/yyyy hh:mm:ss"));
            qDebug() << qry_cmd_table2;
            if (!query.exec()) {
                qDebug() << "Insert Err_log failed:" << query.lastError();
            }
            qry_cmd_table2.clear();
            s_ErrMissLog = false;
            s_ErrDisconnect = false;
            s_ErrStartup = false;
            /*main table*/
            qry_cmd_mainTable.append("'");
            qry_cmd_mainTable.append(nozzlePtr[i].getName());
            qry_cmd_mainTable.append("'");
            qry_cmd_mainTable.append(",");
            qry_cmd_mainTable.append(QString::number(_port.nozzleMsg.Id));
            qry_cmd_mainTable.append(",");
            qry_cmd_mainTable.append(QString::number(_port.nozzleMsg.No));
            qry_cmd_mainTable.append(",");
            qry_cmd_mainTable.append(QString::number(_port.nozzleMsg.Status));
            qry_cmd_mainTable.append(",");
            qry_cmd_mainTable.append(_port.nozzleMsg.liter_begin);
            qry_cmd_mainTable.append(",");
            qry_cmd_mainTable.append(_port.nozzleMsg.unitPrice_begin);
            qry_cmd_mainTable.append(",");
            qry_cmd_mainTable.append(_port.nozzleMsg.money_begin);
            qry_cmd_mainTable.append(",");
            qry_cmd_mainTable.append(_port.nozzleMsg.liter_finish);
            qry_cmd_mainTable.append(",");
            qry_cmd_mainTable.append(_port.nozzleMsg.unitPrice_finish);
            qry_cmd_mainTable.append(",");
            qry_cmd_mainTable.append(_port.nozzleMsg.money_finish);
            qry_cmd_mainTable.append(",");
            qry_cmd_mainTable.append(":time)");
            query.prepare(qry_cmd_mainTable);
            query.bindValue(":time", currentTime.toString("dd/MM/yyyy hh:mm:ss"));
            qDebug() << qry_cmd_mainTable;
            if (!query.exec()) {
                qDebug() << "Insert Err_log failed:" << query.lastError();
            }
            qry_cmd_mainTable.clear();
        }
    }
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
    /*insert assign nozzle to db*/
    QSqlQuery query;
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
    //    query.bindValue(":time", currentTime.toString("dd/MM/yyyy hh:mm:ss"));
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
    ui->pushOpen->setEnabled(true);
}

