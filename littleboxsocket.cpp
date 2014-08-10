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

    QStringList components = QString(request).split(QRegExp("\r\n\r\n"));

    QString headers = components[0];

    QString body = components[1];

    QStringList lines = headers.split(QRegExp("\r\n"));

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
             << parameter;

    this->close();

//    this->disconnectFromHost();
//    this->waitForDisconnected();
}
