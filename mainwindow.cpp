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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadPort();
    connect(&_port,&SerialPort::dataReceived,this,&MainWindow::readDataSerial);
    connect(this,&MainWindow::prepareData,Scada::getScada(),&Scada::updateLog);
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
        //*//
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("mydatabase.db");
        if (!db.open()) {
            qDebug() << "Cannot careate new database. Error:" << db.lastError().text();
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
                       "time DATETIME)");
        }
        QSqlTableModel *model = new QSqlTableModel;
        model->setTable("LogRS232");
        model->setEditStrategy(QSqlTableModel::OnManualSubmit);
        model->select();
        ui->tableViewDb->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableViewDb->setModel(model);
        ui->tableViewDb->show();
        delete(model);
        //*//
    }
}
//sequence,id,status,1,2,3,4
void MainWindow::readDataSerial(QByteArray data)
{//// data received
    static QSqlTableModel *model_1 = new QSqlTableModel;
    QDateTime currentTime;
    QString l_data;
    QString data_part;
    QString qry_cmd = "insert into LogRS232 values(";
    LogRecord *record = LogRecord::getRecord();
    QSqlQuery query;
    ui->listMessage->addItem(QString (data));
    record->parseData(&data);
    emit prepareData();
    l_data = (data.toHex());
    data_part = l_data.mid(0,2);
    qry_cmd.append(data_part); //Sequence
    qry_cmd.append(",");
    data_part = l_data.mid(2,2);
    qry_cmd.append(data_part); //ID
    qry_cmd.append(",");
    data_part = l_data.mid(4,2);
    qry_cmd.append(data_part); //Status
    qry_cmd.append(",");
    data_part = l_data.mid(6,16);
    qry_cmd.append(data_part);
    qry_cmd.append(",");
    data_part = l_data.mid(22,12);
    qry_cmd.append(data_part);
    qry_cmd.append(",");
    data_part = l_data.mid(34,16);
    qry_cmd.append(data_part);
    qry_cmd.append(",");
    data_part = l_data.mid(50,16);
    qry_cmd.append(data_part);
    qry_cmd.append(",");
    data_part = l_data.mid(66,12);
    qry_cmd.append(data_part);
    qry_cmd.append(",");
    data_part = l_data.mid(78,16);
    qry_cmd.append(data_part);
    qry_cmd.append(",");
    data_part = l_data.mid(94,16);
    qry_cmd.append(data_part);
    qry_cmd.append(",");
    data_part = l_data.mid(110,12);
    qry_cmd.append(data_part);
    qry_cmd.append(",");
    data_part = l_data.mid(122,16);
    qry_cmd.append(data_part);
    qry_cmd.append(",");
    data_part = l_data.mid(138,16);
    qry_cmd.append(data_part);
    qry_cmd.append(",");
    data_part = l_data.mid(154,12);
    qry_cmd.append(data_part);
    qry_cmd.append(",");
    data_part = l_data.mid(166,16);
    qry_cmd.append(data_part);
    qry_cmd.append(",");
    qry_cmd.append(":time)");
    currentTime = QDateTime::currentDateTime();
    query.prepare(qry_cmd);
    query.bindValue(":time", currentTime);
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
    Scada *Scada_window = new Scada;
    Scada_window->show();
}

#if 0
    qry_cmd.append(QByteArray::number(record.getSeqence()));
    qry_cmd.append(",");
    qry_cmd.append(QByteArray::number(record.getId()));
    qry_cmd.append(",");
    qry_cmd.append(QByteArray::number(record.getStatus()));
    qry_cmd.append(",");
    QString str;
    str= mergeCharArray(record.getLiter_1(),8) ;
    qry_cmd.append((str));
    qry_cmd.append(",");
    str = mergeCharArray(record.getUnitPrice_1(),6);
    qry_cmd.append((str));
    qry_cmd.append(",");
    str = mergeCharArray(record.getMoney_1(),8);
    qry_cmd.append((str));
    qry_cmd.append(",");
    str= mergeCharArray(record.getLiter_2(),8) ;
    qry_cmd.append((str));
    qry_cmd.append(",");
    str = mergeCharArray(record.getUnitPrice_2(),6);
    qry_cmd.append((str));
    qry_cmd.append(",");
    str = mergeCharArray(record.getMoney_2(),8);
    qry_cmd.append((str));
    qry_cmd.append(",");
    str= mergeCharArray(record.getLiter_3(),8) ;
    qry_cmd.append((str));
    qry_cmd.append(",");
    str = mergeCharArray(record.getUnitPrice_3(),6);
    qry_cmd.append((str));
    qry_cmd.append(",");
    str = mergeCharArray(record.getMoney_3(),8);
    qry_cmd.append((str));
    qry_cmd.append(",");
    str= mergeCharArray(record.getLiter_4(),8) ;
    qry_cmd.append((str));
    qry_cmd.append(",");
    str = mergeCharArray(record.getUnitPrice_4(),6);
    qry_cmd.append((str));
    qry_cmd.append(",");
    str = mergeCharArray(record.getMoney_4(),8);
    qry_cmd.append((str));
    qry_cmd.append(",");
#endif
//    qry_cmd.append(QByteArray::number(record.getUnitPrice_1()));
//    qry_cmd.append(",");
//    qry_cmd.append(QByteArray::number(record.getMoney_1()));
//    qry_cmd.append(",");
//    qry_cmd.append(QByteArray::number(record.getLiter_2()));
//    qry_cmd.append(",");
//    qry_cmd.append(QByteArray::number(record.getUnitPrice_2()));
//    qry_cmd.append(",");
//    qry_cmd.append(QByteArray::number(record.getMoney_2()));
//    qry_cmd.append(",");
//    qry_cmd.append(QByteArray::number(record.getLiter_3()));
//    qry_cmd.append(",");
//    qry_cmd.append(QByteArray::number(record.getUnitPrice_3()));
//    qry_cmd.append(",");
//    qry_cmd.append(QByteArray::number(record.getMoney_3()));
//    qry_cmd.append(",");
//    qry_cmd.append(QByteArray::number(record.getLiter_3()));
//    qry_cmd.append(",");
//    qry_cmd.append(QByteArray::number(record.getUnitPrice_4()));
//    qry_cmd.append(",");
//    qry_cmd.append(QByteArray::number(record.getMoney_4()));
