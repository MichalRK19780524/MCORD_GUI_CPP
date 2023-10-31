//#include "lanconnection.h"
//#include "widget.h"
#include "wizard.h"

#include <QApplication>
#include <QThread>
#include <QFile>

QFile* logFile = new QFile;

void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg){
    QString log = QObject::tr("%1 | %2 | %3 | %4 | %5 | %6\n").
                  arg(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss")).
                  arg(context.line).
                  arg(QString(context.file).
                      section('\\', -1)).			// File name without file path
                  arg(QString(context.function).
                      section('(', -2, -2).		// Function name only
                      section(' ', -1).
                      section(':', -1)).
                  arg(msg);

    logFile->write(log.toLocal8Bit());
    logFile->flush();
}

int main(int argc, char *argv[])
{

    logFile->setFileName("./MyLog.log");
    logFile->open(QIODevice::Append | QIODevice::Text);
    qInstallMessageHandler(messageOutput);
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
