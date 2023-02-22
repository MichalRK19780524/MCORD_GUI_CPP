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
    LanConnection(QTcpSocket* socket);
    virtual ~LanConnection();

    qint64 closeConnection();

    //! Connects the specified HUB
    /*!
     * \brief LanConnection::connect
     * \param ipAddress IPv4 HUB LAN Address
     * \param port TCP
     * \return an empty string if the connection was successful, otherwise an error message informing about the reason for which the connection failed
     */
    QString connect(QString ipAddress, quint16 port);

    QString downloadMeasuredVoltage(Slab* slab, AfeType afeType);
    QString downloadMeasuredCurrent(Slab* slab, AfeType afeType, quint16 number);

    QTcpSocket* getSocket();

private:
    static const QJsonArray CLOSE;
    static const QString HUB_RESPONSE;
    static const QString DOWNLOAD_MASTER_VOLTAGE_COMMAND;
    static const QString DOWNLOAD_SLAVE_VOLTAGE_COMMAND;
    static constexpr quint16 CONNECTING_TIME = 2000;
    static constexpr quint16 READ_READY_LAN_TIME = 10000;
    static constexpr quint16 BYTES_WRITEN_LAN_TIME = 2000;

    QTcpSocket *socket = nullptr;

    static QString isSlabCorrect(Slab* slab);
//    Slab* getMasterVoltageSlabFromHub(Slab* slab, QJsonArray commandMaster);
//    Slab* getSlaveVoltageSlabFromHub(Slab* slab, QJsonArray commandMaster);
    Simp* getSipmVoltagFromHub(Simp* simp, QJsonArray commandSlave);


};

#endif // LANCONNECTION_H
