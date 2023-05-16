﻿//#include <QtGui>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSerialPortInfo>
#include <QDebug>
#include <QSvgRenderer>


#include "widget.h"
#include "statusicondelegate.h"
#include "./ui_widget.h"

const QString Widget::LAN_CONNECTION_LABEL_TEXT = "Connected to IP: ";
const QString Widget::USB_CONNECTION_LABEL_TEXT = "Connected to serial port: ";
const QStringList Widget::HEADERS {"Slab No.", "Status", "Type", "Set SiPM Volt.", "U[V]", "I[nA]", "T[C]"};

Widget::Widget(LanConnection * lanConnection, QWidget *parent)
    : QWidget(parent)
    , lanConnection(lanConnection)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->pushButtonBack->hide();
    ui->groupBoxLanConnection->hide();
    ui->groupBoxUsbConnection->hide();
    ui->groupBoxDetectionSlabs->hide();
    ui->connectionLabel->hide();
    ui->pushButtonDisconnect->hide();
    model = new DetectorTableModel(&Widget::HEADERS, this);
    setMasterSignalMapper = new QSignalMapper(this);
    onMasterSignalMapper = new QSignalMapper(this);
    offMasterSignalMapper = new QSignalMapper(this);

    setSlaveSignalMapper = new QSignalMapper(this);
    onSlaveSignalMapper = new QSignalMapper(this);
    offSlaveSignalMapper = new QSignalMapper(this);

    ui->slabsTableView->setModel(model);
    ui->slabsTableView->setItemDelegate(new StatusIconDelegate());
    ui->slabsTableView->setShowGrid(false);
    ui->slabsTableView->setAlternatingRowColors(true);

    QHeaderView *verticalHeader = ui->slabsTableView->verticalHeader();
    verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader->setDefaultSectionSize(48);

    serial = new QSerialPort(this);

    connect(ui->pushButtonNext, &QPushButton::clicked, this, &Widget::nextClicked);
    connect(ui->pushButtonBack, &QPushButton::clicked, this, &Widget::backClicked);
    connect(ui->pushButtonDisconnect, &QPushButton::clicked, this, &Widget::disconnectClicked);
    connect(ui->pushButtonDetectionSlabsNext, &QPushButton::clicked, this, &Widget::slabNumberSelection);
    connect(ui->pushButtonDetectionSlabBack, &QPushButton::clicked, this, &Widget::detectionSlabsBackClicked);
    connect(ui->pushButtonDetect, &QPushButton::clicked, this, &Widget::addSlab);
    connect(ui->pushButtonOn, &QPushButton::clicked, this, &Widget::addAndOnSlab);
    connect(ui->pushButtonBackSlabsChoice, &QPushButton::clicked, this, &Widget::disconnectClicked /*&Widget::backSlabsChoiceClicked*/);

    connect(lanConnection->getSocket(), &QTcpSocket::disconnected, this, &Widget::disconnected);
    connect(lanConnection->getSocket(), &QTcpSocket::errorOccurred, this, &Widget::connectionError);

    connect(setMasterSignalMapper, &QSignalMapper::mappedInt, this, &Widget::setMasterVoltageClicked);
    connect(onMasterSignalMapper, &QSignalMapper::mappedInt, this, &Widget::onMasterClicked);
    connect(offMasterSignalMapper, &QSignalMapper::mappedInt, this, &Widget::offMasterClicked);

    connect(setSlaveSignalMapper, &QSignalMapper::mappedInt, this, &Widget::setSlaveVoltageClicked);
    connect(onSlaveSignalMapper, &QSignalMapper::mappedInt, this, &Widget::onSlaveClicked);
    connect(offSlaveSignalMapper, &QSignalMapper::mappedInt, this, &Widget::offSlaveClicked);
}

