#ifndef SCADA_H
#define SCADA_H

#include <QWidget>
#include <QDateTime>

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
    void prepareData(int id_nozzle);
    static Scada* getScada();

private:
    Ui::Scada *ui;
    QDateTime time;
};

#endif // SCADA_H
