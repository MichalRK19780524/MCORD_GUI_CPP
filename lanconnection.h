#ifndef LANCONNECTION_H
#define LANCONNECTION_H

#include <QJsonArray>
#include <QTcpSocket>

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
    QTcpSocket* getSocket();

private:
    static const QJsonArray CLOSE;
    static const QString HUB_RESPONSE;
    static constexpr quint16 CONNECTING_TIME = 1000;
    static constexpr quint16 READ_READY_LAN_TIME = 5000;

    QTcpSocket *socket = nullptr;
};

#endif // LANCONNECTION_H
