#ifndef LANCONNECTION_H
#define LANCONNECTION_H

#include <QJsonArray>
#include <QTcpSocket>

#include "slab.h"
#include "afetype.h"

using namespace std;
class LanConnection : public QObject
{

        Q_OBJECT

public:
//    LanConnection();
    LanConnection(QTcpSocket *socket, QObject *parent = nullptr);


    static constexpr quint16 CURRENT_AVG_NUMBER {10};
    static constexpr quint16 TEMPERATURE_AVG_NUMBER {5};

//    LanConnection(QTcpSocket* socket);

    virtual ~LanConnection();

    qint64 closeConnection();

    //! Connects the specified HUB
    /*!
     * \brief LanConnection::connect
     * \param ipAddress IPv4 HUB LAN Address
     * \param port TCP
     * \return an empty string if the connection was successful, otherwise an error message informing about the reason for which the connection failed
     */


    QString downloadMeasuredVoltage(Slab* slab, AfeType afeType);
    QString downloadMeasuredCurrent(Slab* slab, AfeType afeType, quint16 avgNumber);
    QString downloadMeasuredTemperature(Slab *slab, AfeType afeType, quint16 avgNumber);
    QString getSlab(Slab *slab, AfeType afeType);
    QString initSlab(quint16 slabId);
    QString onSlab(quint16 slabId);
    QString offSlab(quint16 slabId);
    QString setSlabVoltage(Slab *slab);

signals:
    void connectionFailed(QString message);
    void connectionSucceeded(QString ipAddress);
public slots:
    void connect(QString ipAddress, quint16 port);

    QTcpSocket* getSocket();

private:
    static const QJsonArray CLOSE;
    static const QString HUB_RESPONSE;
    static const QString DOWNLOAD_MASTER_VOLTAGE_COMMAND;
    static const QString DOWNLOAD_SLAVE_VOLTAGE_COMMAND;
    static const QString DOWNLOAD_MASTER_CURRENT_COMMAND;
    static const QString DOWNLOAD_SLAVE_CURRENT_COMMAND;
    static const QString DOWNLOAD_MASTER_TEMPERATURE_COMMAND;
    static const QString DOWNLOAD_SLAVE_TEMPERATURE_COMMAND;
    static const QString INIT_COMMAND;
    static const QString ON_COMMAND;
    static const QString OFF_COMMAND;
    static const QString SET_VOLTAGE_COMMAND;
    static constexpr quint16 CONNECTING_TIME = 2000;
    static constexpr quint16 READ_READY_LAN_TIME = 10000;
    static constexpr quint16 BYTES_WRITEN_LAN_TIME = 2000;

    QTcpSocket *socket = nullptr;

    static QString isSlabCorrect(Slab* slab);
    Sipm* getSipmVoltagFromHub(Sipm* simp, QJsonArray command);
    Sipm* getSipmAmperageFromHub(Sipm* simp, QJsonArray command, quint16 avgNumber);
    Sipm* getSipmTemperatureFromHub(Sipm* simp, QJsonArray command, quint16 avgNumber);
};

#endif // LANCONNECTION_H
