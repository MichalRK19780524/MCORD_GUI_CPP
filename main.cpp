#include "lanconnection.h"
#include "widget.h"

#include <QApplication>
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTcpSocket* s = new QTcpSocket();
    LanConnection* lc = new LanConnection(s);
//    LanConnection* lc = new LanConnection();
    Widget w(lc);
    QThread *lanConnectionThread = new QThread();
    s->moveToThread(lanConnectionThread);
    lc->moveToThread(lanConnectionThread);
    lanConnectionThread->start();
    w.show();
    return a.exec();
}
