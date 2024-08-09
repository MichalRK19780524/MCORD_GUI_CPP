#ifndef LANCONNECTION_H
#define LANCONNECTION_H

#include <QJsonArray>
#include <QTcpSocket>
#include <QHash>
#include <QPair>

#include "slab.h"
#include "afetype.h"

using namespace std;
class LanConnection : public QObject
{

        Q_OBJECT

public:
//    LanConnection();
    explicit LanConnection(QTcpSocket *socket, QObject *parent = nullptr);

    static constexpr quint16 PORT {5555};
    static constexpr quint16 CURRENT_AVG_NUMBER {1};
    static constexpr quint16 TEMPERATURE_AVG_NUMBER {1};
    static const QJsonArray CLOSE;

//    LanConnection(QTcpSocket* socket);

    ~LanConnection() override;

    //! Connects the specified HUB
    /*!
     * \brief LanConnection::connect
     * \param ipAddress IPv4 HUB LAN Address
     * \param port TCP
     * \return an empty string if the connection was successful, otherwise an error message informing about the reason for which the connection failed
     */

    QString downloadSetVoltage(Slab& slab, AfeType afeType);
    QString downloadMeasuredVoltage(Slab& slab, AfeType afeType);
    QString downloadMeasuredCurrent(Slab& slab, AfeType afeType, quint16 avgNumber);
    QString downloadMeasuredTemperature(Slab& slab, AfeType afeType, quint16 avgNumber);
    QString initSlab(const Slab& slab);
    QString onSlab(const Slab& slab);
    QString offSlab(const Slab& slab);
    QString setSlabVoltage(Slab& slab);
    QTcpSocket* getSocket();
    bool initAndOnSlab(const Slab &slab);
    QHostAddress getPeerAddress();

signals:
    void connectionFailed(const QString &message);
    void connectionSucceeded(const QString &ipAddress);
    void writingError(const QJsonArray &command);
    void readingSlabError(const Slab &slab, const QString &message);
    void readingAllSlabsError(const QString &message);
    void slabReadingCompleted(const Slab &slab);
    void onFailed(quint16 slabId, const QString &message);
    void initFailed(quint16 slabId, const QString &message);
    void offFailed(quint16 slabId, const QString &message);
    void appendSlabToTableRequired(const Slab &slab);
    void appendManySlabsToTableRequired(const QList<Slab> &slabs);
    void updateManySlabsToTableRequired(const QList<Slab> &slabs);
    void updateSlabToTableRequired(const Slab &slab);
    void slabDataRetrieved(const Slab &slab);
    void manySlabsDataRetrieved(const QList<Slab> &slabs);
    void setMasterFailed(quint16 slabId, const QString &message);
    void setSlaveFailed(quint16 slabId, const QString &message);
    void setManySlabsFailed(const QList<int> &ids, const QStringList &results);
    void offManySlabsFailed(const QList<int> &ids, const QStringList &results);
    void onHubFailed(const QString &message);
    void offHubFailed(const QString &message);
    void setMasterSucceeded(const Slab &slab);
    void setSlaveSucceeded(const Slab &slab);
    void setManySlabsSucceeded(const QList<Slab> &slabs);
    void offManySlabsSucceeded(const QList<Slab> &slabs);
    void onHubSucceeded();
    void offHubSucceeded();
    void loadAllSetSipmVoltageCompleted(const QPair<QVariantHash, QVariantHash> &voltages);

public slots:
    void connect(const QString &ipAddress, quint16 port = PORT);
    void closeConnection();
    void initAndOnNewSlab(Slab slab);
    void initAndOnManySlabs(QList<Slab> slabs);
    void offManySlabs(QList<Slab> slabs);
    void setManySlabs(QList<Slab> slabs);
    void initAndOnExistingSlab(Slab slab);
    void getSlab(Slab slab, AfeType afeType);
    void updateSlab(Slab slab);
    void updateManySlabs(QList<Slab> slabs);
    void setMasterVoltage(Slab slab);
    void setSlaveVoltage(Slab slab);
    void onHub();
    void offHub();
    void loadAllSetSipmVoltageFromHub();


private:
    static const QString HUB_RESPONSE;
    static const QString DOWNLOAD_MASTER_VOLTAGE_COMMAND;
    static const QString DOWNLOAD_SLAVE_VOLTAGE_COMMAND;
    static const QString DOWNLOAD_MASTER_SET_VOLTAGE_COMMAND;
    static const QString DOWNLOAD_SLAVE_SET_VOLTAGE_COMMAND;
    static const QString DOWNLOAD_ALL_SET_VOLTAGE_COMMAND;
    static const QString DOWNLOAD_MASTER_CURRENT_COMMAND;
    static const QString DOWNLOAD_SLAVE_CURRENT_COMMAND;
    static const QString DOWNLOAD_MASTER_TEMPERATURE_COMMAND;
    static const QString DOWNLOAD_SLAVE_TEMPERATURE_COMMAND;
    static const QString INIT_COMMAND;
    static const QString ON_COMMAND;
    static const QString OFF_COMMAND;
    static const QString SET_VOLTAGE_COMMAND;
    static const QString ON_HUB_COMMAND;
    static const QString OFF_HUB_COMMAND;
    static constexpr quint16 CONNECTING_TIME = 2000;
    static constexpr quint16 READ_READY_LAN_TIME = 10000;
    static constexpr quint16 BYTES_WRITEN_LAN_TIME = 2000;

    QTcpSocket *socket = nullptr;

    static QString isSlabCorrect(const Slab &slab);
    std::shared_ptr<Sipm> getSipmVoltagFromHub(std::shared_ptr<Sipm> simp, QJsonArray command);
    std::shared_ptr<Sipm> getSetSipmVoltagFromHub(std::shared_ptr<Sipm> simp, QJsonArray command);
    std::shared_ptr<Sipm> getSipmAmperageFromHub(std::shared_ptr<Sipm> sipm, QJsonArray command, quint16 avgNumber);
    std::shared_ptr<Sipm> getSipmTemperatureFromHub(std::shared_ptr<Sipm> sipm, const QJsonArray& command, quint16 avgNumber);
    bool readSlab(Slab &slab, AfeType afeType);
};

#endif // LANCONNECTION_H
