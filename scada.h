#ifndef SCADA_H
#define SCADA_H

#include <QWidget>
#include <QDateTime>
#include <QStandardItemModel>
#include <nozzlehelper.h>
#include <QTimer>


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
public slots:
    void updateNozzleData(NozzleMessage &data);
private:
    Ui::Scada *ui;
    QStandardItemModel *model;
    QTimer heartbeatTicker;
};

#endif // SCADA_H
