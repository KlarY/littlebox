#include <QCoreApplication>
#include <QDebug>

#include "littleboxserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    LittleBoxServer server;

    if(!server.listen(QHostAddress::AnyIPv4, 4096))
    {
        qDebug() << "unable to start the server : " + server.errorString() + ".";

        exit(1);
    }

    qDebug() << __TIME__ << "the server is running...";

    return a.exec();
}
