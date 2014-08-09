#ifndef LITTLEBOXTHREAD_H
#define LITTLEBOXTHREAD_H

#include <QThread>

class LittleBoxThread : public QThread
{
    Q_OBJECT

    int socketDescriptor;

public:
    explicit LittleBoxThread(int socketDescriptor, QObject* parent);

    void run();

signals:

public slots:

};

#endif // LITTLEBOXTHREAD_H
