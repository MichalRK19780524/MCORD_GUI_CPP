#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <gsl/gsl_statistics.h>

#include "lanconnection.h"
#include "afetype.h"

const QJsonArray LanConnection::CLOSE = {QString("!disconnect")};
const QString LanConnection::HUB_RESPONSE {"\"Client connected with AFE HUB "};
const QString LanConnection::DOWNLOAD_MASTER_VOLTAGE_COMMAND {"get_master_voltage"};
const QString LanConnection::DOWNLOAD_SLAVE_VOLTAGE_COMMAND {"get_slave_voltage"};
const QString LanConnection::DOWNLOAD_MASTER_CURRENT_COMMAND {"get_master_amperage"};
const QString LanConnection::DOWNLOAD_SLAVE_CURRENT_COMMAND {"get_slave_amperage"};
const QString LanConnection::DOWNLOAD_MASTER_TEMPERATURE_COMMAND {"get_temperature_degree_master"};
const QString LanConnection::DOWNLOAD_SLAVE_TEMPERATURE_COMMAND {"get_temperature_degree_slave"};
const QString LanConnection::INIT_COMMAND {"init"};
const QString LanConnection::ON_COMMAND {"hvon"};
const QString LanConnection::OFF_COMMAND {"hvoff"};
const QString LanConnection::SET_VOLTAGE_COMMAND {"setdac"};

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

