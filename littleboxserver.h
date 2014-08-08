#ifndef LITTLEBOXSERVER_H
#define LITTLEBOXSERVER_H

#include <QTcpServer>

class LittleBoxServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit LittleBoxServer(QObject *parent = 0);

protected:
    void incomingConnection(qintptr socketDescriptor);

signals:

public slots:

};

#endif // LITTLEBOXSERVER_H
