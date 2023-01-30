#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QSerialPort>

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

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    void showDetectonSlabs(QString labelName, Connection connection);

private slots:
    void nextClicked();
    void backClicked();
    void disconnectClicked();
    void disconnected();
    void connectionError(QAbstractSocket::SocketError se);
    void slubNumberSelection();
    void detectionSlabsBackClicked();
    void addSlab();
//    void socketReadySet();

private:
    Ui::Widget *ui;
    QTcpSocket *socket;
    QSerialPort *serial;
//    bool socketReady;



    static constexpr quint16 PORT = 5555;
    static constexpr quint16 CONNECTING_TIME = 1000;
    static constexpr quint16 READ_READY_LAN_TIME = 5000;
    static constexpr quint16 READ_READY_SERIAL_TIME = 5000;
    static const QString HUB_RESPONSE;
    static const QString LAN_CONNECTION_LABEL_TEXT;
    static const QString USB_CONNECTION_LABEL_TEXT;
    static const QJsonArray CLOSE;


};
#endif // WIDGET_H