QString LanConnection::downloadMeasuredCurrent(Slab *slab, AfeType afeType, quint16 avgNumber)
{
    QString result = LanConnection::isSlabCorrect(slab);
    if(result != "OK")
    {
        return result;
    }

    quint16 id = slab->getId();

    QJsonArray commandMaster = {DOWNLOAD_MASTER_CURRENT_COMMAND, id};
    QJsonArray commandSlave = {DOWNLOAD_SLAVE_CURRENT_COMMAND, id};

    if(socket->isOpen())
    {
        if(afeType == AfeType::Master)
        {
            getSipmAmperageFromHub(slab->getMaster(), commandMaster, avgNumber);
            return "OK";
        }
        else if(afeType == AfeType::Slave)
        {
            getSipmAmperageFromHub(slab->getSlave(), commandSlave, avgNumber);
            return "OK";
        }
        else if(afeType == AfeType::Both)
        {
            getSipmAmperageFromHub(slab->getMaster(), commandMaster, avgNumber);
            getSipmAmperageFromHub(slab->getSlave(), commandSlave, avgNumber);
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

//Z AFE do HUBa przychodzi temperatura z dwóch SiPMów, więc docelowo trzeba się zastanowić,
//czy nie zmienić pobierania temperatury w AFE lub HUB, tak żeby było to zrobione spójnie.

// From the AFE to the HUB comes the temperature from two SiPMs,
//so ultimately you need to consider whether to rework the functions in the HUB or AFE,
//so that there is a uniform download of temperature and current.

QString LanConnection::downloadMeasuredTemperature(Slab *slab, AfeType afeType, quint16 avgNumber)
{
    QString result = LanConnection::isSlabCorrect(slab);
    if(result != "OK")
    {
        return result;
    }

    quint16 id = slab->getId();

    QJsonArray commandMaster = {DOWNLOAD_MASTER_TEMPERATURE_COMMAND, id};
    QJsonArray commandSlave = {DOWNLOAD_SLAVE_TEMPERATURE_COMMAND, id};

    if(socket->isOpen())
    {
        if(afeType == AfeType::Master)
        {
            getSipmTemperatureFromHub(slab->getMaster(), commandMaster, avgNumber);
            return "OK";
        }
        else if(afeType == AfeType::Slave)
        {
            getSipmTemperatureFromHub(slab->getSlave(), commandSlave, avgNumber);
            return "OK";
        }
        else if(afeType == AfeType::Both)
        {
            getSipmTemperatureFromHub(slab->getMaster(), commandMaster, avgNumber);
            getSipmTemperatureFromHub(slab->getSlave(), commandSlave, avgNumber);
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

QString LanConnection::setSlabVoltage(Slab *slab)
{
    QString result = LanConnection::isSlabCorrect(slab);
    if(result != "OK")
    {
        return result;
    }

    quint16 id = slab->getId();
    float masterVoltage = slab->getMaster()->getSetVoltage();
    float slaveVoltage = slab->getSlave()->getSetVoltage();

    QJsonArray command = {SET_VOLTAGE_COMMAND, id, masterVoltage, slaveVoltage};

    if(socket->isOpen())
    {
        qint64 result = socket->write(QJsonDocument(command).toJson(QJsonDocument::Compact));
        if(result <= 0)
        {
            return "Failed to send voltage setting command";
        }

        if(socket->waitForBytesWritten(BYTES_WRITEN_LAN_TIME))
        {
            if(socket->waitForReadyRead(READ_READY_LAN_TIME))
            {
                QJsonDocument jsonDocument = QJsonDocument::fromJson(socket->readAll());
                QString status = jsonDocument.array().at(0).toString();
                if(status.isNull() || status.compare("OK") != 0)
                {
                    return "Error setting voltage to SiPM";
                }
                else
                {
                    return status;
                }
            }
            else
            {
                return "Error reading amperage from SiPM";

            }
        }
        else
        {
            return "Amperage read from SiPM command failed";
        }
    }
    else
    {
        return "Faild to open TCP socket";
    }


    return result;
}

QString LanConnection::getSlab(Slab *slab, AfeType afeType)
{
    QString result;
    result = downloadMeasuredVoltage(slab, afeType);
    result = downloadMeasuredCurrent(slab, afeType, CURRENT_AVG_NUMBER);
    result = downloadMeasuredTemperature(slab, afeType, TEMPERATURE_AVG_NUMBER);
    return result;
}

QString LanConnection::initSlab(quint16 slabId)
{
    QJsonArray command = {INIT_COMMAND, slabId};

    if(socket->isOpen())
    {
        qint64 result = socket->write(QJsonDocument(command).toJson(QJsonDocument::Compact));
        if(result <= 0)
        {
            return "Failed to send initialization command";
        }
        else
        {
            if(socket->waitForBytesWritten(BYTES_WRITEN_LAN_TIME))
            {
                if(socket->waitForReadyRead(READ_READY_LAN_TIME))
                {

                    QJsonDocument jsonDocument = QJsonDocument::fromJson(socket->readAll());
                    QString status = jsonDocument.array().at(0).toString();
                    if(status.isNull() || status.compare("OK") != 0)
                    {
                        return "Error while executing initialization command";
                    }
                    else
                    {
                        return "OK";
                    }
                }
                else
                {
                    return "Error reading the result of the initialization command";
                }
            }
            else
            {
                return "Error processing initialization command";
            }
        }
    }
    {
        return "Faild to open TCP socket";
    }
}

QString LanConnection::onSlab(quint16 slabId)
{
    QJsonArray command = {ON_COMMAND, slabId};

    if(socket->isOpen())
    {
        qint64 result = socket->write(QJsonDocument(command).toJson(QJsonDocument::Compact));
        if(result <= 0)
        {
            return "Failed to send the command to turn on the SiPM voltage";
        }
        else
        {
            if(socket->waitForBytesWritten(BYTES_WRITEN_LAN_TIME))
            {
                if(socket->waitForReadyRead(READ_READY_LAN_TIME))
                {

                    QJsonDocument jsonDocument = QJsonDocument::fromJson(socket->readAll());
                    QString status = jsonDocument.array().at(0).toString();
                    if(status.isNull() || status.compare("OK") != 0)
                    {
                        return "Error while executing the command to turn on the SiPM voltage";
                    }
                    else
                    {
                        return "OK";
                    }
                }
                else
                {
                    return "Error reading the result of the SiPM voltage turn on command";
                }
            }
            else
            {
                return "Error processing the SiPM power on command";
            }
        }
    }
    {
        return "Faild to open TCP socket";
    }
}

QString LanConnection::offSlab(quint16 slabId)
{
    QJsonArray command = {OFF_COMMAND, slabId};

    if(socket->isOpen())
    {
        qint64 result = socket->write(QJsonDocument(command).toJson(QJsonDocument::Compact));
        if(result <= 0)
        {
            return "Failed to send the command to turn off the SiPM voltage";
        }
        else
        {
            if(socket->waitForBytesWritten(BYTES_WRITEN_LAN_TIME))
            {
                if(socket->waitForReadyRead(READ_READY_LAN_TIME))
                {

                    QJsonDocument jsonDocument = QJsonDocument::fromJson(socket->readAll());
                    QString status = jsonDocument.array().at(0).toString();
                    if(status.isNull() || status.compare("OK") != 0)
                    {
                        return "Error while executing the command to turn off the SiPM voltage";
                    }
                    else
                    {
                        return "OK";
                    }
                }
                else
                {
                    return "Error reading the result of the SiPM voltage turn off command";
                }
            }
            else
            {
                return "Error processing the SiPM power off command";
            }
        }
    }
    {
        return "Faild to open TCP socket";
    }
}


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
            getSipmVoltagFromHub(slab->getMaster(), commandMaster);
            return "OK";
        }
        else if(afeType == AfeType::Slave)
        {
            getSipmVoltagFromHub(slab->getSlave(), commandSlave);
            return "OK";
        }
        else if(afeType == AfeType::Both)
        {
            getSipmVoltagFromHub(slab->getMaster(), commandMaster);
            getSipmVoltagFromHub(slab->getSlave(), commandSlave);
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


Sipm* LanConnection::getSipmVoltagFromHub(Sipm* simp, QJsonArray command)
{
            qint64 result = socket->write(QJsonDocument(command).toJson(QJsonDocument::Compact));
            if(result <= 0)
            {
                simp->setStatus("Failed to send voltage reading command");
                return simp;
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
                            result = socket->write(QJsonDocument(command).toJson(QJsonDocument::Compact));
                            if(result <= 0)
                            {
                                simp->setStatus("Failed to send voltage reading command");
                                return simp;
                            }

                            if(socket->waitForBytesWritten(BYTES_WRITEN_LAN_TIME))
                            {
                                if(socket->waitForReadyRead(READ_READY_LAN_TIME))
                                {
                                    QJsonDocument jsonDocument = QJsonDocument::fromJson(socket->readAll());
                                    QString status = jsonDocument.array().at(0).toString();
                                    if(status.isNull() || status.compare("OK") != 0)
                                    {
                                        simp->setStatus("Error reading voltage from SiPM");
                                        return simp;
                                    }
                                    else
                                    {
                                        float voltage = jsonDocument.array().at(1).toDouble();
                                        simp->setMeasuredVoltage(voltage);
                                        simp->setStatus("OK");
                                        return simp;
                                    }
                                }
                                else
                                {
                                    simp->setStatus("Error reading voltage from SiPM");
                                    return simp;
                                }
                            }
                            else
                            {
                                simp->setStatus("Voltage read from SiPM command failed");
                                return simp;
                            }

                        }
                        else
                        {
                            simp->setStatus("Error reading voltage from SiPM");
                            return simp;
                        }

                    }
                    else
                    {
                        float voltage = jsonDocument.array().at(1).toDouble();
                        simp->setMeasuredVoltage(voltage);
                        simp->setStatus("OK");
                        return simp;
                    }

                }
                else
                {
                    simp->setStatus("Voltage reading from SiPM failed");
                    return simp;
                }
            }
            else
            {
                simp->setStatus("Voltage read from SiPM command failed");
                return simp;
            }
}


Sipm *LanConnection::getSipmAmperageFromHub(Sipm *simp, QJsonArray command, quint16 avgNumber)
{
    QList<double> amperageList;
    amperageList.reserve(avgNumber);
    for(int i = 0; i < avgNumber; ++i)
    {
        qint64 result = socket->write(QJsonDocument(command).toJson(QJsonDocument::Compact));
        if(result <= 0)
        {
            simp->setStatus("Failed to send amperage reading command");
            return simp;
        }

        if(socket->waitForBytesWritten(BYTES_WRITEN_LAN_TIME))
        {
            if(socket->waitForReadyRead(READ_READY_LAN_TIME))
            {
                QJsonDocument jsonDocument = QJsonDocument::fromJson(socket->readAll());
                QString status = jsonDocument.array().at(0).toString();
                if(status.isNull() || status.compare("OK") != 0)
                {
                    simp->setStatus("Error reading amperage from SiPM");
                    return simp;
                }
                else
                {
                    double amperage = jsonDocument.array().at(1).toDouble();
                    amperageList.append(amperage);
                }
            }
            else
            {
                simp->setStatus("Error reading amperage from SiPM");
                return simp;
            }
        }
        else
        {
            simp->setStatus("Amperage read from SiPM command failed");
            return simp;
        }
    }

    const double* amperageArray = amperageList.constData();
    double meanAmperage = gsl_stats_mean(amperageArray, 1, amperageList.size());
    double meanNanoAmperage = meanAmperage * 1E9;
    simp->setCurrent(static_cast<float>(meanNanoAmperage));
    double standardDeviationAmperage = gsl_stats_sd_m(amperageArray, 1, amperageList.size(), meanAmperage);
    simp->setCurrentStandardDeviation(standardDeviationAmperage);
    return simp;
}

Sipm *LanConnection::getSipmTemperatureFromHub(Sipm *simp, QJsonArray command, quint16 avgNumber)
{
    QList<double> temperatureList;
    temperatureList.reserve(avgNumber);
    for(int i = 0; i < avgNumber; ++i)
    {
        qint64 result = socket->write(QJsonDocument(command).toJson(QJsonDocument::Compact));
        if(result <= 0)
        {
            simp->setStatus("Failed to send temperature reading command");
            return simp;
        }

        if(socket->waitForBytesWritten(BYTES_WRITEN_LAN_TIME))
        {
            if(socket->waitForReadyRead(READ_READY_LAN_TIME))
            {
                QJsonDocument jsonDocument = QJsonDocument::fromJson(socket->readAll());
                QString status = jsonDocument.array().at(0).toString();
                if(status.isNull() || status.compare("OK") != 0)
                {
                    simp->setStatus("Error reading temperature from SiPM");
                    return simp;
                }
                else
                {
                    double temperature = jsonDocument.array().at(1).toDouble();
                    temperatureList.append(temperature);
                }
            }
            else
            {
                simp->setStatus("Error reading temperature from SiPM");
                return simp;
            }
        }
        else
        {
            simp->setStatus("Temperature read from SiPM command failed");
            return simp;
        }
    }

    const double* temperatureArray = temperatureList.constData();
    double meanTemperature = gsl_stats_mean(temperatureArray, 1, temperatureList.size());
    simp->setTemperature(static_cast<float>(meanTemperature));
    double standardDeviationTemperature = gsl_stats_sd_m(temperatureArray, 1, temperatureList.size(), meanTemperature);
    simp->setTemperatureStandardDeviation(standardDeviationTemperature);
    return simp;
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
