#include "lanconnection.h"
#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTcpSocket* s = new QTcpSocket();
    LanConnection* lc = new LanConnection(s);
    Widget w(lc);
    w.show();
    return a.exec();
}
