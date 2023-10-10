//#include "lanconnection.h"
//#include "widget.h"
#include "wizard.h"

#include <QApplication>
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    LanConnection* lc = new LanConnection();
//    Widget w(lc);
    Wizard w;
//    QThread *lanConnectionThread = new QThread();
//    s->moveToThread(lanConnectionThread);
//    lc->moveToThread(lanConnectionThread);
//    lanConnectionThread->start();
    w.setWizardStyle(QWizard::ModernStyle);
    w.show();
    return a.exec();
}
