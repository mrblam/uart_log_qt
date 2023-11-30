#ifndef FILTER_H
#define FILTER_H

#include <QWidget>
#include "nozzle.h"

namespace Ui {
class Filter;
}

class Filter : public QWidget
{
    Q_OBJECT

public:
    explicit Filter(QWidget *parent = nullptr);
    ~Filter();
    static Filter *getFilter();
    void initListNozzle(Nozzle *list,uint8_t &num);
private slots:
    void on_pushQuery_clicked();

private:
    Ui::Filter *ui;
    Nozzle *nozzlePtr;
    uint8_t nozzleNum;
};

#endif // FILTER_H
