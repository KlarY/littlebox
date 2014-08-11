#include "littleboxdbutil.h"

LittleBoxDBUtil::LittleBoxDBUtil(QObject *parent) : QObject(parent)
{
    db_connection = QSqlDatabase::addDatabase("QMYSQL");

    db_connection.setDatabaseName("littlebox");

    db_connection.setHostName("localhost");

    db_connection.setUserName("root");

    db_connection.setPassword("");

    if(!db_connection.open())
    {
        qDebug() << db_connection.lastError().text();

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
