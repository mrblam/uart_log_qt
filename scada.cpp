#include "scada.h"
#include "ui_scada.h"
#include "logrecord.h"
#include "nozzlehelper.h"


static nozzle nozzle_arr[NOZZLE_NUM];

Scada::Scada(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Scada)
{
    ui->setupUi(this);
    this->setWindowTitle("SCADA");
    QStringList horzHeaders;
    ui->tableWidget->setRowCount(NOZZLE_NUM);
    ui->tableWidget->setColumnCount(8);
    horzHeaders << "Nozzle" << "Time" << "Disconnect" << "Lost Log" << "Shut Down" <<  "Total Money" << "Liter" << "Unit Price";
    ui->tableWidget->setHorizontalHeaderLabels(horzHeaders);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->verticalHeader()->setVisible(false);
    for (int i = 0;i < NOZZLE_NUM;i++){
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i, 0),i,Qt::EditRole);
    }
//    ui->tableWidget->setItem(0, 0, new QTableWidgetItem("0"));
#if 0
    model = new QStandardItemModel(NOZZLE_NUM,9,this);
        ui->tableView->setModel(model);

    // Generate data
    for(int row = 0; row < NOZZLE_NUM; row++)
    {
        for(int col = 0; col < 9; col++)
        {
            QModelIndex index
                = model->index(row,col,QModelIndex());
            // 0 for all data
            model->setData(index,0);
        }
    }
#endif
    updateLog();
}

Scada::~Scada()
{
    delete ui;
}

void Scada::updateLog()
{
    uint8_t id = 0;
//    QTableWidgetItem *item = new QTableWidgetItem();
    id = LogRecord::getRecord()->getId();
    prepareData(id);
    for(int i = 0;i < NOZZLE_NUM;i++){
#if 1
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i, 1),nozzle_arr[i].time,Qt::EditRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i, 2),nozzle_arr[i].disconnect,Qt::EditRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i, 3),nozzle_arr[i].lostLog,Qt::EditRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i, 4),nozzle_arr[i].shutdown,Qt::EditRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i, 5),nozzle_arr[i].totalMoney,Qt::EditRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i, 6),nozzle_arr[i].liter,Qt::EditRole);
        ui->tableWidget->model()->setData(ui->tableWidget->model()->index(i, 7),nozzle_arr[i].unitPrice,Qt::EditRole);

#else
        item->setData(Qt::DisplayRole,nozzle_arr[i].totalMoney);
        ui->tableWidget->setItem(i,1,item);
        item->setData(Qt::DisplayRole,nozzle_arr[i].liter);
        ui->tableWidget->setItem(i,2,item);
        item->setData(Qt::DisplayRole,nozzle_arr[i].unitPrice);
        ui->tableWidget->setItem(i,3,item);
        item->setData(Qt::DisplayRole,nozzle_arr[i].disconnect);
        ui->tableWidget->setItem(i,4,item);
        item->setData(Qt::DisplayRole,nozzle_arr[i].lostLog);
        ui->tableWidget->setItem(i,5,item);
        item->setData(Qt::DisplayRole,nozzle_arr[i].shutdown);
        ui->tableWidget->setItem(i,6,item);
        item->setData(Qt::DisplayRole,time);
        ui->tableWidget->setItem(i,7,item);
        ui->tableWidget->update();
#endif

#if 0
        // Generate data
        for(int row = 0; row < NOZZLE_NUM; row++)
        {
            for(int col = 0; col < 9; col++)
            {
                QModelIndex index
                    = model->index(row,col,QModelIndex());
                // 0 for all data
                model->setData(index,nozzle_arr[5].unitPrice);
                emit model->dataChanged(index,index);
            }
        }
//        emit model->dataChanged(index,index);
#endif
    }
}

void Scada::prepareData(uint8_t id_nozzle)
{
    char* buff;
    QString stringBuff;
    buff = LogRecord::getRecord()->getMoney_1();
    stringBuff.append(QString::number((int)(buff[0])));
    stringBuff.append(QString::number((int)(buff[1])));
    stringBuff.append(QString::number((int)(buff[2])));
    stringBuff.append(QString::number((int)(buff[3])));
    stringBuff.append(QString::number((int)(buff[4])));
    stringBuff.append(QString::number((int)(buff[5])));
    stringBuff.append(QString::number((int)(buff[6])));
    stringBuff.append(QString::number((int)(buff[7])));
    nozzle_arr[id_nozzle].totalMoney = stringBuff.toLongLong();
    stringBuff.clear();
    buff = LogRecord::getRecord()->getUnitPrice_1();
    stringBuff.append(QString::number((int)(buff[0])));
    stringBuff.append(QString::number((int)(buff[1])));
    stringBuff.append(QString::number((int)(buff[2])));
    stringBuff.append(QString::number((int)(buff[3])));
    stringBuff.append(QString::number((int)(buff[4])));
    stringBuff.append(QString::number((int)(buff[5])));
    nozzle_arr[id_nozzle].unitPrice = stringBuff.toLongLong();
    stringBuff.clear();
    buff = LogRecord::getRecord()->getLiter_1();
    stringBuff.append(QString::number((int)(buff[0])));
    stringBuff.append(QString::number((int)(buff[1])));
    stringBuff.append(QString::number((int)(buff[2])));
    stringBuff.append(QString::number((int)(buff[3])));
    stringBuff.append(QString::number((int)(buff[4])));
    stringBuff.append(QString::number((int)(buff[5])));
    stringBuff.append(QString::number((int)(buff[6])));
    stringBuff.append(QString::number((int)(buff[7])));
    nozzle_arr[id_nozzle].liter = stringBuff.toLongLong();
}

Scada *Scada::getScada()
{
    static Scada* self;
    if(self==nullptr){
        self=new Scada();
    }
    return self;
}

void Scada::updateErr(uint8_t id_nozzle)
{
    QDateTime time_current = QDateTime::currentDateTime();
    nozzle_arr[id_nozzle].time = time_current.toString();
//    if(LogRecord::getRecord()->getSeqence())
        if ((int)(LogRecord::getRecord()->getStatus()) == 1){
            nozzle_arr[id_nozzle].lostLog++;
        }else if((int)(LogRecord::getRecord()->getStatus()) == 2){
            nozzle_arr[id_nozzle].disconnect++;
        }else if((int)(LogRecord::getRecord()->getStatus()) == 3){
            nozzle_arr[id_nozzle].shutdown++;
        }
}
