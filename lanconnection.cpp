#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>
#include <QThread>
#include <gsl/gsl_statistics.h>
#include <QMessageBox>
#include <QStringList>

#include "afetype.h"
#include "lanconnection.h"

const QJsonArray LanConnection::CLOSE = {QString("!disconnect")};
const QString LanConnection::HUB_RESPONSE{"\"Client connected with AFE HUB "};
const QString LanConnection::DOWNLOAD_MASTER_VOLTAGE_COMMAND{
        "get_master_voltage"};
const QString LanConnection::DOWNLOAD_SLAVE_VOLTAGE_COMMAND{
        "get_slave_voltage"};
const QString LanConnection::DOWNLOAD_MASTER_SET_VOLTAGE_COMMAND{
        "get_master_set_voltage"};
const QString LanConnection::DOWNLOAD_SLAVE_SET_VOLTAGE_COMMAND{
        "get_slave_set_voltage"};
const QString LanConnection::DOWNLOAD_MASTER_CURRENT_COMMAND{
        "get_master_amperage"};
const QString LanConnection::DOWNLOAD_SLAVE_CURRENT_COMMAND{
        "get_slave_amperage"};
const QString LanConnection::DOWNLOAD_MASTER_TEMPERATURE_COMMAND{
        "get_temperature_degree_master"};
const QString LanConnection::DOWNLOAD_SLAVE_TEMPERATURE_COMMAND{
        "get_temperature_degree_slave"};
const QString LanConnection::INIT_COMMAND{"init"};
//const QString LanConnection::ON_COMMAND{"hvon"};
//const QString LanConnection::OFF_COMMAND{"hvoff"};
const QString LanConnection::ON_COMMAND{"slabOn"};
const QString LanConnection::OFF_COMMAND{"slabOff"};
const QString LanConnection::SET_VOLTAGE_COMMAND{"setdac"};
const QString LanConnection::ON_HUB_COMMAND{"hubOn"};
const QString LanConnection::OFF_HUB_COMMAND{"hubOff"};

LanConnection::LanConnection(QTcpSocket *socket, QObject *parent)
        : socket(socket) {}


LanConnection::~LanConnection() {
    if (socket->waitForBytesWritten(1000)) {
        socket->close();
    }

    delete socket;
    socket = nullptr;
}

QString LanConnection::downloadSetVoltage(Slab &slab, AfeType afeType)
{
    QString result = LanConnection::isSlabCorrect(&slab);
    if (result != "OK") {
        return result;
    }

    quint16 id = slab.getId();

    QJsonArray commandMaster = {DOWNLOAD_MASTER_SET_VOLTAGE_COMMAND, id};
    QJsonArray commandSlave = {DOWNLOAD_SLAVE_SET_VOLTAGE_COMMAND, id};

    if (socket->isOpen()) {
        if (afeType == AfeType::Master) {
            getSetSipmVoltagFromHub(slab.getMaster(), commandMaster);
            return "OK";
        } else if (afeType == AfeType::Slave) {
            getSetSipmVoltagFromHub(slab.getSlave(), commandSlave);
            return "OK";
        } else if (afeType == AfeType::Both) {
            getSetSipmVoltagFromHub(slab.getMaster(), commandMaster);
            getSetSipmVoltagFromHub(slab.getSlave(), commandSlave);
            return "OK";
        } else {
            return "Internal Error";
        }

    } else {
        return "Faild to open TCP socket";
    }
}

void LanConnection::closeConnection() {
    qDebug() << "in closeConnection thread: " << QThread::currentThread();
    if (socket->isOpen()) {
        if (socket->write(QJsonDocument(CLOSE).toJson(QJsonDocument::Compact)) <
            0) {
            emit writingError(CLOSE);
        } else {
            if (socket->waitForBytesWritten(BYTES_WRITEN_LAN_TIME)) {
                socket->close();
            } else {
                emit writingError(CLOSE);
            }
        }
    }
}

