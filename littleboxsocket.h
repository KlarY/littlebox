#ifndef LITTLEBOXSOCKET_H
#define LITTLEBOXSOCKET_H

#include <QTcpSocket>

class LittleBoxDBUtil;

class LittleBoxSocket : public QTcpSocket
{
    Q_OBJECT

    LittleBoxDBUtil* database;

    void sendResponse(QString, QString);

    bool authenticate(int, QString);

    void dealWithSignUP(QJsonObject);
    void dealWithSignIn(QJsonObject);

public:
    LittleBoxSocket();

signals:

private slots:
    void parseRequest();

};

#endif // LITTLEBOXSOCKET_H
