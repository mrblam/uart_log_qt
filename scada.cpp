#include "scada.h"
#include "ui_scada.h"
#include "nozzlehelper.h"

static nozzle nozzle_arr[NOZZLE_NUM];

Scada::Scada(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Scada)
{
    ui->setupUi(this);
    this->setWindowTitle("SCADA");
    connect(&heartbeatTicker,&QTimer::timeout,this,&Scada::updateScada);
    heartbeatTicker.start(500);
    QStringList horzHeaders;
    ui->tableWidget->setRowCount(NOZZLE_NUM);
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
    for (int i = 0;i < NOZZLE_NUM;i++){
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i, 0),i,Qt::EditRole);
    }
}

Scada::~Scada()
{
    delete ui;
}

void Scada::updateScada()
{
    for(int i = 0;i < NOZZLE_NUM;i++){
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i, 1),nozzle_arr[i].time,Qt::EditRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i, 2),nozzle_arr[i].disconnect,Qt::EditRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i, 3),nozzle_arr[i].lostLog,Qt::EditRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i, 4),nozzle_arr[i].shutdown,Qt::EditRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i, 5),nozzle_arr[i].totalMoney,Qt::EditRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i, 6),nozzle_arr[i].liter,Qt::EditRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i, 7),nozzle_arr[i].unitPrice,Qt::EditRole);
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

void Scada::updateNozzleData(NozzleMessage &data)
{
    uint8_t id_nozzle = 0;
    uint8_t status = 0;
    QDateTime time_current = QDateTime::currentDateTime();
    id_nozzle = data.Id;
    if(id_nozzle < NOZZLE_NUM && id_nozzle >= 0){
        status = data.Status;
        nozzle_arr[id_nozzle].time = time_current.toString("dd/MM/yyyy hh:mm:ss");
        switch (status) {
        case 0:
            nozzle_arr[id_nozzle].liter = data.liter_4.toLongLong();
            nozzle_arr[id_nozzle].unitPrice = data.unitPrice_4.toLongLong();
            nozzle_arr[id_nozzle].totalMoney = data.money_4.toLongLong();
            break;
        case 1:
            nozzle_arr[id_nozzle].lostLog++;
            break;
        case 2:
            nozzle_arr[id_nozzle].disconnect++;
            break;
        case 3:
            nozzle_arr[id_nozzle].shutdown++;
            break;
        default:
            break;
        }
    }
}


