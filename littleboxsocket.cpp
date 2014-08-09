#include "littleboxsocket.h"

LittleBoxSocket::LittleBoxSocket()
{
    qDebug() << __FUNCTION__;

    connect(this, SIGNAL(readyRead()), this, SLOT(parseRequest()));
}

void LittleBoxSocket::parseRequest()
{
    qDebug() << __FUNCTION__;

    QStringList lines;

    while(this->canReadLine())
    {
        lines.append(this->readLine());
    }

    qDebug() << lines;
}

void LittleBoxSocket::sendResponse()
{

}
