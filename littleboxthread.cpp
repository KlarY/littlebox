#include "littleboxthread.h"
#include "littleboxsocket.h"

LittleBoxThread::LittleBoxThread(int socketDescriptor, QObject* parent) : socketDescriptor(socketDescriptor), QThread(parent)
{
}

void LittleBoxThread::run()
{
    qDebug() << __FUNCTION__;

    LittleBoxSocket socket;

    if(!socket.setSocketDescriptor(socketDescriptor))
    {
        qDebug() << socket.errorString();

        return;
    }
}
