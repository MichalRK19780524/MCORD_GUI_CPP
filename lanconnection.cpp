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

//QString LanConnection::downloadMeasuredCurrent(Slab *slab, AfeType afeType, quint16 number)
//{

//}

QString LanConnection::downloadMeasuredVoltage(Slab *slab, AfeType afeType)
{

    QString result = LanConnection::isSlabCorrect(slab);
    if(result != "OK")
    {
        return result;
    }

    quint16 id = slab->getId();

    QJsonArray commandMaster = {DOWNLOAD_MASTER_VOLTAGE_COMMAND, id};
    QJsonArray commandSlave = {DOWNLOAD_SLAVE_VOLTAGE_COMMAND, id};

    if(socket->isOpen())
    {
        if(afeType == AfeType::Master)
        {
            getMasterVoltageSlabFromHub(slab, commandMaster);
            return "OK";
        }
        else if(afeType == AfeType::Slave)
        {
            getSlaveVoltageSlabFromHub(slab, commandSlave);
            return "OK";
        }
        else if(afeType == AfeType::Both)
        {
            getMasterVoltageSlabFromHub(slab, commandMaster);
            getSlaveVoltageSlabFromHub(slab, commandSlave);
            return "OK";
        }
        else
        {
            return "Internal Error";
        }

    }
    else
    {
        return "Faild to open TCP socket";
    }

}

Slab* LanConnection::getMasterVoltageSlabFromHub(Slab* slab, QJsonArray commandMaster)
{
    qint64 resultMaster = socket->write(QJsonDocument(commandMaster).toJson(QJsonDocument::Compact));
    if(resultMaster <= 0)
    {
        slab->getMaster()->setStatus("Failed to send master voltage reading command");
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
                if(status.isEmpty())
                {
                    resultMaster = socket->write(QJsonDocument(commandMaster).toJson(QJsonDocument::Compact));
                    if(resultMaster <= 0)
                    {
                        slab->getMaster()->setStatus("Failed to send master voltage reading command");
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
                                slab->getMaster()->setStatus("Error reading voltage from master SiPM");
                                return slab;
                            }
                            else
                            {
                                float voltage = jsonDocument.array().at(1).toDouble();
                                slab->getMaster()->setMeasuredVoltage(voltage);
                                slab->getMaster()->setStatus("OK");
                                return slab;
                            }
                        }
                        else
                        {
                            slab->getMaster()->setStatus("Error reading voltage from master SiPM");
                            return slab;
                        }
                    }
                    else
                    {
                        slab->getMaster()->setStatus("Voltage read from master SiPM command failed");
                        return slab;
                    }

                }
                else
                {
                    slab->getMaster()->setStatus("Error reading voltage from master SiPM");
                    return slab;
                }

            }
            else
            {
                float voltage = jsonDocument.array().at(1).toDouble();
                slab->getMaster()->setMeasuredVoltage(voltage);
                slab->getMaster()->setStatus("OK");
                return slab;
            }

        }
        else
        {
            slab->getMaster()->setStatus("Voltage reading from master SiPM failed");
            return slab;
        }
    }
    else
    {
        slab->getMaster()->setStatus("Voltage read from master SiPM command failed");
        return slab;
    }
}

Slab* LanConnection::getSlaveVoltageSlabFromHub(Slab* slab, QJsonArray commandSlave)
{
            qint64 resultSlave = socket->write(QJsonDocument(commandSlave).toJson(QJsonDocument::Compact));
            if(resultSlave <= 0)
            {
                slab->getSlave()->setStatus("Failed to send slave voltage reading command");
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
                        if(status.isEmpty())
                        {
                            resultSlave = socket->write(QJsonDocument(commandSlave).toJson(QJsonDocument::Compact));
                            if(resultSlave <= 0)
                            {
                                slab->getSlave()->setStatus("Failed to send slave voltage reading command");
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
                                        slab->getSlave()->setStatus("Error reading voltage from slave SiPM");
                                        return slab;
                                    }
                                    else
                                    {
                                        float voltage = jsonDocument.array().at(1).toDouble();
                                        slab->getSlave()->setMeasuredVoltage(voltage);
                                        slab->getSlave()->setStatus("OK");
                                        return slab;
                                    }
                                }
                                else
                                {
                                    slab->getSlave()->setStatus("Error reading voltage from slave SiPM");
                                    return slab;
                                }
                            }
                            else
                            {
                                slab->getSlave()->setStatus("Voltage read from slave SiPM command failed");
                                return slab;
                            }

                        }
                        else
                        {
                            slab->getSlave()->setStatus("Error reading voltage from slave SiPM");
                            return slab;
                        }

                    }
                    else
                    {
                        float voltage = jsonDocument.array().at(1).toDouble();
                        slab->getSlave()->setMeasuredVoltage(voltage);
                        slab->getSlave()->setStatus("OK");
                        return slab;
                    }

                }
                else
                {
                    slab->getSlave()->setStatus("Voltage reading from slave SiPM failed");
                    return slab;
                }
            }
            else
            {
                slab->getSlave()->setStatus("Voltage read from slave SiPM command failed");
                return slab;
            }
}

QString LanConnection::isSlabCorrect(Slab* slab)
{
    if(slab==nullptr)
    {
        return "Error: Slab Null Ptr";
    }
    else if(slab->getId() == 0)
    {
        return "Error: Null Id";
    }
    else if(slab->getMaster()->getStatus().compare( "OK") != 0)
    {
        return "Error: Master SiPM Status: " + slab->getMaster()->getStatus();
    }
    else if(slab->getSlave()->getStatus().compare( "OK") != 0)
    {
        return "Error: Slave SiPM Status: " + slab->getSlave()->getStatus();
    }
    else
    {
        return "OK";
    }
}

QTcpSocket *LanConnection::getSocket()
{
    return socket;
}
