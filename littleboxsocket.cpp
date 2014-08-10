#include "littleboxsocket.h"

LittleBoxSocket::LittleBoxSocket()
{
    connect(this, SIGNAL(readyRead()), this, SLOT(parseRequest()), Qt::DirectConnection);
//    connect(this, SIGNAL(disconnected()), this, SLOT(deleteLater()), Qt::DirectConnection);
}

void LittleBoxSocket::parseRequest()
{
    qDebug() << __TIME__ << "socket" << this->socketDescriptor() << "serving...";

    this->waitForReadyRead();

    QByteArray request = this->readAll();

    QStringList componets = QString(request).split(QRegExp("\r\n\r\n"));

    QString headers = componets[0];

    QString body = componets[1];

    qDebug() << headers;

    qDebug() << body;

    qDebug() << componets.size();

//    for(QString componet : componets)
//    {
//        qDebug() << componet;
//    }

    QStringList lines = headers.split(QRegExp("[ \r\n][ \r\n]*")/*, QString::SkipEmptyParts*/);

    for(QString line : lines)
    {
        qDebug() << line;
    }

    sendResponse(body);
}

void LittleBoxSocket::sendResponse(QString parameter)
{
    QTextStream response(this);

    response.setAutoDetectUnicode(true);

    response << "HTTP/1.1 200 OK\r\n"
             << "content-type: text/html; charset=\"utf-8\"\r\n"
             << "\r\n"
             << parameter
             << this->socketDescriptor();
    this->close();
//    this->disconnectFromHost();
//    this->waitForDisconnected();
}
