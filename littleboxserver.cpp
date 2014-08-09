#include "littleboxserver.h"
#include "littleboxthread.h"

LittleBoxServer::LittleBoxServer(QObject* parent) : QTcpServer(parent)
{
}

void LittleBoxServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << __FUNCTION__;

    LittleBoxThread* thread = new LittleBoxThread(socketDescriptor, this);

    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}