Widget::~Widget()
{


//    serial->close();

    if(lanConnection->closeConnection() < 0)
    {
        QMessageBox::critical(this, "LAN error", "Unable to send data to HUB");
    }
    delete serial;
    serial = nullptr;

//    delete detectionSlabsWidget;
//    detectionSlabsWidget = nullptr;

    delete model;
    model = nullptr;

    delete setMasterSignalMapper;
    setMasterSignalMapper = nullptr;

    delete onMasterSignalMapper;
    onMasterSignalMapper = nullptr;

    delete offMasterSignalMapper;
    offMasterSignalMapper = nullptr;

    delete setSlaveSignalMapper;
    setSlaveSignalMapper = nullptr;

    delete onSlaveSignalMapper;
    onSlaveSignalMapper = nullptr;

    delete offSlaveSignalMapper;
    offSlaveSignalMapper = nullptr;

    delete ui;
    ui = nullptr;

}


void Widget::showDetectonSlabs(QString labelName, Connection connection)
{
    for(QWidget* children: ui->groupBoxDetectionSlabs->findChildren<QWidget*>())
    {
        children->show();
    }

    ui->groupBoxDetectionSlabs->show();
    if(connection == Connection::SERIAL)
    {
        ui->groupBoxUsbConnection->hide();
    }
    else
    {
        ui->groupBoxLanConnection->hide();
    }
    ui->pushButtonBack->hide();
    ui->pushButtonNext->hide();
//    ui->pushButtonDisconnect->show();
    ui->connectionLabel->setText(labelName);
    ui->connectionLabel->show();
    ui->widgetAddOneSlab->hide();
    ui->slabsTableView->hide();
//    ui->labelAddSlab->hide();
//    ui->lineEditAddSlab->hide();
//    ui->pushButtonDetect->hide();
//    ui->pushButtonOn->hide();
//    ui->pushButtonDetectionSlabBack->hide();
}

void Widget::nextClicked()
{
    qDebug() << "nextClicked";
    ui->groupBoxSelectConnection->setEnabled(false);
    if(state == State::DISCONNECTED)
    {
        qDebug() << "first nextClicked";
//        ui->groupBoxSelectConnection->hide();       
        ui->pushButtonBack->show();
        if(ui->radioButtonLan->isChecked())
        {
            state = State::LAN_SELECTED;
            ui->groupBoxLanConnection->show();

            QString ipRange = "(([ 0]+)|([ 0]*[0-9] *)|([0-9][0-9] )|([ 0][0-9][0-9])|(1[0-9][0-9])|([2][0-4][0-9])|(25[0-5]))";
            // You may want to use QRegularExpression for new code with Qt 5 (not mandatory).
            QRegularExpression ipRegex ("^" + ipRange
                         + "\\." + ipRange
                         + "\\." + ipRange
                         + "\\." + ipRange + "$");
            QRegularExpressionValidator *ipValidator = new QRegularExpressionValidator(ipRegex, this);
            ui->lineEditLan->setValidator(ipValidator);
            ui->lineEditLan->setInputMask("000.000.000.000");
            ui->lineEditLan->setCursorPosition(0);
        }
        else if(ui->radioButtonUsb->isChecked())
        {
            state = State::USB_SELECTED;
            ui->comboBoxUsb->clear();
            ui->groupBoxUsbConnection->show();
            QList<QString> portNames;
            QList<QSerialPortInfo> portList = QSerialPortInfo::availablePorts();
            std::transform(portList.cbegin(), portList.cend(),
                           std::back_inserter(portNames), [](QSerialPortInfo spi){return spi.portName();} );
            ui->comboBoxUsb->addItems(portNames);
        }
        else
        {
            state = State::ERROR;
            QMessageBox::critical(this, "Internal error", "Push buttons unexpected behaviour");
        }
    }
    else
    {
        if(ui->radioButtonLan->isChecked())
        {
            qDebug() << "LAN nextClicked";
            QString ipAddress = ui->lineEditLan->text();
            QString connectionResult = lanConnection->connect(ipAddress, PORT);
            ui->pushButtonDisconnect->hide();
            if(connectionResult.isEmpty())
            {
                state = State::LAN_CONNECTED;
                showDetectonSlabs(LAN_CONNECTION_LABEL_TEXT + ipAddress, Connection::LAN);
            }
            else
            {
                state = State::ERROR;
                QMessageBox::critical(this, "LAN Error", connectionResult);
            }
        }
        else if (ui->radioButtonUsb->isChecked())
        {
            qDebug() << "Serial nextClicked";
            QString portName = ui->comboBoxUsb->currentText();
            if(!portName.isEmpty())
            {
               serial->setPortName(portName);
               if (!serial->open(QIODevice::ReadWrite))
               {
                    state = State::ERROR;
                    QMessageBox::critical(this, "Connection error", "Unable to connect to the specified serial port");
               }
               else
               {
                    state = State::USB_CONNECTED;
                    showDetectonSlabs(USB_CONNECTION_LABEL_TEXT + portName, Connection::SERIAL);
                    if(serial->waitForReadyRead(READ_READY_SERIAL_TIME))
                    {
                        qDebug() << serial->readAll();
                    }
               }
            }
            else
            {
                QMessageBox::warning(this, "Serial port not selected",  "Serial port not selected");
            }
        }
        else
        {
            QMessageBox::critical(this, "Internal error", "The application does not work properly - report the problem to its author");
        }
    }
}


