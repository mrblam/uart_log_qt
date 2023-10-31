#ifndef LOCALDB_H
#define LOCALDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QDebug>
#include <logrecord.h>

class LocalDB : public QObject
{
    Q_OBJECT
public:
    explicit LocalDB();
    LocalDB* getLocalDB();
    void createDb (LogRecord* record);
private:
    QSqlDatabase database;
};

#endif // LOCALDB_H
