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

void Filter::initListNozzle(Nozzle *list, uint8_t num)
{
    nozzleNum = num;
    nozzlePtr = list;
    for(int i = 0;i < nozzleNum;i++){
        ui->nozzleID->addItem(nozzlePtr[i].getName());
    }
    ui->nozzleID->addItem("*");
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
    qry_cmd.append("SELECT Vòi,sum(Disconnect) as [Số lần mất \nkết nối] ,sum(Startup) as [Số lần \nkhởi động],sum(MissLog) as [Số lần \nmất log] from err_log WHERE Time BETWEEN '");
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
        qry_cmd.append(" group by Vòi");
//        qry_cmd.append(" group by ID,Status");
    }else{
        qry_cmd.append(" and Vòi = ");
        qry_cmd.append("'");
        qry_cmd.append(ui->nozzleID->currentText());
        qry_cmd.append("'");
//        qry_cmd.append(" and ID = ");
//        qry_cmd.append(ui->nozzleID->currentText());
//        qry_cmd.append(" group by Status");
        qry_cmd2.append(" and Vòi = ");
        qry_cmd2.append("'");
        qry_cmd2.append(ui->nozzleID->currentText());
        qry_cmd2.append("'");
    }
    query1.prepare(qry_cmd);
    qDebug()<< qry_cmd;
    if (!query1.exec(qry_cmd)) {
        qDebug() << "1Insert failed:" << query1.lastError();
    }
    model_1->setQuery(std::move(query1));
    model_1->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model_1->select();
    ui->total->setModel(model_1);
    ui->total->verticalHeader()->setVisible(false);
    ui->total->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//    ui->total->resizeColumnsToContents();
    ui->total->show();
    query2.prepare(qry_cmd2);
    qDebug()<< qry_cmd2;
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

void Filter::on_exportPDF_clicked()
{
//    QTextDocument *doc = new QTextDocument;
//    doc->setDocumentMargin(10);
//    QTextCursor cursor(doc);

//    cursor.movePosition(QTextCursor::Start);

//    QTextTable *table = cursor.insertTable(properties.size() + 1, 2, tableFormat);
//    QTextTableCell headerCell = table->cellAt(0, 0);
//    QTextCursor headerCellCursor = headerCell.firstCursorPosition();
//    headerCellCursor.insertText(QObject::tr("Name"), boldFormat);
//    headerCell = table->cellAt(0, 1);
//    headerCellCursor = headerCell.firstCursorPosition();
//    headerCellCursor.insertText(QObject::tr("Value"), boldFormat);

//    for(int i = 0; i < properties.size(); i++){
//        QTextCharFormat cellFormat = i % 2 == 0 ? textFormat : alternateCellFormat;
//        QTextTableCell cell = table->cellAt(i + 1, 0);
//        cell.setFormat(cellFormat);
//        QTextCursor cellCursor = cell.firstCursorPosition();
//        cellCursor.insertText(properties.at(i)->name());

//        cell = table->cellAt(i + 1, 1);
//        cell.setFormat(cellFormat);
//        cellCursor = cell.firstCursorPosition();
//        cellCursor.insertText(properties.at(i)->value().toString() + " " + properties.at(i)->unit());
//    }

//    cursor.movePosition(QTextCursor::End);
//    cursor.insertBlock();

//    //Print to PDF
//    QPrinter printer(QPrinter::HighResolution);
//    printer.setOutputFormat(QPrinter::PdfFormat);
//    printer.setOutputFileName(filename);
//    doc->print(&printer);

}

