#ifndef LITTLEBOXTHREAD_H
#define LITTLEBOXTHREAD_H

#include <QThread>
#include <QTcpSocket>

class LittleBoxThread : public QThread
{
    Q_OBJECT

    int socketDescriptor;

public:
    explicit LittleBoxThread(int socketDescriptor, QObject *parent);

    void run();

signals:
    void error(QTcpSocket::SocketError socketError);

public slots:

};

#endif // LITTLEBOXTHREAD_H
