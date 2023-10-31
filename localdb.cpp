#include "localdb.h"


LocalDB::LocalDB()
{
    database = QSqlDatabase::addDatabase("QSQLITE");
//    database.setDatabaseName("/home/hoan/"+nameDatabase+".db");
    if(!database.open()) qDebug() << "cannot careate new database";
}

LocalDB* LocalDB::getLocalDB()
{
    static LocalDB* self;
    if(self==nullptr){
        self=new LocalDB();
    }
    return self;
}

void LocalDB::createDb(LogRecord *record)
{
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("/home/hoanpx/"+ record->getNameTable());
    if(!database.open())
    {
        qDebug() << "cannot create new:" <<record->getNameTable();
    }
    else qDebug() << "created new:" << record->getNameTable();
}
