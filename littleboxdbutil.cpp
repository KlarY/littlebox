#include "littleboxdbutil.h"

LittleBoxDBUtil::LittleBoxDBUtil(QObject *parent) : QObject(parent)
{
    database = QSqlDatabase::addDatabase("QMYSQL");

    database.setDatabaseName("littlebox");

    database.setHostName("localhost");

    database.setUserName("root");

    database.setPassword("");

    if(!database.open())
    {
        qDebug() << database.lastError().text();

        exit(1);
    }

    query = QSqlQuery();
}

QSqlQuery LittleBoxDBUtil::execute(QString sql)
{
    query.exec(sql);

    if(!query.isActive())
    {
        qDebug() << query.lastError().text();

        qDebug() << query.lastQuery();
    }

    return query;
}
