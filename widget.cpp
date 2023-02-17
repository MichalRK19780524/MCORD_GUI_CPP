//#include <QtGui>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSerialPortInfo>
#include <QDebug>


#include "widget.h"
#include "./ui_widget.h"
#include "detectionslabswidget.h"

const QString Widget::LAN_CONNECTION_LABEL_TEXT = "Connected to IP: ";
const QString Widget::USB_CONNECTION_LABEL_TEXT = "Connected to serial port: ";

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

    serial = new QSerialPort(this);
    detectionSlabsWidget = new DetectionSlabsWidget(lanConnection, this);
    connect(ui->pushButtonNext, &QPushButton::clicked, this, &Widget::nextClicked);
    connect(ui->pushButtonBack, &QPushButton::clicked, this, &Widget::backClicked);
    connect(ui->pushButtonDisconnect, &QPushButton::clicked, this, &Widget::disconnectClicked);
    connect(ui->pushButtonDetectionSlabsNext, &QPushButton::clicked, this, &Widget::slabNumberSelection);
    connect(ui->pushButtonDetectionSlabsBack, &QPushButton::clicked, this, &Widget::detectionSlabsBackClicked);
    connect(ui->pushButtonAdd, &QPushButton::clicked, this, &Widget::addSlab);
    connect(lanConnection->getSocket(), &QTcpSocket::disconnected, this, &Widget::disconnected);
    connect(lanConnection->getSocket(), &QTcpSocket::errorOccurred, this, &Widget::connectionError);
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

    delete detectionSlabsWidget;
    detectionSlabsWidget = nullptr;

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
    ui->pushButtonDisconnect->show();
    ui->connectionLabel->setText(labelName);
    ui->connectionLabel->show();
    ui->labelAddSlab->hide();
    ui->lineEditAddSlab->hide();
    ui->pushButtonAdd->hide();
    ui->pushButtonDetectionSlabsBack->hide();
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
    if(ui->radioButtonOneSlab->isChecked())
    {
        ui->labelAddSlab->show();
        ui->lineEditAddSlab->show();
        ui->pushButtonAdd->show();
    }
    ui->pushButtonDetectionSlabsBack->show();
}

void Widget::detectionSlabsBackClicked()
{
    ui->radioButtonOneSlab->show();
    ui->radioButtonManySlabs->show();
    ui->pushButtonDetectionSlabsNext->show();
    ui->pushButtonDetectionSlabsBack->hide();
    ui->labelAddSlab->hide();
    ui->pushButtonAdd->hide();
    ui->lineEditAddSlab->hide();
}

void Widget::addSlab()
{
    quint16 slabId = ui->lineEditAddSlab->text().toUInt();
    QBoxLayout* layout = qobject_cast<QBoxLayout*>(ui->groupBoxDetectionSlabs->layout());
    detectionSlabsWidget->addDetectionSlab(new Slab(slabId, new Simp(), new Simp()), AfeType::Both);
    layout->addWidget(detectionSlabsWidget);
}

