#include "scada.h"
#include "ui_scada.h"
#include "nozzlehelper.h"

//static nozzle nozzle_arr[20][4];

Scada::Scada(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Scada)
{
    ui->setupUi(this);
    nozzlePtr = nullptr;
    this->setWindowTitle("SCADA");
    this->setWindowIcon(QIcon(":/UI/Icon/p.ico"));
    connect(&heartbeatTicker,&QTimer::timeout,this,&Scada::updateScada);
    heartbeatTicker.start(500);
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

void Scada::initListNozzle(Nozzle *list, uint8_t num)
{
    nozzlePtr = list;
    nozzleNum = num;
}

void Scada::updateNozzleData(NozzleMessage &data)
{
    /// thong nhat su dung con tro tro toi mang nozzlearr[32] o lop mainwindow
#if 0
    uint8_t id485 = 0;
    uint8_t no;
    uint8_t status = 0;
    QDateTime time_current = QDateTime::currentDateTime();
    id485 = data.Id;
    no = data.No;
    if(id485 < NOZZLE_NUM && id485 >= MIN_ID485){
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
#endif
    ui->statusConnect->setText("Connected");
    ui->statusConnect->setStyleSheet("font-weight: bold; color: blue;");
}

void Scada::setDisconnectToMCU()
{
    ui->statusConnect->setText("Disconnect from MCU (Timeout)");
    ui->statusConnect->setStyleSheet("font-weight: bold; color: red;");
}


