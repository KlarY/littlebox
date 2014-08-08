#include <QCoreApplication>
#include <QDebug>

#include "littleboxserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    LittleBoxServer server;

    if(!server.listen(QHostAddress::Any, 2048))
    {
        qDebug() << "unable to start the server : " + server.errorString() + ".";

        exit(1);
    }

    qDebug() << "IP : " << server.serverAddress() << '\n'
             << "Port : " << server.serverPort() << '\n';

    return a.exec();
}
