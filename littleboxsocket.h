#ifndef LITTLEBOXSOCKET_H
#define LITTLEBOXSOCKET_H

#include <QTcpSocket>

class LittleBoxSocket : public QTcpSocket
{
    Q_OBJECT

    void sendResponse();

public:
    LittleBoxSocket();

signals:

private slots:
    void parseRequest();

};

#endif // LITTLEBOXSOCKET_H
