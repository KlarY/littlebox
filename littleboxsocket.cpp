#include "littleboxsocket.h"

LittleBoxSocket::LittleBoxSocket()
{
    connect(this, SIGNAL(readyRead()), this, SLOT(parseRequest()), Qt::DirectConnection);
    connect(this, SIGNAL(disconnected()), this, SLOT(deleteLater()), Qt::DirectConnection);
}

void LittleBoxSocket::parseRequest()
{
    QByteArray request = this->readAll();

    qDebug() << request;
}

void LittleBoxSocket::sendResponse()
{

}
