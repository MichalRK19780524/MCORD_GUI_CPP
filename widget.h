#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QSerialPort>
#include <QStandardItemModel>

#include "detectionslabswidget.h"
#include "lanconnection.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

    enum class Connection
    {
        SERIAL,
        LAN
    };

    enum class State
    {
        CONNECTION_SELECTION,
        LAN_SELECTED,
        LAN_CONNECTED,
        USB_SELECTED,
        USB_CONNECTED,
        DISCONNECTED,
        ONE_SLAB_LAN,
        ONE_SLAB_USB,
        ERROR
    };

public:
    Widget(LanConnection *lanConnection, QWidget *parent = nullptr);
    ~Widget();

private:
    void showDetectonSlabs(QString labelName, Connection connection);

private slots:
    void nextClicked();
    void backClicked();
    void disconnectClicked();
    void setVoltageClicked(QObject *button);
    void disconnected();
    void connectionError(QAbstractSocket::SocketError se);
    void slabNumberSelection();
    void detectionSlabsBackClicked();
    void addSlab();
//    void backSlabsChoiceClicked();
//    void socketReadySet();

private:
    Ui::Widget *ui;

    LanConnection *lanConnection;
    QSerialPort *serial = nullptr;
//    QTcpSocket *socket = nullptr;
    DetectionSlabsWidget *detectionSlabsWidget = nullptr;
    QStandardItemModel *model = nullptr;
//    bool socketReady;
    State state = State::DISCONNECTED;



    static constexpr quint16 PORT = 5555;
    static constexpr quint16 READ_READY_SERIAL_TIME = 5000;
    static const QString HUB_RESPONSE;
    static const QString LAN_CONNECTION_LABEL_TEXT;
    static const QString USB_CONNECTION_LABEL_TEXT;
    static const QJsonArray CLOSE;


};
#endif // WIDGET_H
