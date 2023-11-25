#include "scada.h"
#include "ui_scada.h"
#include "nozzlehelper.h"

static nozzle nozzle_arr[20][4];

Scada::Scada(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Scada)
{
    ui->setupUi(this);
    this->setWindowTitle("SCADA");
    this->setWindowIcon(QIcon(":/UI/Icon/p.ico"));
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
}

Scada::~Scada()
{
    delete ui;
}

void Scada::updateScada()
{
    for(int i = 0;i < 20;i++){
        for(int j = 0;j < 4; j++){
            ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i*4+j, 0),nozzle_arr[i][j].name,Qt::EditRole);
            ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i*4+j, 1),nozzle_arr[i][j].time,Qt::EditRole);
            ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i*4+j, 2),nozzle_arr[i][j].disconnect,Qt::EditRole);
            ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i*4+j, 3),nozzle_arr[i][j].lostLog,Qt::EditRole);
            ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i*4+j, 4),nozzle_arr[i][j].shutdown,Qt::EditRole);
            ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i*4+j, 5),nozzle_arr[i][j].totalMoney,Qt::EditRole);
            ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i*4+j, 6),nozzle_arr[i][j].liter,Qt::EditRole);
            ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i*4+j, 7),nozzle_arr[i][j].unitPrice,Qt::EditRole);
        }
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
    return &nozzle_arr;
}

void Scada::updateNozzleData(NozzleMessage &data)
{
    uint8_t id485 = 0;
    uint8_t no;
    uint8_t status = 0;
    QDateTime time_current = QDateTime::currentDateTime();
    id485 = data.Id;
    no = data.No;
    if(id485 < NOZZLE_NUM && id485 >= 0){
        status = data.Status;
        nozzle_arr[id485][no].time = time_current.toString("dd/MM/yyyy hh:mm:ss");
        switch (status) {
        case 0:
            nozzle_arr[id485][no].liter = data.liter_idle.toLongLong();
            nozzle_arr[id485][no].unitPrice = data.unitPrice_idle.toLongLong();
            nozzle_arr[id485][no].totalMoney = data.money_idle.toLongLong();
            break;
        case 1:
            nozzle_arr[id485][no].lostLog++;
            break;
        case 2:
            nozzle_arr[id485][no].disconnect++;
            break;
        case 3:
            nozzle_arr[id485][no].shutdown++;
            break;
        default:
            break;
        }
    }
    ui->statusConnect->setText("Connected");
    ui->statusConnect->setStyleSheet("font-weight: bold; color: blue;");
}

void Scada::setDisconnectToMCU()
{
    ui->statusConnect->setText("Disconnect from MCU (Timeout)");
    ui->statusConnect->setStyleSheet("font-weight: bold; color: red;");
}


