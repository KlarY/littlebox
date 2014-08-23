#ifndef LITTLEBOXDBUTIL_H
#define LITTLEBOXDBUTIL_H

#include <QDebug>

#include <QObject>

#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDatabase>

#include <QElapsedTimer>

class LittleBoxDBUtil : public QObject
{
    Q_OBJECT

    QElapsedTimer timer;

    QSqlDatabase database;

    QSqlQuery query;

public:
    explicit LittleBoxDBUtil(QObject *parent = 0);

    QSqlQuery execute(QString);

signals:

public slots:

};

#endif // LITTLEBOXDBUTIL_H
