#ifndef SERIALRECEIVERTHREAD_H
#define SERIALRECEIVERTHREAD_H

#include <QThread>

class SerialReceiverThread : public QThread
{
    Q_OBJECT
public:
    explicit SerialReceiverThread(QObject *parent = nullptr);
};

#endif // SERIALRECEIVERTHREAD_H
