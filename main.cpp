#include <QCoreApplication>
#include <QDebug>

#include "littleboxserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    LittleBoxServer server;

    if(!server.listen())
    {
        qDebug() << "unable to start the server : " + server.errorString() + ".";

        exit(1);
    }

    qDebug() << "the server is running...";

    qDebug() << "address :" << server.serverAddress().toString();

    qDebug() << "port :" << server.serverPort();

    return a.exec();
}