void Widget::backClicked()
{
    state = State::DISCONNECTED;
    ui->groupBoxSelectConnection->show();
    ui->groupBoxSelectConnection->setEnabled(true);
    ui->pushButtonBack->hide();
    ui->groupBoxLanConnection->hide();
    ui->groupBoxUsbConnection->hide();
}

void Widget::disconnectClicked()
{
    if(ui->radioButtonLan->isChecked())
    {
        if(lanConnection->closeConnection() < 0)
        {
            QMessageBox::critical(this, "LAN error", "Unable to send data to HUB");
        }

    }
    else
    {
        serial->close();
        ui->groupBoxDetectionSlabs->hide();
        ui->connectionLabel->hide();
        ui->pushButtonDisconnect->hide();
        ui->groupBoxSelectConnection->show();
        ui->pushButtonNext->show();
    }
}

void Widget::setVoltageClicked(QObject *button)
{
    qDebug() << qobject_cast<QPushButton*>(button)->parentWidget();
}

void Widget::disconnected()
{
    ui->groupBoxDetectionSlabs->hide();
    ui->connectionLabel->hide();
    ui->pushButtonDisconnect->hide();
    ui->groupBoxSelectConnection->show();
    ui->groupBoxSelectConnection->setEnabled(true);
    ui->pushButtonNext->show();
    state = State::DISCONNECTED;
    qDebug() << "disconnected";
}

void Widget::connectionError(QAbstractSocket::SocketError se)
{
    qDebug() << "Error: " << se;
}

void Widget::slabNumberSelection()
{
//    ui->radioButtOneSlab->hide();
//    ui->radioButtonManySlabs->hide();
    ui->radioButtonOneSlab->setEnabled(false);
    ui->radioButtonManySlabs->setEnabled(false);
    ui->pushButtonDetectionSlabsNext->hide();
    ui->pushButtonBackSlabsChoice->hide();
    if(ui->radioButtonOneSlab->isChecked())
    {
        ui->widgetAddOneSlab->show();
//        ui->labelAddSlab->show();
//        ui->lineEditAddSlab->show();
//        ui->pushButtonDetect->show();
    }
    ui->pushButtonDetectionSlabBack->show();
}

void Widget::detectionSlabsBackClicked()
{
    ui->radioButtonOneSlab->setEnabled(true);
    ui->radioButtonManySlabs->setEnabled(true);
    ui->radioButtonOneSlab->show();
    ui->radioButtonManySlabs->show();
    ui->pushButtonDetectionSlabsNext->show();
    ui->pushButtonBackSlabsChoice->show();
    ui->widgetAddOneSlab->hide();
//    ui->labelAddSlab->hide();
//    ui->pushButtonDetect->hide();
//    ui->lineEditAddSlab->hide();
}

