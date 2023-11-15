#include "filter.h"
#include "ui_filter.h"
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>
#include <nozzlehelper.h>

Filter::Filter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Filter)
{
    ui->setupUi(this);
    this->setWindowTitle("Báo Cáo");
    for(int i = 0;i < NOZZLE_NUM;i++){
        ui->nozzleID->addItem(QString::number(i));
    }
    ui->nozzleID->addItem("*");
    ui->dateTimeBegin->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeFinish->setDateTime(QDateTime::currentDateTime());
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

void Filter::on_pushQuery_clicked()
{
    static QSqlTableModel *model_1 = new QSqlTableModel;
    static QSqlTableModel *model_2 = new QSqlTableModel;
    QSqlQuery query1;
    QSqlQuery query2;
    QString qry_cmd2;
    QString qry_cmd;
    qry_cmd2.append("SELECT * FROM LogRS232 where [Thời gian] between '");
    qry_cmd2.append(ui->dateTimeBegin->dateTime().toString("dd/MM/yyyy hh:mm:ss"));
    qry_cmd2.append("' and '");
    qry_cmd2.append(ui->dateTimeFinish->dateTime().toString("dd/MM/yyyy hh:mm:ss"));
    qry_cmd2.append("'");
    /**************************************/
    qry_cmd.append("SELECT ID,sum(Disconnect) as [Số lần mất kết nối] ,sum(Startup) as [Số lần khởi động],sum(MissLog) as [Số lần mất log] from err_log WHERE Time BETWEEN '");
    qry_cmd.append(ui->dateTimeBegin->dateTime().toString("dd/MM/yyyy hh:mm:ss"));
    qry_cmd.append("' and '");
    qry_cmd.append(ui->dateTimeFinish->dateTime().toString("dd/MM/yyyy hh:mm:ss"));
    qry_cmd.append("'");

//    qry_cmd.append("select ID,Status,Count(*) as Count from LogRS232 where Time between '");
//    qry_cmd.append(ui->dateTimeBegin->dateTime().toString());
//    qry_cmd.append("' and '");
//    qry_cmd.append(ui->dateTimeFinish->dateTime().toString());
//    qry_cmd.append("'");
    if(ui->nozzleID->currentText() == "*"){
        qry_cmd.append(" group by ID");
//        qry_cmd.append(" group by ID,Status");
    }else{
        qry_cmd.append(" and ID = ");
        qry_cmd.append(ui->nozzleID->currentText());
//        qry_cmd.append(" and ID = ");
//        qry_cmd.append(ui->nozzleID->currentText());
//        qry_cmd.append(" group by Status");
        qry_cmd2.append(" and ID = ");
        qry_cmd2.append(ui->nozzleID->currentText());
    }
    query1.prepare(qry_cmd);
    if (!query1.exec(qry_cmd)) {
        qDebug() << "1Insert failed:" << query1.lastError();
    }
    model_1->setQuery(std::move(query1));
    model_1->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model_1->select();
    ui->total->setModel(model_1);
    ui->total->verticalHeader()->setVisible(false);
//    ui->total->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->total->resizeColumnsToContents();
    ui->total->show();
    query2.prepare(qry_cmd2);
    if (!query2.exec(qry_cmd2)) {
        qDebug() << "2Insert failed:" << query2.lastError();
    }
    model_2->setQuery(std::move(query2));
    model_2->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model_2->select();
    ui->logAfterFilter->setModel(model_2);
    ui->logAfterFilter->resizeColumnsToContents();
    ui->logAfterFilter->show();
}
//SELECT ID,sum(Disconnect),sum(Startup),sum(MissLog) from err_log WHERE Time BETWEEN 'Tue Nov 14 20:43:44 2023' AND 'Tue Nov 14 20:44:15 2023' GROUP BY ID
