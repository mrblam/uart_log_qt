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

void LocalDB::insertDb()
{

}