QString Widget::appendSlabToModel()
{
    quint16 slabId = ui->lineEditAddSlab->text().toUInt();
    Slab* slab = new Slab(slabId);
    QString result = lanConnection->getSlab(slab, AfeType::Both);
    QString message;
    if(result == "OK")
    {
        QString result = model->appendSlab(slab);
        if(result != "OK")
        {
            message = "Adding detection slab error";
            QMessageBox::information(this, message, result);
            return message;
        }

        if(slab->getMaster()->getMeasuredVoltage() > 0)
        {
            slab->getMaster()->setStatusColor(StatusColor::Yellow);
        }
        else
        {
            slab->getMaster()->setStatusColor(StatusColor::Transparent);
        }

        if(slab->getSlave()->getMeasuredVoltage() > 0)
        {
            slab->getSlave()->setStatusColor(StatusColor::Yellow);
        }
        else
        {
            slab->getSlave()->setStatusColor(StatusColor::Transparent);
        }
    }
    else
    {
        message = "Error";
        QMessageBox::critical(this, message, result);
    }
    return "OK";
}

QString Widget::reloadMasterSlabToModel(Slab* slab)
{

    QString result = lanConnection->getSlab(slab, AfeType::Master);
    QString message;
    if(result == "OK")
    {
        QString result = model->reloadMasterSlab(slab);
        if(result != "OK")
        {
            message = "Adding detection slab error";
            QMessageBox::information(this, message, result);
            return message;
        }
    }
    else
    {
        message = "Error";
        QMessageBox::critical(this, message, result);
    }
    return "OK";
}

QString Widget::reloadSlaveSlabToModel(Slab *slab)
{
    QString result = lanConnection->getSlab(slab, AfeType::Slave);
    QString message;
    if(result == "OK")
    {
        QString result = model->reloadSlaveSlab(slab);
        if(result != "OK")
        {
            message = "Adding detection slab error";
            QMessageBox::information(this, message, result);
            return message;
        }
    }
    else
    {
        message = "Error";
        QMessageBox::critical(this, message, result);
    }
    return "OK";
}


QString Widget::initAndOnSlab()
{
    quint16 slabId = ui->lineEditAddSlab->text().toUInt();
    QString result = lanConnection->initSlab(slabId);
    QString message;
    if(result == "OK")
    {
        result = lanConnection->onSlab(slabId);
        if(result != "OK")
        {
            message = "Error";
            QMessageBox::critical(this, message, result);
        }
    }
    else
    {
        message = "Error";
        QMessageBox::critical(this, message, result);
    }
    return "OK";
}

void Widget::addSetWidgets()
{
    QWidget * setVoltageWidgetMaster = new QWidget();
    QLineEdit * setVoltageLineEditMaster = new QLineEdit(setVoltageWidgetMaster);
    QPushButton * setVoltageButtonMaster = new QPushButton("Set", setVoltageWidgetMaster);
    QHBoxLayout * setVoltageLayoutMaster = new QHBoxLayout(setVoltageWidgetMaster);
    setVoltageLayoutMaster->addWidget(setVoltageLineEditMaster);
    setVoltageLayoutMaster->addWidget(setVoltageButtonMaster);

    QWidget * setVoltageWidgetSlave = new QWidget();
    QLineEdit * setVoltageLineEditSlave = new QLineEdit(setVoltageWidgetSlave);
    QPushButton * setVoltageButtonSlave = new QPushButton("Set", setVoltageWidgetSlave);
    QHBoxLayout * setVoltageLayoutSlave = new QHBoxLayout(setVoltageWidgetSlave);
    setVoltageLayoutSlave->addWidget(setVoltageLineEditSlave);
    setVoltageLayoutSlave->addWidget(setVoltageButtonSlave);

    int masterRow = model->rowCount() - 2;
    QModelIndex masterSetIndex = model->index(masterRow, SET_COLUMN_INDEX);
    ui->slabsTableView->setIndexWidget(masterSetIndex, setVoltageWidgetMaster);
    connect(setVoltageButtonMaster, &QPushButton::clicked, setMasterSignalMapper, static_cast<void(QSignalMapper::*)()>(&QSignalMapper::map));
    int masterSlabId = model->data(model->index(masterRow, 0)).toInt();
    setMasterSignalMapper->setMapping(setVoltageButtonMaster, masterSlabId);

    int slaveRow = model->rowCount() - 1;
    QModelIndex slaveSetIndex = model->index(slaveRow, SET_COLUMN_INDEX);
    ui->slabsTableView->setIndexWidget(slaveSetIndex, setVoltageWidgetSlave);
    connect(setVoltageButtonSlave, &QPushButton::clicked, setSlaveSignalMapper, static_cast<void(QSignalMapper::*)()>(&QSignalMapper::map));
    int slaveSlabId = model->data(model->index(slaveRow, 0)).toInt();
    setSlaveSignalMapper->setMapping(setVoltageButtonSlave, slaveSlabId);
}