void LanConnection::connect(QString ipAddress, quint16 port) {

    socket->connectToHost(QHostAddress(ipAddress), port);

    qDebug() << "in connect thread: " << QThread::currentThread();
    // TODO  W dokumentacji jest napisane, że nie zawsze działa pod Windowsem - do
    // zastanowienia się jak taką samą funkcjonalność napisać z wykorzystaniem
    // innych mechanizmów
    if (socket->waitForConnected(CONNECTING_TIME)) {
        if (socket->waitForReadyRead(READ_READY_LAN_TIME)) {
            QString welcomeMessage = QString(socket->readAll());
            if (welcomeMessage != HUB_RESPONSE + ipAddress + "\"") {
                emit connectionFailed(
                        "Unable to read from the HUB with specified IP address");
            } else {
                emit connectionSucceeded(ipAddress);
            }

        } else {
            emit connectionFailed(
                    "Unable to read data from the specified IP address");
        }
    } else {
        emit connectionFailed("Unable to connect to the specified IP address");
    }
    emit connectionFailed("");
}

QString LanConnection::downloadMeasuredCurrent(Slab &slab, AfeType afeType,
                                               quint16 avgNumber) {
    QString result = LanConnection::isSlabCorrect(&slab);
    if (result != "OK") {
        return result;
    }

    quint16 id = slab.getId();

    QJsonArray commandMaster = {DOWNLOAD_MASTER_CURRENT_COMMAND, id};
    QJsonArray commandSlave = {DOWNLOAD_SLAVE_CURRENT_COMMAND, id};

    if (socket->isOpen()) {
        if (afeType == AfeType::Master) {
            getSipmAmperageFromHub(slab.getMaster(), commandMaster, avgNumber);
            return "OK";
        } else if (afeType == AfeType::Slave) {
            getSipmAmperageFromHub(slab.getSlave(), commandSlave, avgNumber);
            return "OK";
        } else if (afeType == AfeType::Both) {
            getSipmAmperageFromHub(slab.getMaster(), commandMaster, avgNumber);
            getSipmAmperageFromHub(slab.getSlave(), commandSlave, avgNumber);
            return "OK";
        } else {
            return "Internal Error";
        }
    } else {
        return "Faild to open TCP socket";
    }
}

// Z AFE do HUBa przychodzi temperatura z dwóch SiPMów, więc docelowo trzeba się
// zastanowić, czy nie zmienić pobierania temperatury w AFE lub HUB, tak żeby
// było to zrobione spójnie.

// From the AFE to the HUB comes the temperature from two SiPMs,
// so ultimately you need to consider whether to rework the functions in the HUB
// or AFE, so that there is a uniform download of temperature and current.

