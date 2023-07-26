#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QSerialPort>
#include <QStandardItemModel>
#include <QStringList>
#include <QSignalMapper>
#include <QSettings>

//#include "detectionslabswidget.h"
#include "lanconnection.h"
#include "detectortablemodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget {
Q_OBJECT

    enum class Connection : size_t {
        SERIAL,
        LAN
    };

    enum class State : size_t {
        CONNECTION_SELECTION,
        LAN_SELECTED,
        LAN_CONNECTED,
        USB_SELECTED,
        USB_CONNECTED,
        DISCONNECTED,
        ONE_SLAB_LAN,
        ONE_SLAB_USB,
        LAN_SLAB_DETECTING,
        LAN_SLAB_INITIALIZING,
        ERROR
    };

    enum class SlabState : size_t {
        Detected,
        On,
        Off,
        Set,
        Error
    };

    inline size_t qHash(SlabState &key, uint seed) {
        return ::qHash(static_cast<size_t>(key), seed);
    }

public:
    Widget(LanConnection *lanConnection, QWidget *parent = nullptr);

    ~Widget();

    void addPowerWidgets();

    static constexpr int SET_COLUMN_INDEX = 3;
    static constexpr int POWER_COLUMN_INDEX = 4;

private:
    void showDetectonSlabs(QString labelName, Connection connection);
//    QString appendSlabToModel(int slabId);

//    QString initAndOnSlab(int slabId);
    QString reloadMasterSlabToModel(Slab *slab);

    QString reloadSlaveSlabToModel(Slab *slab);

signals:

    void connectLan(QString ipAddress, quint16 port);

    void closeLanConnection();

    void slabRequired(quint16 slabId, AfeType afeType);

    void slabUpdateRequired(quint16 slabId);

    void initializationRequired(quint16 slabId);



private slots:

    void nextClicked();

    void backClicked();

    void disconnectClicked();

    void disconnected();

    void connectionError(QAbstractSocket::SocketError se);

    void slabNumberSelection();

    void detectionSlabsBackClicked();

    void detectSlab();

    void detectAndOnSlab();

    void writingErrorLanHandler(const QJsonArray& command);

    void setMasterVoltageClicked(int slabId);

    void setSlaveVoltageClicked(int slabId);

    void onMasterClicked(int slabId);

    void onSlaveClicked(int slabId);

    void offMasterClicked(int slabId);

    void offSlaveClicked(int slabId);

    void showSlabsAfterLanConnection(QString ipAddress);

    void appendSlabToModel(Slab slab);

    void updateSlabInModel(Slab slab);

    void addWidgetsToTable(quint16 id);

    void initFailedLanHandler(quint16 id, const QString& message);

    void onFailedLanHandler(quint16 id, const QString& message);

    void refreshSlab(quint16 id);

    void tableUpdate();
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
    QHash<quint8, SlabState> slabStates;
    QSettings *settings = nullptr;


    static constexpr quint16 PORT = 5555;
    static constexpr quint16 READ_READY_SERIAL_TIME = 5000;
    static constexpr int UPDATE_TABLE_TIME = 10000;
    static const QString HUB_RESPONSE;
    static const QString LAN_CONNECTION_LABEL_TEXT;
    static const QString USB_CONNECTION_LABEL_TEXT;
    static const QStringList HEADERS;
    static const QJsonArray CLOSE;

    void addSetWidgets();
};

#endif // WIDGET_H
