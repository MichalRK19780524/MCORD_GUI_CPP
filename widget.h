#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QSerialPort>
#include <QStandardItemModel>
#include <QStringList>
#include <QSignalMapper>
#include <QSettings>
#include <QFile>
#include <QMessageBox>

//#include "detectionslabswidget.h"
#include "lanconnection.h"
#include "manyslabsatonce.h"
#include "basewidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public BaseWidget {
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
        LAN_SLAB_SETTING,
        LAN_SLAB_OFF,
        ERROR
    };



public:
    explicit Widget(LanConnection *lanConnection, QWidget *parent = nullptr);

    ~Widget() override;

    void addPowerWidgets();


    


private:
    void showDetectonSlabs(const QString& labelName, Connection connection);

    void setMasterStatusColor(Slab &slab);

    void setSlaveStatusColor(Slab &slab);

//    QString appendSlabToModel(Slab slab);

    QString initAndOnNewSlab(int slabId);
    QString reloadMasterSlabToModel(Slab *slab);

    QString reloadSlaveSlabToModel(Slab *slab);

signals:

    void connectLan(QString ipAddress, quint16 port);

    void closeLanConnection();

    void slabRequired(Slab slab, AfeType afeType);

    void slabUpdateRequired(Slab slab);

    void initializationRequired(Slab slab);

    void setMasterVoltageRequired(Slab slab);

    void setSlaveVoltageRequired(Slab slab);

    void onRequired(Slab slab);

    void offRequired(Slab slab);

    void onSlaveRequired(Slab slab);

    void offSlaveRequired(Slab slab);

    void hubShutdownConfirmed();



private slots:

    void nextClicked();

    void backClicked();

    void disconnectClicked();

    void disconnected();

    static void connectionError(QAbstractSocket::SocketError se);

    void slabNumberSelection();
//    void connectionTypeBackClicked();

    void detectionSlabsBackClicked();

    void detectSlab();

    void detectAndOnSlab();

//    void onHubClicked();

    void offHubClicked();

    void writingErrorLanHandler(const QJsonArray& command);

    void setMasterVoltageClicked(int slabId);

    void setSlaveVoltageClicked(int slabId);

    void onClicked(int slabId);

//    void onSlaveClicked(int slabId);

    void offClicked(int slabId);

//    void offSlaveClicked(int slabId);

    void actionsAfterLanConnection(const QString& ipAddress);

    void appendSlabToModel(Slab slab);

    void updateSlabInModel(Slab slab);

    void addWidgetsToTable(quint16 id);

    void initFailedLanHandler(quint16 id, const QString& message);

    void onFailedLanHandler(quint16 id, const QString& message);

    void setMasterFailedLanHandler(quint16 id, const QString &message);

    void setSlaveFailedLanHandler(quint16 id, const QString &message);

    void refreshSlab(Slab slab);

    void tableUpdate();

    void saveSlabToFile(Slab slab);
private:
    Ui::Widget *ui;


    QSerialPort *serial = nullptr;
//    DetectionSlabsWidget *detectionSlabsWidget = nullptr;
    QSignalMapper *setMasterSignalMapper = nullptr;
    QSignalMapper *setSlaveSignalMapper = nullptr;
    QSignalMapper *onSignalMapper = nullptr;
//    QSignalMapper *onSlaveSignalMapper = nullptr;
    QSignalMapper *offSignalMapper = nullptr;
//    QSignalMapper *offSlaveSignalMapper = nullptr;
    State state = State::DISCONNECTED;
    QHash<quint8, SlabState> slabStates;
    QSettings *settings = nullptr;
    QFile file;
    QTextStream outTextData;
    ManySlabsAtOnce* manySlabsAtOnceDialog;

    static const QString HUB_RESPONSE;
    static const QString LAN_CONNECTION_LABEL_TEXT;
    static const QString USB_CONNECTION_LABEL_TEXT;
    static const QJsonArray CLOSE;

    void addSetWidgets();
};

#endif // WIDGET_H
