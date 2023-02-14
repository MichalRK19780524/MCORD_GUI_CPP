#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>

#include "lanconnection.h"
#include "afetype.h"

const QJsonArray LanConnection::CLOSE = {QString("!disconnect")};
const QString LanConnection::HUB_RESPONSE {"\"Client connected with AFE HUB "};
const QString LanConnection::DOWNLOAD_MASTER_VOLTAGE_COMMAND {"get_master_voltage"};
const QString LanConnection::DOWNLOAD_SLAVE_VOLTAGE_COMMAND {"get_slave_voltage"};

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
        if(socket->waitForReadyRead(READ_READY_LAN_TIME))
        {
            QString welcomeMessage = QString(socket->readAll());
            if(welcomeMessage != HUB_RESPONSE + ipAddress + "\"")
            {
                return "Unable to read from the HUB with specified IP address";
            }
            else
            {
                qDebug() << "read OK";
            }

        }
        else
        {
            return "Unable to read data from the specified IP address";
        }
    }
    else
    {
       return "Unable to connect to the specified IP address";
    }
    return "";
}

Slab* LanConnection::downloadMeasuredVoltage(Slab *slab, AfeType afeType)
{
    if(slab->getStatus().compare( "OK") != 0)
    {
        return nullptr;
    }

    quint16 id = slab->getId();
    if(id == 0)
    {
        slab->setStatus("No Id");
        return slab;
    }

    QJsonArray commandMaster = {DOWNLOAD_MASTER_VOLTAGE_COMMAND, id};
    QJsonArray commandSlave = {DOWNLOAD_SLAVE_VOLTAGE_COMMAND, id};
    qint64 resultMaster = 0;
    qint64 resultSlave = 0;
    if(socket->isOpen())
    {
        if(afeType == AfeType::Master)
        {
            resultMaster = socket->write(QJsonDocument(commandMaster).toJson(QJsonDocument::Compact));
            if(resultMaster <= 0)
            {
                slab->setStatus("Failed to send master voltage reading command");
                return slab;
            }

            if(socket->waitForBytesWritten(BYTES_WRITEN_LAN_TIME))
            {
                if(socket->waitForReadyRead(READ_READY_LAN_TIME))
                {

                    QJsonDocument jsonDocument = QJsonDocument::fromJson(socket->readAll());
                    QString status = jsonDocument.array().at(0).toString();
                    if(status.isNull() || status.compare("OK") != 0)
                    {
                        slab->setStatus("Error reading voltage from master SiPM");
                        return slab;
                    }
                    else
                    {
                        float voltage = jsonDocument.array().at(1).toDouble();
                        slab->getMaster()->setMeasuredVoltage(voltage);
                        slab->setStatus("OK");
                        return slab;
                    }

                }
                else
                {
                    slab->setStatus("Voltage reading from master SiPM failed");
                    return slab;
                }
            }
            else
            {
                slab->setStatus("Voltage read from master SiPM command failed");
                return slab;
            }
        }
        else if(afeType == AfeType::Slave)
        {
            resultSlave = socket->write(QJsonDocument(commandSlave).toJson(QJsonDocument::Compact));
            if(resultSlave <= 0)
            {
                slab->setStatus("Failed to send slave voltage reading command");
                return slab;
            }

            if(socket->waitForBytesWritten(BYTES_WRITEN_LAN_TIME))
            {
                if(socket->waitForReadyRead(READ_READY_LAN_TIME))
                {

                    QJsonDocument jsonDocument = QJsonDocument::fromJson(socket->readAll());
                    QString status = jsonDocument.array().at(0).toString();
                    if(status.isNull() || status.compare("OK") != 0)
                    {
                        slab->setStatus("Error reading voltage from slave SiPM");
                        return slab;
                    }
                    else
                    {
                        float voltage = jsonDocument.array().at(1).toDouble();
                        slab->getSlave()->setMeasuredVoltage(voltage);
                        slab->setStatus("OK");
                        return slab;
                    }

                }
                else
                {
                    slab->setStatus("Voltage reading from slave SiPM failed");
                    return slab;
                }
            }
            else
            {
                slab->setStatus("Voltage read from slave SiPM command failed");
                return slab;
            }
        }
        else
        {
            resultMaster = socket->write(QJsonDocument(commandMaster).toJson(QJsonDocument::Compact));
            if(resultMaster <= 0)
            {
                slab->setStatus("Failed to send master voltage reading command");
                return slab;
            }


            if(socket->waitForBytesWritten(BYTES_WRITEN_LAN_TIME))
            {
                if(socket->waitForReadyRead(READ_READY_LAN_TIME))
                {

                    QJsonDocument jsonDocument = QJsonDocument::fromJson(socket->readAll());
                    QString status = jsonDocument.array().at(0).toString();
                    if(status.isNull() || status.compare("OK") != 0)
                    {
                        slab->setStatus("Error reading voltage from master SiPM");
                        return slab;
                    }
                    else
                    {
                        float voltage = jsonDocument.array().at(1).toDouble();
                        slab->getMaster()->setMeasuredVoltage(voltage);

                        resultSlave = socket->write(QJsonDocument(commandSlave).toJson(QJsonDocument::Compact));
                        if(resultSlave <= 0)
                        {
                            slab->setStatus("Failed to send slave voltage reading command");
                            return slab;
                        }

                        if(socket->waitForBytesWritten(BYTES_WRITEN_LAN_TIME))
                        {
                            if(socket->waitForReadyRead(READ_READY_LAN_TIME))
                            {

                                QJsonDocument jsonDocument = QJsonDocument::fromJson(socket->readAll());
                                QString status = jsonDocument.array().at(0).toString();
                                if(status.isNull() || status.compare("OK") != 0)
                                {
                                    slab->setStatus("Error reading voltage from slave SiPM");
                                    return slab;
                                }
                                else
                                {
                                    float voltage = jsonDocument.array().at(1).toDouble();
                                    slab->getSlave()->setMeasuredVoltage(voltage);
                                    slab->setStatus("OK");
                                    return slab;
                                }

                            }
                            else
                            {
                                slab->setStatus("Voltage reading from slave SiPM failed");
                                return slab;
                            }

                            slab->setStatus("OK");
                            return slab;
                        }
                        else
                        {
                            slab->setStatus("Voltage read from slave SiPM command failed");
                            return slab;
                        }
                    }
                }
                else
                {
                    slab->setStatus("Voltage reading from master SiPM failed");
                    return slab;
                }
            }
            else
            {
                slab->setStatus("Voltage read from master SiPM command failed");
                return slab;
            }
        }
    }
    else
    {
        slab->setStatus("Faild to open TCP socket");
        return slab;
    }
}

QTcpSocket *LanConnection::getSocket()
{
    return socket;
}
