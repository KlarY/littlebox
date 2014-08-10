#ifndef LITTLEBOXSOCKET_H
#define LITTLEBOXSOCKET_H

#include <QTcpSocket>

class LittleBoxDBUtil;

class LittleBoxSocket : public QTcpSocket
{
    Q_OBJECT

    LittleBoxDBUtil* dbWorker;

    void sendResponse(QString, QString);

public:
    LittleBoxSocket();

signals:

private slots:
    void parseRequest();

};

#endif // LITTLEBOXSOCKET_H
