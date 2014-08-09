#include <QCoreApplication>
#include <QDebug>

#include "littleboxserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    LittleBoxServer server;

    if(!server.listen(QHostAddress::LocalHost, 2048))
    {
        qDebug() << "unable to start the server : " + server.errorString() + ".";

        exit(1);
    }

    qDebug() << "the server is running...";

    return a.exec();
}
