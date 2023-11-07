#ifndef SCADA_H
#define SCADA_H

#include <QWidget>
#include <QDateTime>
#include <QStandardItemModel>

namespace Ui {
class Scada;
}

class Scada : public QWidget
{
    Q_OBJECT

public:
    explicit Scada(QWidget *parent = nullptr);
    ~Scada();
    void updateLog();
    void prepareData(uint8_t id_nozzle);
    static Scada* getScada();
public slots:
    void updateErr(uint8_t id_nozzle);
private:
    Ui::Scada *ui;
    QStandardItemModel *model;
};

#endif // SCADA_H
