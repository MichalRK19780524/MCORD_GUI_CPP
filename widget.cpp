//#include <QtGui>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSerialPortInfo>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "widget.h"
#include "./ui_widget.h"
#include "detectionslabswidget.h"

const QString Widget::HUB_RESPONSE = "\"Client connected with AFE HUB ";
const QString Widget::LAN_CONNECTION_LABEL_TEXT = "Connected to IP: ";
const QString Widget::USB_CONNECTION_LABEL_TEXT = "Connected to serial port: ";
const QJsonArray Widget::CLOSE = {QString("!disconnect")};

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->pushButtonBack->hide();
    ui->groupBoxLanConnection->hide();
    ui->groupBoxUsbConnection->hide();
    ui->groupBoxDetectionSlabs->hide();
    ui->connectionLabel->hide();
    ui->pushButtonDisconnect->hide();
    socket = new QTcpSocket(this);
    serial = new QSerialPort(this);
    connect(ui->pushButtonNext, &QPushButton::clicked, this, &Widget::nextClicked);
    connect(ui->pushButtonBack, &QPushButton::clicked, this, &Widget::backClicked);
    connect(ui->pushButtonDisconnect, &QPushButton::clicked, this, &Widget::disconnectClicked);
    connect(ui->pushButtonDetectionSlabsNext, &QPushButton::clicked, this, &Widget::slubNumberSelection);
    connect(ui->pushButtonDetectionSlabsBack, &QPushButton::clicked, this, &Widget::detectionSlabsBackClicked);
    connect(ui->pushButtonAdd, &QPushButton::clicked, this, &Widget::addSlab);
    connect(socket, &QTcpSocket::disconnected, this, &Widget::disconnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &Widget::connectionError);
}

Widget::~Widget()
{
    socket->write(QJsonDocument(CLOSE).toJson(QJsonDocument::Compact));
    if(socket->waitForBytesWritten(1000))
    {
        socket->close();
    }

    serial->close();
    delete ui;
    ui = nullptr;

    delete socket;
    socket = nullptr;

    delete serial;
    serial = nullptr;

    delete detectionSlabsWidget;
    detectionSlabsWidget = nullptr;

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
    if(!ui->groupBoxSelectConnection->isHidden())
    {
        qDebug() << "first nextClicked";
        ui->groupBoxSelectConnection->hide();
        ui->pushButtonBack->show();
        if(ui->radioButtonLan->isChecked())
        {
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
            QMessageBox::critical(this, "Internal error", "Push buttons unexpected behaviour");
        }
    }
    else
    {
        if(ui->radioButtonLan->isChecked())
        {
            qDebug() << "LAN nextClicked";
            QString ipAddress = ui->lineEditLan->text();
            socket->connectToHost(QHostAddress(ipAddress), PORT);
            // TODO  W dokumentacji jest napisane, że nie zawsze działa pod Windowsem - do zastanowienia się jak taką samą funkcjonalność napisać z wykorzystaniem innych mechanizmów
            if(socket->waitForConnected(CONNECTING_TIME))
            {
                showDetectonSlabs(LAN_CONNECTION_LABEL_TEXT + ipAddress, Connection::LAN);
                if(socket->waitForReadyRead(READ_READY_LAN_TIME))
                {
                    if(QString(socket->readAll()) != HUB_RESPONSE + ipAddress + "\"")
                    {
                        QMessageBox::critical(this, "LAN reading error", "Unable to read from the HUB with specified IP address");
                    }
                    else
                    {
                        qDebug() << "read OK";
                    }

                }
                else
                {
                    QMessageBox::critical(this, "Connection error", "Unable to read data from the specified IP address");
                }
            }
            else
            {
               QMessageBox::critical(this, "Connection error", "Unable to connect to the specified IP address");
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
                    QMessageBox::critical(this, "Connection error", "Unable to connect to the specified serial port");
               }
               else
               {
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
    ui->pushButtonBack->hide();
    ui->groupBoxLanConnection->hide();
    ui->groupBoxUsbConnection->hide();
}

void Widget::disconnectClicked()
{
    if(ui->radioButtonLan->isChecked())
    {
        socket->write(QJsonDocument(CLOSE).toJson(QJsonDocument::Compact));
        if(socket->waitForBytesWritten(1000))
        {
            socket->close();
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
    ui->pushButtonNext->show();
    qDebug() << "disconnected";
}

void Widget::connectionError(QAbstractSocket::SocketError se)
{
    qDebug() << "Error: " << se;
}

void Widget::slubNumberSelection()
{
    ui->radioButtOnoneSlab->hide();
    ui->radioButtonManySlabs->hide();
    ui->pushButtonDetectionSlabsNext->hide();
    if(ui->radioButtOnoneSlab->isChecked())
    {
        ui->labelAddSlab->show();
        ui->lineEditAddSlab->show();
        ui->pushButtonAdd->show();
    }
    ui->pushButtonDetectionSlabsBack->show();
}

void Widget::detectionSlabsBackClicked()
{
    ui->radioButtOnoneSlab->show();
    ui->radioButtonManySlabs->show();
    ui->pushButtonDetectionSlabsNext->show();
    ui->pushButtonDetectionSlabsBack->hide();
    ui->labelAddSlab->hide();
    ui->pushButtonAdd->hide();
    ui->lineEditAddSlab->hide();
}

void Widget::addSlab()
{
    QBoxLayout* layout = qobject_cast<QBoxLayout*>(ui->groupBoxDetectionSlabs->layout());
    QList<Slab*> test;


    test.push_back(new Slab(12));
    if(detectionSlabsWidget == nullptr)
    {
        detectionSlabsWidget = new DetectionSlabsWidget(test, this);
        layout->addWidget(detectionSlabsWidget);
    }
    else
    {
        detectionSlabsWidget->addDetectionSlab(new Slab(13));
    }
}
//void Widget::socketReadySet()
//{
//    socketReady = true;
//}