void Widget::addPowerWidgets()
{
    QWidget * powerWidgetMaster = new QWidget();
    QPushButton * powerOnButtonMaster = new QPushButton("On", powerWidgetMaster);
    QPushButton * powerOffButtonMaster = new QPushButton("Off", powerWidgetMaster);
    QHBoxLayout * powerLayoutMaster = new QHBoxLayout(powerWidgetMaster);

    powerLayoutMaster->addWidget(powerOnButtonMaster);
    int masterRow = model->rowCount() - 2;
    int masterSlabId = model->data(model->index(masterRow, 0)).toInt();

    QModelIndex masterPowerIndex = model->index(masterRow, POWER_COLUMN_INDEX);
    powerLayoutMaster->addWidget(powerOffButtonMaster);
    ui->slabsTableView->setIndexWidget(masterPowerIndex, powerWidgetMaster);

    connect(powerOnButtonMaster, &QPushButton::clicked, onMasterSignalMapper, static_cast<void(QSignalMapper::*)()>(&QSignalMapper::map));
    onMasterSignalMapper->setMapping(powerOnButtonMaster, masterSlabId);

    connect(powerOffButtonMaster, &QPushButton::clicked, offMasterSignalMapper, static_cast<void(QSignalMapper::*)()>(&QSignalMapper::map));
    offMasterSignalMapper->setMapping(powerOffButtonMaster, masterSlabId);

    QWidget * powerWidgetSlave = new QWidget();
    QPushButton * powerOnButtonSlave = new QPushButton("On", powerWidgetSlave);
    QPushButton * powerOffButtonSlave = new QPushButton("Off", powerWidgetSlave);
    QHBoxLayout * powerLayoutSlave = new QHBoxLayout(powerWidgetSlave);
    powerLayoutSlave->addWidget(powerOnButtonSlave);


    int slaveRow = model->rowCount() - 1;
    int slaveSlabId = model->data(model->index(slaveRow, 0)).toInt();

    QModelIndex slavePowerIndex = model->index(slaveRow, POWER_COLUMN_INDEX);
    powerLayoutSlave->addWidget(powerOffButtonSlave);
    ui->slabsTableView->setIndexWidget(slavePowerIndex, powerWidgetSlave);

    connect(powerOnButtonSlave, &QPushButton::clicked, onSlaveSignalMapper, static_cast<void(QSignalMapper::*)()>(&QSignalMapper::map));
    onSlaveSignalMapper->setMapping(powerOnButtonSlave, slaveSlabId);

    connect(powerOffButtonSlave, &QPushButton::clicked, offSlaveSignalMapper, static_cast<void(QSignalMapper::*)()>(&QSignalMapper::map));
    offSlaveSignalMapper->setMapping(powerOffButtonSlave, slaveSlabId);
}

void Widget::addSlab()
{
    appendSlabToModel();
    addSetWidgets();
    addPowerWidgets();
    ui->slabsTableView->show();
}

void Widget::addAndOnSlab()
{
    QString result = initAndOnSlab();
    if(result == "OK")
    {
        appendSlabToModel();
    }

    addSetWidgets();
    addPowerWidgets();

    ui->slabsTableView->show();
}

