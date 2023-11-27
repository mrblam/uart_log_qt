#ifndef SCADA_H
#define SCADA_H

#include <QWidget>
#include <QDateTime>
#include <QStandardItemModel>
#include <nozzlehelper.h>
#include <QTimer>
#include "nozzle.h"

namespace Ui {
class Scada;
}

class Scada : public QWidget
{
    Q_OBJECT

public:
    explicit Scada(QWidget *parent = nullptr);
    ~Scada();
    void updateScada();
    static Scada* getScada();
    nozzle2D* getNozzle();
    void initListNozzle(Nozzle *list,uint8_t num);
public slots:
    void updateNozzleData(NozzleMessage &data);
    void setDisconnectToMCU();
private:
    Ui::Scada *ui;
    QStandardItemModel *model;
    QTimer heartbeatTicker;
    Nozzle *nozzlePtr;
    uint8_t nozzleNum;
};

#endif // SCADA_H
