#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>

#include "lanconnection.h"

const QJsonArray LanConnection::CLOSE = {QString("!disconnect")};
const QString LanConnection::HUB_RESPONSE = "\"Client connected with AFE HUB ";

LanConnection::LanConnection(QTcpSocket* socket): socket(socket)
{
//    socket = new QTcpSocket();
}

LanConnection::~LanConnection()
{
    if(socket->waitForBytesWritten(1000))
    {
        socket->close();
    }

    delete socket;
    socket = nullptr;
}

qint64 LanConnection::closeConnection()
{
    qint64 result = 0;
    if(socket->isOpen())
    {
        result = socket->write(QJsonDocument(CLOSE).toJson(QJsonDocument::Compact));
        if(socket->waitForBytesWritten(1000))
        {
            socket->close();
        }

    }
    return result;
}



QString LanConnection::connect(QString ipAddress, quint16 port)
{

    socket->connectToHost(QHostAddress(ipAddress), port);
    // TODO  W dokumentacji jest napisane, że nie zawsze działa pod Windowsem - do zastanowienia się jak taką samą funkcjonalność napisać z wykorzystaniem innych mechanizmów
    if(socket->waitForConnected(CONNECTING_TIME))
    {
//        showDetectonSlabs(LAN_CONNECTION_LABEL_TEXT + ipAddress, Connection::LAN);
        if(socket->waitForReadyRead(READ_READY_LAN_TIME))
        {
            if(QString(socket->readAll()) != HUB_RESPONSE + ipAddress + "\"")
            {
//                QMessageBox::critical(this, "LAN reading error", "Unable to read from the HUB with specified IP address");
                return "Unable to read from the HUB with specified IP address";
            }
            else
            {
                qDebug() << "read OK";
            }

        }
        else
        {
            //QMessageBox::critical(this, "Connection error", "Unable to read data from the specified IP address");
            return "Unable to read data from the specified IP address";
        }
    }
    else
    {
       //QMessageBox::critical(this, "Connection error", "Unable to connect to the specified IP address");
       return "Unable to connect to the specified IP address";
    }
    return "";
}

QTcpSocket *LanConnection::getSocket()
{
    return socket;
}
