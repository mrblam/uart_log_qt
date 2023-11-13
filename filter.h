#ifndef FILTER_H
#define FILTER_H

#include <QWidget>

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
private:
    Ui::Filter *ui;
};

#endif // FILTER_H
