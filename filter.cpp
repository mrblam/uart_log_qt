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

void Filter::initListNozzle(Nozzle *list, uint8_t &num)
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
    static QSqlTableModel *modelErrLog = new QSqlTableModel;
    static QSqlTableModel *modelLogRS232 = new QSqlTableModel;
    QSqlQuery queryErrLog;
    QSqlQuery queryLogRS232;
    QString qryCmdLogRS232;
    QString qryCmdErrLog;
    qryCmdLogRS232.append("SELECT * FROM LogRS232 where [Thời gian] between '");
    qryCmdLogRS232.append(ui->dateTimeBegin->dateTime().toString("dd/MM/yyyy hh:mm:ss"));
    qryCmdLogRS232.append("' and '");
    qryCmdLogRS232.append(ui->dateTimeFinish->dateTime().toString("dd/MM/yyyy hh:mm:ss"));
    qryCmdLogRS232.append("'");
    /**************************************/
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
    queryErrLog.prepare(qryCmdErrLog);
    qDebug()<< qryCmdErrLog;
    if (!queryErrLog.exec(qryCmdErrLog)) {
        qDebug() << "1Insert failed:" << queryErrLog.lastError();
    }
    modelErrLog->setQuery(std::move(queryErrLog));
    modelErrLog->setEditStrategy(QSqlTableModel::OnManualSubmit);
    modelErrLog->select();
    ui->total->setModel(modelErrLog);
    ui->total->verticalHeader()->setVisible(false);
    ui->total->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->total->show();
    queryLogRS232.prepare(qryCmdLogRS232);
    qDebug()<< qryCmdLogRS232;
    if (!queryLogRS232.exec(qryCmdLogRS232)) {
        qDebug() << "2Insert failed:" << queryLogRS232.lastError();
    }
    modelLogRS232->setQuery(std::move(queryLogRS232));
    modelLogRS232->setEditStrategy(QSqlTableModel::OnManualSubmit);
    modelLogRS232->select();
    ui->logAfterFilter->setModel(modelLogRS232);
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

