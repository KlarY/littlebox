#include "littleboxthread.h"

#include <QtNetwork>

LittleBoxThread::LittleBoxThread(int socketDescriptor, QObject *parent)
    : QThread(parent), socketDescriptor(socketDescriptor)
{
}

void LittleBoxThread::run()
{
    QTcpSocket tcpSocket;

    if(!tcpSocket.setSocketDescriptor(socketDescriptor))
    {
        emit error(tcpSocket.error());

        return;
    }

    QByteArray response;

    QDataStream out(&response, QIODevice::WriteOnly);

    out.setVersion(QDataStream::Qt_5_3);

    out << quint16(0);

    out << "Hello, LittleBox.";

    out.device()->seek(0);

    out << quint16(response.size() - sizeof(quint16));

    tcpSocket.write(response);

    tcpSocket.disconnectFromHost();
    tcpSocket.waitForDisconnected();
}
