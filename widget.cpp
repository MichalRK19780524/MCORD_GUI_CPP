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
    model = new DetectorTableModel(&Widget::HEADERS);
    ui->slabsTableView->setModel(model);

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
    connect(ui->pushButtonBackSlabsChoice, &QPushButton::clicked, this, &Widget::disconnectClicked /*&Widget::backSlabsChoiceClicked*/);
    connect(lanConnection->getSocket(), &QTcpSocket::disconnected, this, &Widget::disconnected);
    connect(lanConnection->getSocket(), &QTcpSocket::errorOccurred, this, &Widget::connectionError);
    connect(detectionSlabsWidget, &DetectionSlabsWidget::clicked, this, &Widget::setVoltageClicked);
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

    delete model;
    model = nullptr;

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

void Widget::addSlab()
{
    quint16 slabId = ui->lineEditAddSlab->text().toUInt();
//    QBoxLayout* layout = qobject_cast<QBoxLayout*>(ui->groupBoxDetectionSlabs->layout());
//    QList<QStandardItem *> items = {new QStandardItem(QString::number(slabId)), new QStandardItem("Gray")};
    Slab* slab = new Slab(slabId);
    QString result = lanConnection->getSlab(slab, AfeType::Both);
    if(result == "OK")
    {
        QString result = model->appendSlab(slab);
        if(result != "OK")
        {
            QMessageBox::information(this, "Adding detection slab error", result);
        }
    }
    else
    {
        QMessageBox::critical(this, "Error", result);
    }

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

    ui->slabsTableView->setIndexWidget(model->index(model->rowCount() - 2, model->columnCount() - 5), setVoltageWidgetMaster);
    ui->slabsTableView->setIndexWidget(model->index(model->rowCount() - 1, model->columnCount() - 5), setVoltageWidgetSlave);

    QWidget * powerWidgetMaster = new QWidget();
    QPushButton * powerOnButtonMaster = new QPushButton("On", powerWidgetMaster);
    QPushButton * powerOffButtonMaster = new QPushButton("Off", powerWidgetMaster);
    QHBoxLayout * powerLayoutMaster = new QHBoxLayout(powerWidgetMaster);
    powerLayoutMaster->addWidget(powerOnButtonMaster);
    powerLayoutMaster->addWidget(powerOffButtonMaster);

    QWidget * powerWidgetSlave = new QWidget();
    QPushButton * powerOnButtonSlave = new QPushButton("On", powerWidgetSlave);
    QPushButton * powerOffButtonSlave = new QPushButton("Off", powerWidgetSlave);
    QHBoxLayout * powerLayoutSlave = new QHBoxLayout(powerWidgetSlave);
    powerLayoutSlave->addWidget(powerOnButtonSlave);
    powerLayoutSlave->addWidget(powerOffButtonSlave);

    ui->slabsTableView->setIndexWidget(model->index(model->rowCount() - 2, model->columnCount() - 4), powerWidgetMaster);
    ui->slabsTableView->setIndexWidget(model->index(model->rowCount() - 1, model->columnCount() - 4), powerWidgetSlave);

    ui->slabsTableView->setShowGrid(false);
    ui->slabsTableView->setAlternatingRowColors(true);
    ui->slabsTableView->show();
}

//void Widget::backSlabsChoiceClicked()
//{

//}

