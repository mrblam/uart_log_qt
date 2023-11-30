#include "scada.h"
#include "ui_scada.h"
#include "nozzlehelper.h"
#include <QSqlQuery>

Scada::Scada(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Scada)
{
    ui->setupUi(this);
    nozzlePtr = nullptr;
    this->setWindowTitle("SCADA");
    this->setWindowIcon(QIcon(":/UI/Icon/p.ico"));
    connect(&heartbeatTicker,&QTimer::timeout,this,&Scada::updateScada);
    heartbeatTicker.start(100);
    QStringList horzHeaders;
    ui->tableWidget->setRowCount(32);
    ui->tableWidget->setColumnCount(8);
    horzHeaders << QObject::tr("Vòi số")
                << QObject::tr("Thời điểm gần nhất")
                << QObject::tr("Số lần mất kết nối")
                << QObject::tr("Số lần mất giao dịch")
                << QObject::tr("Số lần khởi động")
                << QObject::tr("Thành tiền(vnđ)")
                << QObject::tr("Lượng lít(ml)")
                << QObject::tr("Đơn giá(vnđ/lít)");
    ui->tableWidget->setHorizontalHeaderLabels(horzHeaders);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

Scada::~Scada()
{
    delete ui;
}

void Scada::updateScada()
{
    if(nozzlePtr == nullptr){
        return;
    }
    for(int i = 0;i < nozzleNum;i++){
            ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i, 0),nozzlePtr[i].getName(),Qt::EditRole);
            ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i, 1),nozzlePtr[i].getTime(),Qt::EditRole);
            ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i, 2),nozzlePtr[i].getDisconnect(),Qt::EditRole);
            ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i, 3),nozzlePtr[i].getLostLog(),Qt::EditRole);
            ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i, 4),nozzlePtr[i].getShutDown(),Qt::EditRole);
            if(nozzlePtr[i].getStatus() == 0){
                ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i, 5),nozzlePtr[i].getTotalMoney(),Qt::EditRole);
                ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i, 6),nozzlePtr[i].getLiter(),Qt::EditRole);
                ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i, 7),nozzlePtr[i].getUnitPrice(),Qt::EditRole);
            }
    }
    for(int i = nozzleNum;i < 32;i++){
            ui->tableWidget->model()->clearItemData(ui->tableWidget->model()->index(i, 0));
            ui->tableWidget->model()->clearItemData(ui->tableWidget->model()->index(i, 1));
            ui->tableWidget->model()->clearItemData(ui->tableWidget->model()->index(i, 2));
            ui->tableWidget->model()->clearItemData(ui->tableWidget->model()->index(i, 3));
            ui->tableWidget->model()->clearItemData(ui->tableWidget->model()->index(i, 4));
            ui->tableWidget->model()->clearItemData(ui->tableWidget->model()->index(i, 5));
            ui->tableWidget->model()->clearItemData(ui->tableWidget->model()->index(i, 6));
            ui->tableWidget->model()->clearItemData(ui->tableWidget->model()->index(i, 7));

    }
}

Scada *Scada::getScada()
{
    static Scada* self;
    if(self==nullptr){
        self=new Scada();
    }
    return self;
}

nozzle2D *Scada::getNozzle()
{
    return nullptr;
}

void Scada::initListNozzle(Nozzle *list, uint8_t &num)
{
    nozzlePtr = list;
    nozzleNum = num;
}

void Scada::insertConnectedStateToDB()
{
    QSqlQuery query;
    QDateTime currentTime;
    ui->statusConnect->setText("Connected");
    ui->statusConnect->setStyleSheet("font-weight: bold; color: blue;");
    currentTime = QDateTime::currentDateTime();
    query.prepare("insert into Log_State values (:time,'MCU Connected')");
    query.bindValue(":time", currentTime.toString("dd/MM/yyyy hh:mm:ss"));
    if (!query.exec()) {
        qDebug() << "Insert disconnect into Log_State failed";
    }
}

void Scada::setDisconnectToMCU()
{
    QSqlQuery query;
    QDateTime currentTime;
    ui->statusConnect->setText("Disconnect from MCU (Timeout)");
    ui->statusConnect->setStyleSheet("font-weight: bold; color: red;");
    currentTime = QDateTime::currentDateTime();
    query.prepare("insert into Log_State values (:time,'MCU Timeout')");
    query.bindValue(":time", currentTime.toString("dd/MM/yyyy hh:mm:ss"));
    if (!query.exec()) {
        qDebug() << "Insert disconnect into Log_State failed";
    }
}


