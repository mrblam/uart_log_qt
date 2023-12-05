#include "filter.h"
#include "ui_filter.h"
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>
#include <nozzlehelper.h>
#include <QTextDocument>
#include <QTextTable>
#include <QPrinter>

Filter::Filter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Filter)
{
    ui->setupUi(this);
    this->setWindowTitle("Báo Cáo");
    this->setWindowIcon(QIcon(":/UI/Icon/p.ico"));
    connect(this,&Filter::qryLogRS232,this,&Filter::showDbLogResult);
    connect(this,&Filter::qryErrLog,this,&Filter::showErrLogResult);
    ui->dateTimeBegin->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeBegin->setDisplayFormat("dd/MM/yyyy hh:mm:ss");
    ui->dateTimeFinish->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeFinish->setDisplayFormat("dd/MM/yyyy hh:mm:ss");
}

Filter::~Filter()
{
    delete ui;
}

Filter *Filter::getFilter()
{
    static Filter* self;
    if(self==nullptr){
        self=new Filter();
    }
    return self;
}

void Filter::initListNozzle(Nozzle *list, uint8_t &num)
{
    nozzleNum = num;
    nozzlePtr = list;
    ui->nozzleID->clear();
    for(int i = 0;i < nozzleNum;i++){
        ui->nozzleID->addItem(nozzlePtr[i].getName());
    }
    ui->nozzleID->addItem("*");
}

void Filter::on_pushQuery_clicked()
{
    QString qryCmdLogRS232;
    QString qryCmdErrLog;
    qryCmdLogRS232.clear();
    qryCmdLogRS232.append("SELECT * FROM LogRS232 where [Thời gian] between '");
    qryCmdLogRS232.append(ui->dateTimeBegin->dateTime().toString("dd/MM/yyyy hh:mm:ss"));
    qryCmdLogRS232.append("' and '");
    qryCmdLogRS232.append(ui->dateTimeFinish->dateTime().toString("dd/MM/yyyy hh:mm:ss"));
    qryCmdLogRS232.append("'");
    /**************************************/
    qryCmdErrLog.clear();
    qryCmdErrLog.append("SELECT Vòi,sum(Disconnect) as [Số lần mất \nkết nối] ,sum(Startup) as [Số lần \nkhởi động],sum(MissLog) as [Số lần \nmất log] from err_log WHERE Time BETWEEN '");
    qryCmdErrLog.append(ui->dateTimeBegin->dateTime().toString("dd/MM/yyyy hh:mm:ss"));
    qryCmdErrLog.append("' and '");
    qryCmdErrLog.append(ui->dateTimeFinish->dateTime().toString("dd/MM/yyyy hh:mm:ss"));
    qryCmdErrLog.append("'");
    if(ui->nozzleID->currentText() == "*"){
        qryCmdErrLog.append(" group by Vòi");
    }else{
        qryCmdErrLog.append(" and Vòi = ");
        qryCmdErrLog.append("'");
        qryCmdErrLog.append(ui->nozzleID->currentText());
        qryCmdErrLog.append("'");
        qryCmdLogRS232.append(" and Vòi = ");
        qryCmdLogRS232.append("'");
        qryCmdLogRS232.append(ui->nozzleID->currentText());
        qryCmdLogRS232.append("'");
    }
    emit qryErrLog(qryCmdErrLog);
    emit qryLogRS232(qryCmdLogRS232);
}

void Filter::showDbLogResult(QString qryCmdLogRS232)
{
    static QSqlTableModel *modelLogRS232 = new QSqlTableModel;
    QSqlQuery queryLogRS232;
    queryLogRS232.prepare(qryCmdLogRS232);
    qDebug()<< qryCmdLogRS232;
    if (!queryLogRS232.exec(qryCmdLogRS232)) {
        qDebug() << "Query LogRS232 failed:" << queryLogRS232.lastError();
    }
    modelLogRS232->setQuery(std::move(queryLogRS232));
    modelLogRS232->setEditStrategy(QSqlTableModel::OnManualSubmit);
    modelLogRS232->select();
    ui->logAfterFilter->setModel(modelLogRS232);
    ui->logAfterFilter->resizeColumnsToContents();
    ui->logAfterFilter->show();
}

void Filter::showErrLogResult(QString qryCmdErrLog)
{
    static QSqlTableModel *modelErrLog = new QSqlTableModel;
    QSqlQuery queryErrLog;
    queryErrLog.prepare(qryCmdErrLog);
    qDebug()<< qryCmdErrLog;
    if (!queryErrLog.exec(qryCmdErrLog)) {
        qDebug() << "Query ErrLog failed:" << queryErrLog.lastError();
    }
    modelErrLog->setQuery(std::move(queryErrLog));
    modelErrLog->setEditStrategy(QSqlTableModel::OnManualSubmit);
    modelErrLog->select();
    ui->total->setModel(modelErrLog);
    ui->total->verticalHeader()->setVisible(false);
    ui->total->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->total->show();
}
//SELECT ID,sum(Disconnect),sum(Startup),sum(MissLog) from err_log WHERE Time BETWEEN 'Tue Nov 14 20:43:44 2023' AND 'Tue Nov 14 20:44:15 2023' GROUP BY ID