QString LanConnection::downloadMeasuredTemperature(Slab &slab, AfeType afeType,
                                                   quint16 avgNumber) {
    QString result = LanConnection::isSlabCorrect(&slab);
    if (result != "OK") {
        return result;
    }

    quint16 id = slab.getId();

    QJsonArray commandMaster = {DOWNLOAD_MASTER_TEMPERATURE_COMMAND, id};
    QJsonArray commandSlave = {DOWNLOAD_SLAVE_TEMPERATURE_COMMAND, id};

    if (socket->isOpen()) {
        if (afeType == AfeType::Master) {
            getSipmTemperatureFromHub(slab.getMaster(), commandMaster, avgNumber);
            return "OK";
        } else if (afeType == AfeType::Slave) {
            getSipmTemperatureFromHub(slab.getSlave(), commandSlave, avgNumber);
            return "OK";
        } else if (afeType == AfeType::Both) {
            getSipmTemperatureFromHub(slab.getMaster(), commandMaster, avgNumber);
            getSipmTemperatureFromHub(slab.getSlave(), commandSlave, avgNumber);
            return "OK";
        } else {
            return "Internal Error";
        }
    } else {
        return "Faild to open TCP socket";
    }
}

 QString LanConnection::setSlabVoltage(Slab &slab)
{
     QString result = LanConnection::isSlabCorrect(&slab);
     if(result != "OK")
     {
         return result;
     }

    quint16 id = slab.getId();
    float masterVoltage = slab.getMaster()->getSetVoltage();
    float slaveVoltage = slab.getSlave()->getSetVoltage();

    QJsonArray command = {SET_VOLTAGE_COMMAND, id, masterVoltage,
    slaveVoltage};

    if(socket->isOpen())
    {
        qint64 result =
        socket->write(QJsonDocument(command).toJson(QJsonDocument::Compact));
        qDebug() << command;
        if(result <= 0)
        {
            return "Failed to send voltage setting command";
        }

        if(socket->waitForBytesWritten(BYTES_WRITEN_LAN_TIME))
        {
            if(socket->waitForReadyRead(READ_READY_LAN_TIME))
            {
                QJsonDocument jsonDocument =
                QJsonDocument::fromJson(socket->readAll()); QString status =
                jsonDocument.array().at(0).toString(); if(status.isNull() ||
                status.compare("OK") != 0)
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
}

bool LanConnection::readSlab(Slab &slab, AfeType afeType) {
    QString result;

    result = downloadSetVoltage(slab, afeType);
    if (result != "OK") {
        emit readingError(result);
        return false;
    }
    result = downloadMeasuredVoltage(slab, afeType);
    if (result != "OK") {
        emit readingError(result);
        return false;
    }
    result = downloadMeasuredCurrent(slab, afeType, CURRENT_AVG_NUMBER);
    if (result != "OK") {
        emit readingError(result);
        return false;
    }
    result = downloadMeasuredTemperature(slab, afeType, TEMPERATURE_AVG_NUMBER);
    if (result != "OK") {
        emit readingError(result);
        return false;
    }
    return true;
}

void LanConnection::getSlab(Slab slab, AfeType afeType) {
    if (readSlab(slab, afeType)) {
        emit slabReadingCompleted(slab);
    }
}

void LanConnection::updateSlab(Slab slab) {
    if (readSlab(slab, AfeType::Both)) {
        emit slabDataRetrieved(slab);
    }
}

void LanConnection::updateManySlabs(QList<Slab> slabs)
{

    bool status{true};
    for(Slab slab: slabs){
        if(!status){
            break;
        }
        if(slab.getId() != -1){
            status &= readSlab(slab, AfeType::Both);
        }
    }
    emit manySlabsDataRetrieved(slabs);
}


QString LanConnection::initSlab(Slab& slab) {
    QJsonArray command = {INIT_COMMAND, slab.getId()};

    if (socket->isOpen()) {
        qint64 result = socket->write(QJsonDocument(command).toJson(QJsonDocument::Compact));
        qDebug() << command;
        if (result <= 0) {
            emit writingError(command);
            return "Writing Error";
        } else {
            if (socket->waitForBytesWritten(BYTES_WRITEN_LAN_TIME)) {
                if (socket->waitForReadyRead(READ_READY_LAN_TIME)) {

                    QJsonDocument jsonDocument =
                            QJsonDocument::fromJson(socket->readAll());
                    QString status = jsonDocument.array().at(0).toString();
                    if (status.isNull() || status.compare("OK") != 0) {
                        return "Error while executing initialization command";
                    } else {
                        return "OK";
                    }
                } else {
                    return "Error reading the result of the initialization command";
                }
            } else {
                return "Error processing initialization command";
            }
        }
    } else {
        return "Failed to open TCP socket";
    }
}

QString LanConnection::onSlab(const Slab& slab) {
    QJsonArray command = {ON_COMMAND, slab.getId()};

    if (socket->isOpen()) {
        qint64 result = socket->write(QJsonDocument(command).toJson(QJsonDocument::Compact));
        qDebug() << command;
        if (result <= 0) {
            emit writingError(command);
            return "Writing Error";
        } else {
            if (socket->waitForBytesWritten(BYTES_WRITEN_LAN_TIME)) {
                if (socket->waitForReadyRead(READ_READY_LAN_TIME)) {

                    QJsonDocument jsonDocument =
                            QJsonDocument::fromJson(socket->readAll());
                    QString status = jsonDocument.array().at(0).toString();
                    if (status.isNull() || status.compare("OK") != 0) {
                        return "Error while executing the command to turn on the SiPM "
                               "voltage";
                    } else {
                        return "OK";
                    }
                } else {
                    return "Error reading the result of the SiPM voltage turn on command";
                }
            } else {
                return "Error processing the SiPM power on command";
            }
        }
    } else {
        return "Faild to open TCP socket";
    }
}


QString LanConnection::offSlab(const Slab& slab) {
    QJsonArray command = {OFF_COMMAND, slab.getId()};
    QString message;

    if (socket->isOpen()) {
        qint64 result = socket->write(QJsonDocument(command).toJson(QJsonDocument::Compact));
        qDebug() << command;
        if (result <= 0) {
            emit writingError(command);
            return "Writing Error";
        } else {
            if (socket->waitForBytesWritten(BYTES_WRITEN_LAN_TIME)) {
                if (socket->waitForReadyRead(READ_READY_LAN_TIME)) {

                    QJsonDocument jsonDocument =
                            QJsonDocument::fromJson(socket->readAll());
                    QString status = jsonDocument.array().at(0).toString();
                    if (status.isNull() || status.compare("OK") != 0) {
                        message = "Error while executing the command to turn off the SiPM voltage";
                        emit offFailed(slab.getId(), message);
                        return message;
                    } else {
                        emit updateSlabToTableRequired(slab);
                        return "OK";
                    }
                } else {
                    message = "Error reading the result of the SiPM voltage turn off command";
                    emit offFailed(slab.getId(), message);
                    return message;
                }
            } else {
                message = "Error processing the SiPM power off command";
                emit offFailed(slab.getId(), message);
                return message;
            }
        }
    } else {
        message = "Faild to open TCP socket";
        emit offFailed(slab.getId(), message);
        return message;
    }
}

QString LanConnection::downloadMeasuredVoltage(Slab &slab, AfeType afeType) {

    QString result = LanConnection::isSlabCorrect(&slab);
    if (result != "OK") {
        return result;
    }

    quint16 id = slab.getId();

    QJsonArray commandMaster = {DOWNLOAD_MASTER_VOLTAGE_COMMAND, id};
    QJsonArray commandSlave = {DOWNLOAD_SLAVE_VOLTAGE_COMMAND, id};

    if (socket->isOpen()) {
        if (afeType == AfeType::Master) {
            getSipmVoltagFromHub(slab.getMaster(), commandMaster);
            return "OK";
        } else if (afeType == AfeType::Slave) {
            getSipmVoltagFromHub(slab.getSlave(), commandSlave);
            return "OK";
        } else if (afeType == AfeType::Both) {
            getSipmVoltagFromHub(slab.getMaster(), commandMaster);
            getSipmVoltagFromHub(slab.getSlave(), commandSlave);
            return "OK";
        } else {
            return "Internal Error";
        }

    } else {
        return "Faild to open TCP socket";
    }
}

std::shared_ptr<Sipm> LanConnection::getSipmVoltagFromHub(std::shared_ptr<Sipm> sipm, QJsonArray command) {
    qint64 result = socket->write(QJsonDocument(command).toJson(QJsonDocument::Compact));
    qDebug() << command;
    if (result <= 0) {
        emit writingError(command);
    }

    if (socket->waitForBytesWritten(BYTES_WRITEN_LAN_TIME)) {
        if (socket->waitForReadyRead(READ_READY_LAN_TIME)) {

            QJsonDocument jsonDocument = QJsonDocument::fromJson(socket->readAll());
            QString status = jsonDocument.array().at(0).toString();
            if (status.isNull() || status.compare("OK") != 0) {
                if (status.isEmpty()) {
                    result = socket->write(
                            QJsonDocument(command).toJson(QJsonDocument::Compact));
                    if (result <= 0) {
                        sipm->setStatus("Failed to send voltage reading command");
                        //                                emit
                        return sipm;
                    }

                    if (socket->waitForBytesWritten(BYTES_WRITEN_LAN_TIME)) {
                        if (socket->waitForReadyRead(READ_READY_LAN_TIME)) {
                            jsonDocument = QJsonDocument::fromJson(socket->readAll());
                            QString status = jsonDocument.array().at(0).toString();
                            if (status.isNull() || status.compare("OK") != 0) {
                                sipm->setStatus("Error reading voltage from SiPM");
                                return sipm;
                            } else {
                                float voltage = jsonDocument.array().at(1).toDouble();
                                sipm->setMeasuredVoltage(voltage);
                                sipm->setStatus("OK");
                                return sipm;
                            }
                        } else {
                            sipm->setStatus("Error reading voltage from SiPM");
                            return sipm;
                        }
                    } else {
                        sipm->setStatus("Voltage read from SiPM command failed");
                        return sipm;
                    }

                } else {
                    sipm->setStatus("Error reading voltage from SiPM");
                    return sipm;
                }

            } else {
                float voltage = jsonDocument.array().at(1).toDouble();
                sipm->setMeasuredVoltage(voltage);
                sipm->setStatus("OK");
                return sipm;
            }

        } else {
            sipm->setStatus("Voltage reading from SiPM failed");
            return sipm;
        }
    } else {
        sipm->setStatus("Voltage read from SiPM command failed");
        return sipm;
    }
}

std::shared_ptr<Sipm> LanConnection::getSetSipmVoltagFromHub(std::shared_ptr<Sipm> sipm, QJsonArray command)
{
    qint64 result = socket->write(QJsonDocument(command).toJson(QJsonDocument::Compact));
    qDebug() << command;
    if (result <= 0) {
        emit writingError(command);
    }

    if (socket->waitForBytesWritten(BYTES_WRITEN_LAN_TIME)) {
        if (socket->waitForReadyRead(READ_READY_LAN_TIME)) {

            QJsonDocument jsonDocument = QJsonDocument::fromJson(socket->readAll());
            QString status = jsonDocument.array().at(0).toString();
            if (status.isNull() || status.compare("OK") != 0) {
                if (status.isEmpty()) {
                    result = socket->write(
                        QJsonDocument(command).toJson(QJsonDocument::Compact));
                    if (result <= 0) {
                        sipm->setStatus("Failed to send set voltage reading command");
                        //                                emit
                        return sipm;
                    }

                    if (socket->waitForBytesWritten(BYTES_WRITEN_LAN_TIME)) {
                        if (socket->waitForReadyRead(READ_READY_LAN_TIME)) {
                            jsonDocument = QJsonDocument::fromJson(socket->readAll());
                            QString status = jsonDocument.array().at(0).toString();
                            if (status.isNull() || status.compare("OK") != 0) {
                                sipm->setStatus("Error reading set voltage from SiPM");
                                return sipm;
                            } else {
                                float voltage = jsonDocument.array().at(1).toDouble();
                                sipm->setSetVoltage(voltage);
                                sipm->setStatus("OK");
                                return sipm;
                            }
                        } else {
                            sipm->setStatus("Error reading set voltage from SiPM");
                            return sipm;
                        }
                    } else {
                        sipm->setStatus("Set voltage read from SiPM command failed");
                        return sipm;
                    }

                } else {
                    sipm->setStatus("Error reading set voltage from SiPM");
                    return sipm;
                }

            } else {
                float voltage = jsonDocument.array().at(1).toDouble();
                sipm->setSetVoltage(voltage);
                sipm->setStatus("OK");
                return sipm;
            }

        } else {
            sipm->setStatus("Set voltage reading from SiPM failed");
            return sipm;
        }
    } else {
        sipm->setStatus("Set voltage read from SiPM command failed");
        return sipm;
    }
}


std::shared_ptr<Sipm> LanConnection::getSipmAmperageFromHub(std::shared_ptr<Sipm> sipm, QJsonArray command,
                                                            quint16 avgNumber) {
    QList<double> amperageList;
    amperageList.reserve(avgNumber);
    for (int i = 0; i < avgNumber; ++i) {
        qint64 result = socket->write(QJsonDocument(command).toJson(QJsonDocument::Compact));
        qDebug() << command;
        if (result <= 0) {
            emit writingError(command);
        }

        if (socket->waitForBytesWritten(BYTES_WRITEN_LAN_TIME)) {
            if (socket->waitForReadyRead(READ_READY_LAN_TIME)) {
                QJsonDocument jsonDocument = QJsonDocument::fromJson(socket->readAll());
                QString status = jsonDocument.array().at(0).toString();
                if (status.isNull() || status.compare("OK") != 0) {
                    sipm->setStatus("Error reading amperage from SiPM");
                    return sipm;
                } else {
                    double amperage = jsonDocument.array().at(1).toDouble();
                    amperageList.append(amperage);
                }
            } else {
                sipm->setStatus("Error reading amperage from SiPM");
                return sipm;
            }
        } else {
            sipm->setStatus("Amperage read from SiPM command failed");
            return sipm;
        }
    }

    const double *amperageArray = amperageList.constData();
    double meanAmperage = gsl_stats_mean(amperageArray, 1, amperageList.size());
    double meanNanoAmperage = meanAmperage * 1E9;
    sipm->setCurrent(static_cast<float>(meanNanoAmperage));
    double standardDeviationAmperage =
            gsl_stats_sd_m(amperageArray, 1, amperageList.size(), meanAmperage);
    sipm->setCurrentStandardDeviation(standardDeviationAmperage);
    return sipm;
}

std::shared_ptr<Sipm> LanConnection::getSipmTemperatureFromHub(std::shared_ptr<Sipm> sipm, const QJsonArray& command,
                                               quint16 avgNumber) {
    QList<double> temperatureList;
    temperatureList.reserve(avgNumber);
    for (int i = 0; i < avgNumber; ++i) {
        qint64 result = socket->write(QJsonDocument(command).toJson(QJsonDocument::Compact));
        qDebug() << command;
        if (result <= 0) {
            emit writingError(command);
        }

        if (socket->waitForBytesWritten(BYTES_WRITEN_LAN_TIME)) {
            if (socket->waitForReadyRead(READ_READY_LAN_TIME)) {
                QJsonDocument jsonDocument = QJsonDocument::fromJson(socket->readAll());
                QString status = jsonDocument.array().at(0).toString();
                if (status.isNull() || status.compare("OK") != 0) {
                    sipm->setStatus("Error reading temperature from SiPM");
                    return sipm;
                } else {
                    double temperature = jsonDocument.array().at(1).toDouble();
                    temperatureList.append(temperature);
                }
            } else {
                sipm->setStatus("Error reading temperature from SiPM");
                return sipm;
            }
        } else {
            sipm->setStatus("Temperature read from SiPM command failed");
            return sipm;
        }
    }

    const double *temperatureArray = temperatureList.constData();
    double meanTemperature =
            gsl_stats_mean(temperatureArray, 1, temperatureList.size());
    sipm->setTemperature(static_cast<float>(meanTemperature));
    double standardDeviationTemperature = gsl_stats_sd_m(
            temperatureArray, 1, temperatureList.size(), meanTemperature);
    sipm->setTemperatureStandardDeviation(float(standardDeviationTemperature));
    return sipm;
}

QString LanConnection::isSlabCorrect(Slab *slab) {
    if (slab == nullptr) {
        return "Error: Slab Null Ptr";
    } else if (slab->getMaster() == nullptr) {
        return "Error: Master SiPM Null Ptr";
    } else if (slab->getSlave() == nullptr) {
        return "Error: Slave SiPM Null Ptr";
    } else if (slab->getId() == 0) {
        return "Error: Null Id";
    } else if (slab->getMaster()->getStatus().compare("OK") != 0) {
        return "Error: Master SiPM Status: " + slab->getMaster()->getStatus();
    } else if (slab->getSlave()->getStatus().compare("OK") != 0) {
        return "Error: Slave SiPM Status: " + slab->getSlave()->getStatus();
    } else {
        return "OK";
    }
}

QTcpSocket *LanConnection::getSocket() { return socket; }

bool LanConnection::initAndOnSlab(Slab slab) {
    QString result = initSlab(slab);
    if (result == "OK") {
        result = onSlab(slab);
        if (result != "OK") {
            emit onFailed(slab.getId(), result);
            return false;
        }
    } else {
        emit initFailed(slab.getId(), result);
        return false;
    }
    return true;
}

QHostAddress LanConnection::getPeerAddress()
{
    if(socket->isOpen()){
        return socket->peerAddress();
    } else {
        return QHostAddress::Null;
    }
}

void LanConnection::initAndOnNewSlab(Slab slab) {
    if(initAndOnSlab(slab)) {
        emit appendSlabToTableRequired(slab);
    }
}

void LanConnection::initAndOnManySlabs(QList<Slab> slabs){
    bool state{true};
    for(Slab slab: slabs){
        if(slab.getId() != -1){
            state &= initAndOnSlab(slab);
            state &= readSlab(slab, AfeType::Both);
        }
    }
    if(state){
        emit appendManySlabsToTableRequired(slabs);
    }
}

void LanConnection::offManySlabs(QList<Slab> slabs){
    bool state{true};
    QList<int> ids;
    QStringList results;
    for(const Slab& slab: slabs){
        int id = slab.getId();
        ids.append(id);
        if(id != -1){
            QString result = offSlab(slab);
            results.append(result);
            state &= (result == "OK");
        }
        if(!state){
            emit offManySlabsFailed(ids, results);
        } else {
            emit offManySlabsSucceeded(slabs);
        }
    }
}

void LanConnection::setManySlabs(QList<Slab> slabs){
    bool state{true};
    QList<int> ids;
    QStringList results;
    for(Slab slab: slabs){
        int id = slab.getId();
        ids.append(id);
        if(id != -1){
            QString result = setSlabVoltage(slab);
            results.append(result);
            state &= (result == "OK");
        }
    }
    if(!state){
        emit setManySlabsFailed(ids, results);
        return;
    } else {
        emit setManySlabsSucceeded(slabs);
        return;
    }
}

void LanConnection::initAndOnExistingSlab(Slab slab) {
    if(initAndOnSlab(slab)) {
        emit updateSlabToTableRequired(slab);
    }
}

void LanConnection::setMasterVoltage(Slab slab) {
    QString result = setSlabVoltage(slab);
    qDebug() << "Slab in LanConnection::setMasterVoltage: "<< '\n' << "Id:" << slab.getId() << '\t' << "Set Master Voltage: " << slab.getMaster()->getSetVoltage();
    if (result != "OK") {
        emit setMasterFailed(slab.getId(), result);
        return;
    } else {
        emit setMasterSucceeded(slab);
        return;
    }
}

void LanConnection::setSlaveVoltage(Slab slab) {
    QString result = setSlabVoltage(slab);
    if (result != "OK") {
        emit setSlaveFailed(slab.getId(), result);
        return;
    } else {
        emit setSlaveSucceeded(slab);
        return;
    }
}

void LanConnection::onHub()
{
    QString result{""};
    QJsonArray command {ON_HUB_COMMAND};
    if(socket->isOpen())
    {
        qint64 writtingResult =
            socket->write(QJsonDocument(command).toJson(QJsonDocument::Compact));
        qDebug() << command;
        if(writtingResult <= 0)
        {
            result = "Failed to send voltage setting command";
        }
        if(socket->waitForBytesWritten(BYTES_WRITEN_LAN_TIME))
        {
            if(socket->waitForReadyRead(READ_READY_LAN_TIME))
            {
                QJsonDocument jsonDocument = QJsonDocument::fromJson(socket->readAll());
                QString status = jsonDocument.array().at(0).toString();
                if(status.isNull() || status.compare("OK") != 0)
                {
                    result = "Error turning on voltage to Hub";
                }
                else
                {
                    result = status;
                }
            }
            else
            {
                result = "Error processingturning on voltage to Hub";

            }
        }
        else
        {
            result = "Error processing turning on voltage to Hub";
        }
    }
    else
    {
        result = "Faild to open TCP socket";
    }

    if (result != "OK") {
        emit onHubFailed(result);
        return;
    } else {
        emit onHubSucceeded();
        return;
    }
}

void LanConnection::offHub()
{
    QString result{""};
    QJsonArray command {OFF_HUB_COMMAND};
    if(socket->isOpen())
    {
        qint64 writtingResult =
            socket->write(QJsonDocument(command).toJson(QJsonDocument::Compact));
        qDebug() << command;
        if(writtingResult <= 0)
        {
            result = "Failed to send voltage setting command";
        }
        if(socket->waitForBytesWritten(BYTES_WRITEN_LAN_TIME))
        {
            if(socket->waitForReadyRead(READ_READY_LAN_TIME))
            {
                QJsonDocument jsonDocument = QJsonDocument::fromJson(socket->readAll());
                QString status = jsonDocument.array().at(0).toString();
                if(status.isNull() || status.compare("OK") != 0)
                {
                    result = "Error turning off voltage to Hub";
                }
                else
                {
                    result = status;
                }
            }
            else
            {
                result = "Error processing turning off voltage to Hub";

            }
        }
        else
        {
            result = "Error processing turning off voltage to Hub";
        }
    }
    else
    {
        result = "Faild to open TCP socket";
    }

    if (result != "OK") {
        emit offHubFailed(result);
        return;
    } else {
        emit offHubSucceeded();
        return;
    }
}
