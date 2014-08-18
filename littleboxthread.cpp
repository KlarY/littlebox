#include "littleboxthread.h"
#include "littleboxsocket.h"

LittleBoxThread::LittleBoxThread(int socketDescriptor, QObject* parent) : QThread(parent), socketDescriptor(socketDescriptor)
{
}

void LittleBoxThread::run()
{
    LittleBoxSocket socket;

    if(!socket.setSocketDescriptor(socketDescriptor))
    {
        qDebug() << socket.errorString();

        return;
    }

    exec();
}
