#include "littleboxsocket.h"

LittleBoxSocket::LittleBoxSocket()
{
    connect(this, SIGNAL(readyRead()), this, SLOT(parseRequest()), Qt::DirectConnection);
//    connect(this, SIGNAL(disconnected()), this, SLOT(deleteLater()), Qt::DirectConnection);
}

void LittleBoxSocket::parseRequest()
{
    qDebug() << __TIME__ << "socket" << this->socketDescriptor() << "serving...";

    this->waitForReadyRead(3000);

    QByteArray request = this->readAll();

    //============================================================================

    QStringList components = QString(request).split(QRegExp("\r\n\r\n"));

    QString headers = components[0];

    QString body = components[1];

    //============================================================================

    QStringList lines = headers.split(QRegExp("\r\n"));

    //============================================================================

    QStringList tokens = lines[0].split(' ');

    if("POST" == tokens[0])
    {
        sendResponse(tokens[1].remove(0, 1), body);
    }
    else
    {
        sendResponse("display_error_msg\r\n", request);
    }
}

void LittleBoxSocket::sendResponse(QString method, QString parameter)
{
    QTextStream response(this);

    response.setAutoDetectUnicode(true);

    response << "HTTP/1.1 200 OK\r\n"
             << "content-type: text/html; charset=\"utf-8\"\r\n"
             << "\r\n"
             << method << parameter;

    this->close();

//    this->disconnectFromHost();
//    this->waitForDisconnected();
}
