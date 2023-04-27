#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QSerialPort>
#include <QStandardItemModel>
#include <QStringList>
#include <QSignalMapper>

//#include "detectionslabswidget.h"
#include "lanconnection.h"
#include "detectortablemodel.h"

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

    void addPowerWidgets();

    static constexpr int SET_COLUMN_INDEX = 3;
    static constexpr int POWER_COLUMN_INDEX = 4;

private:
    void showDetectonSlabs(QString labelName, Connection connection);
    QString appendSlabToModel();
    QString initAndOnSlab();
    QString reloadMasterSlabToModel(Slab* slab);
    QString reloadSlaveSlabToModel(Slab* slab);

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
    void addAndOnSlab();

    void setMasterVoltageClicked(int slabId);
    void setSlaveVoltageClicked(int slabId);

    void onMasterClicked(int slabId);
    void onSlaveClicked(int slabId);

    void offMasterClicked(int slabId);
    void offSlaveClicked(int slabId);

private:
    Ui::Widget *ui;

    LanConnection *lanConnection = nullptr;
    QSerialPort *serial = nullptr;
//    DetectionSlabsWidget *detectionSlabsWidget = nullptr;
    DetectorTableModel *model = nullptr;
    QSignalMapper *setMasterSignalMapper = nullptr;
    QSignalMapper *setSlaveSignalMapper = nullptr;
    QSignalMapper *onMasterSignalMapper = nullptr;
    QSignalMapper *onSlaveSignalMapper = nullptr;
    QSignalMapper *offMasterSignalMapper = nullptr;
    QSignalMapper *offSlaveSignalMapper = nullptr;
    State state = State::DISCONNECTED;

    static constexpr quint16 PORT = 5555;
    static constexpr quint16 READ_READY_SERIAL_TIME = 5000;
    static const QString HUB_RESPONSE;
    static const QString LAN_CONNECTION_LABEL_TEXT;
    static const QString USB_CONNECTION_LABEL_TEXT;
    static const QStringList HEADERS;
    static const QJsonArray CLOSE;
    void addSetWidgets();
};
#endif // WIDGET_H
