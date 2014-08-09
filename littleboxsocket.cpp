#include "littleboxsocket.h"

LittleBoxSocket::LittleBoxSocket()
{
    connect(this, SIGNAL(readyRead()), this, SLOT(parseRequest()), Qt::DirectConnection);
    connect(this, SIGNAL(disconnected()), this, SLOT(deleteLater()), Qt::DirectConnection);
}

void LittleBoxSocket::parseRequest()
{
    QStringList lines;

    while(this->canReadLine())
    {
        lines.append(this->readLine());
    }

    qDebug() << lines;

    close();
}

void LittleBoxSocket::sendResponse()
{

}