void Widget::setMasterVoltageClicked(int slabId)
{
    Slab* slab = model->findSlab(slabId);
    QWidget * setWidget = ui->slabsTableView->indexWidget(model->findIndexOfMasterSlabSetButton(slabId));
    QString setVoltageText = setWidget->findChildren<QLineEdit *>().at(0)->text();
    slab->getMaster()->setSetVoltage(setVoltageText.toFloat());
    QString result = lanConnection->setSlabVoltage(slab);
    QString message;
    if(result == "OK")
    {
        result = reloadMasterSlabToModel(slab);
        if(result != "OK")
        {
            message = "Error";
            QMessageBox::critical(this, message, result);
        }
        slab->getMaster()->setStatusColor(StatusColor::Green);
    }
    else
    {
        message = "Error";
        QMessageBox::critical(this, message, result);
    }

}

//TO DO zdebagować jak zmienia się model podczas wywoływania tej funkcji
void Widget::setSlaveVoltageClicked(int slabId)
{
    Slab* slab = model->findSlab(slabId);
    QWidget * setWidget = ui->slabsTableView->indexWidget(model->findIndexOfSlaveSlabSetButton(slabId));
    QString setVoltageText = setWidget->findChildren<QLineEdit *>().at(0)->text();
    slab->getSlave()->setSetVoltage(setVoltageText.toFloat());
    QString result = lanConnection->setSlabVoltage(slab);
    QString message;
    if(result == "OK")
    {
        result = reloadSlaveSlabToModel(slab);
        if(result != "OK")
        {
            message = "Error";
            QMessageBox::critical(this, message, result);
        }
        slab->getSlave()->setStatusColor(StatusColor::Green);
    }
    else
    {
        message = "Error";
        QMessageBox::critical(this, message, result);
    }
}

void Widget::onMasterClicked(int slabId)
{
    Slab* slab = model->findSlab(slabId);
    QString result = lanConnection->initSlab(slabId);
    QString message;
    if(result == "OK")
    {
        result = lanConnection->onSlab(slabId);
        if(result == "OK")
        {
            result = reloadMasterSlabToModel(slab);
            if(result != "OK")
            {
                message = "Error";
                QMessageBox::critical(this, message, result);
            }
        }
        else
        {
            message = "Error";
            QMessageBox::critical(this, message, result);
        }
    }
    else
    {
        message = "Error";
        QMessageBox::critical(this, message, result);
    }
}

void Widget::onSlaveClicked(int slabId)
{
    Slab* slab = model->findSlab(slabId);
    QString result = lanConnection->initSlab(slabId);
    QString message;
    if(result == "OK")
    {
        QString result = lanConnection->onSlab(slabId);
        if(result == "OK")
        {
            result = reloadSlaveSlabToModel(slab);
            if(result != "OK")
            {
                message = "Error";
                QMessageBox::critical(this, message, result);
            }
        }
        else
        {
            message = "Error";
            QMessageBox::critical(this, message, result);
        }
    }
    else
    {
        message = "Error";
        QMessageBox::critical(this, message, result);
    }
}

void Widget::offMasterClicked(int slabId)
{
    Slab* slab = model->findSlab(slabId);
    QString result = lanConnection->offSlab(slabId);
    QString message;
    if(result == "OK")
    {
        result = reloadMasterSlabToModel(slab);
        if(result != "OK")
        {
            message = "Error";
            QMessageBox::critical(this, message, result);
        }
    }
    else
    {
        message = "Error";
        QMessageBox::critical(this, message, result);
    }
}

void Widget::offSlaveClicked(int slabId)
{
    Slab* slab = model->findSlab(slabId);
    QString result = lanConnection->offSlab(slabId);
    QString message;
    if(result == "OK")
    {
        result = reloadSlaveSlabToModel(slab);
        if(result != "OK")
        {
            message = "Error";
            QMessageBox::critical(this, message, result);
        }
    }
    else
    {
        message = "Error";
        QMessageBox::critical(this, message, result);
    }
}

//void Widget::backSlabsChoiceClicked()
//{

//}

