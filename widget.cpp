// #include <QtGui>
#include <QDebug>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSerialPortInfo>
#include <QSvgRenderer>
#include <QThread>
#include <QTimer>
#include <QDateTime>
#include <utility>

#include "./ui_widget.h"
#include "statusicondelegate.h"
#include "widget.h"

const QString Widget::LAN_CONNECTION_LABEL_TEXT = "Connected to IP: ";
const QString Widget::USB_CONNECTION_LABEL_TEXT = "Connected to serial port: ";


Widget::Widget(LanConnection *lanConnection, QWidget *parent)
    : BaseWidget(lanConnection, parent), ui(new Ui::Widget) {
    ui->setupUi(this);
    ui->groupBoxSelectConnection->hide();
//    ui->connectionTypeNextPushButton->hide();
//    ui->connectionTypeBackPushButton->hide();
    ui->widgetNextBack->hide();
    adjustSize();

    connect(ui->connectionTypeBackPushButton, &QPushButton::clicked, this, &Widget::connectionTypeBackClicked);
////    ui->pushButtonBack->hide();
////    ui->groupBoxLanConnection->hide();
////    ui->groupBoxUsbConnection->hide();
////    ui->groupBoxDetectionSlabs->hide();
////    ui->connectionLabel->hide();
////    ui->pushButtonDisconnect->hide();
////    ui->hubWidget->hide();
////    ui->groupBoxSelectConnection->hide();
////    ui->pushButtonNext->hide();
////    ui->tabWidgetConnection->hide();
//    model = new DetectorTableModel(Widget::HEADERS, this);
//    setMasterSignalMapper = new QSignalMapper(this);
//    onSignalMapper = new QSignalMapper(this);
//    offSignalMapper = new QSignalMapper(this);
//    setSlaveSignalMapper = new QSignalMapper(this);

//    QPalette pal = palette();
//    pal.setColor(QPalette::Window, Qt::darkRed);
//    setAutoFillBackground(true);
//    setPalette(pal);
////    ui->slabsTableView->setModel(model);
////    ui->slabsTableView->setItemDelegate(new StatusIconDelegate());
////    ui->slabsTableView->setShowGrid(false);
////    ui->slabsTableView->setAlternatingRowColors(true);
////    ui->slabsTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
////    ui->slabsTableView->setMinimumSize(QSize(0,0));
////    ui->slabsTableView->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
////    ui->slabsTableView->setColumnWidth(0, 5);
////    ui->groupBoxDetectionSlabs->setMinimumSize(QSize(0,0));
////    ui->groupBoxDetectionSlabs->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

////    QHeaderView *verticalHeader = ui->slabsTableView->verticalHeader();
////    verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
////    verticalHeader->setDefaultSectionSize(48);
////    verticalHeader->setStretchLastSection(false);

//    serial = new QSerialPort(this);

//    settings = new QSettings("TJ3", "MCORD_GUI");


//    file.setFileName(QString("result ") + QDateTime::currentDateTime().toString() + ".txt");

////    ui->lineEditLan->text();

//    if(!file.open(QFile::WriteOnly | QFile::Text)){
//        QMessageBox::warning(this, "Error", "File not open");
//    }
//    outTextData.setDevice(&file);

//    auto *updateTableTimer = new QTimer(this);
//    connect(updateTableTimer, &QTimer::timeout, this, &Widget::tableUpdate);
//    updateTableTimer->start(UPDATE_TABLE_TIME);

//    connect(ui->pushButtonNext, &QPushButton::clicked, this, &Widget::nextClicked);
//    connect(ui->pushButtonBack, &QPushButton::clicked, this, &Widget::backClicked);
//    connect(ui->pushButtonDisconnect, &QPushButton::clicked, this, &Widget::disconnectClicked);
    connect(ui->pushButtonDetectionSlabsNext, &QPushButton::clicked, this, &Widget::slabNumberSelection);
//    connect(ui->pushButtonDetectionSlabBack, &QPushButton::clicked, this, &Widget::detectionSlabsBackClicked);
//    connect(ui->pushButtonDetect, &QPushButton::clicked, this, &Widget::detectSlab);
//    connect(ui->pushButtonOn, &QPushButton::clicked, this, &Widget::detectAndOnSlab);
//    connect(ui->pushButtonBackSlabsChoice, &QPushButton::clicked, this, &Widget::disconnectClicked /*&Widget::backSlabsChoiceClicked*/);
//    connect(lanConnection->getSocket(), &QTcpSocket::disconnected, this, &Widget::disconnected);
//    connect(lanConnection->getSocket(), &QTcpSocket::errorOccurred, this, &Widget::connectionError);
//    connect(ui->pushButtonHubOn, &QPushButton::clicked, lanConnection, &LanConnection::onHub);
//    connect(ui->pushButtonHubOff, &QPushButton::clicked, this, &Widget::offHubClicked);
//    connect(this, &Widget::hubShutdownConfirmed, lanConnection, &LanConnection::offHub);
//    connect(setMasterSignalMapper, &QSignalMapper::mappedInt, this, &Widget::setMasterVoltageClicked);
//    connect(onSignalMapper,&QSignalMapper::mappedInt, this, &Widget::onClicked);
//    connect(offSignalMapper, &QSignalMapper::mappedInt, this, &Widget::offClicked);
//    connect(setSlaveSignalMapper, &QSignalMapper::mappedInt, this, &Widget::setSlaveVoltageClicked);
//    connect(this, &Widget::connectLan, lanConnection, &LanConnection::connect);
//    connect(lanConnection, &LanConnection::connectionSucceeded, this, &Widget::actionsAfterLanConnection);
//    connect(this, &Widget::closeLanConnection, lanConnection, &LanConnection::closeConnection);
//    connect(lanConnection, &LanConnection::writingError, this, &Widget::writingErrorLanHandler);
//    connect(this, &Widget::slabRequired, lanConnection, &LanConnection::getSlab);
//    connect(lanConnection, &LanConnection::slabReadingCompleted, this, &Widget::appendSlabToModel);
//    connect(lanConnection, &LanConnection::slabDataRetrieved, this, &Widget::updateSlabInModel);
//    connect(model, &DetectorTableModel::slabAppended, this, &Widget::addWidgetsToTable);
//    connect(this, &Widget::initializationRequired, lanConnection, &LanConnection::initAndOnNewSlab);
//    connect(this, &Widget::onRequired, lanConnection, &LanConnection::initAndOnExistingSlab);
//    connect(this, &Widget::offRequired, lanConnection, &LanConnection::offSlab);
////    connect(this, &Widget::onSlaveRequired, lanConnection, &LanConnection::initAndOnExistingSlab);
//    connect(this, &Widget::offSlaveRequired, lanConnection, &LanConnection::offSlab);
//    connect(lanConnection, &LanConnection::initFailed, this, &Widget::initFailedLanHandler);
//    connect(lanConnection, &LanConnection::onFailed, this, &Widget::onFailedLanHandler);
//    connect(lanConnection, &LanConnection::appendSlabToTableRequired, this, &Widget::refreshSlab);
//    connect(lanConnection, &LanConnection::updateSlabToTableRequired, lanConnection, &LanConnection::updateSlab);
//    connect(this, &Widget::slabUpdateRequired, lanConnection, &LanConnection::updateSlab);
//    connect(this, &Widget::setMasterVoltageRequired, lanConnection, &LanConnection::setMasterVoltage);
//    connect(lanConnection, &LanConnection::setMasterFailed, this, &Widget::setMasterFailedLanHandler);
//    connect(lanConnection, &LanConnection::setSlaveFailed, this, &Widget::setSlaveFailedLanHandler);
//    connect(lanConnection, &LanConnection::setMasterSucceeded, lanConnection, &LanConnection::updateSlab);
//    connect(this, &Widget::setSlaveVoltageRequired, lanConnection, &LanConnection::setSlaveVoltage);
//    connect(lanConnection, &LanConnection::setSlaveSucceeded, lanConnection, &LanConnection::updateSlab);
//    connect(lanConnection, &LanConnection::slabDataRetrieved, this, &Widget::saveSlabToFile);
}

Widget::~Widget() {
}

//    //    serial->close();

//    //    if(lanConnection->closeConnection() < 0)
//    //    {
//    //        QMessageBox::critical(this, "LAN error", "Unable to send data to
//    //        HUB");
//    //    }
//    emit closeLanConnection();
//    delete serial;
//    serial = nullptr;

//    //    delete detectionSlabsWidget;
//    //    detectionSlabsWidget = nullptr;

//    delete model;
//    model = nullptr;

//    delete setMasterSignalMapper;
//    setMasterSignalMapper = nullptr;

//    delete onSignalMapper;
//    onSignalMapper = nullptr;

//    delete offSignalMapper;
//    offSignalMapper = nullptr;

//    delete setSlaveSignalMapper;
//    setSlaveSignalMapper = nullptr;

//    delete ui;
//    ui = nullptr;

//    file.close();
//}

//void Widget::actionsAfterLanConnection(const QString& ipAddress) {
//    state = State::LAN_CONNECTED;
//    settings->beginGroup("IP address");
//    settings->setValue("LAN address", ipAddress);
//    settings->endGroup();
////    ui->pushButtonDisconnect->hide();
////    ui->hubWidget->show();
//    outTextData << "MCORD HUB ip address: " << ipAddress << '\n';
//    outTextData << "Date" << '\t' << "Id" << '\t' << "Master Set Voltage" << '\t' << "Master Measured Voltage" << '\t'
//                << "Master Current" << '\t' << "Master Temperature" << '\t' << "Slave Set Voltage" << '\t'
//                << "Slave Measured Voltage" << '\t' << "Slave Current" << '\t' << "Slave Temperature" << '\n';
////    showDetectonSlabs(LAN_CONNECTION_LABEL_TEXT + ipAddress, Connection::LAN);
//}

//void Widget::showDetectonSlabs(const QString& labelName, Connection connection) {
//    for (QWidget *children:
//            ui->groupBoxDetectionSlabs->findChildren<QWidget *>()) {
//        children->show();
//    }

//    ui->groupBoxDetectionSlabs->show();
//    if (connection == Connection::SERIAL) {
//        ui->groupBoxUsbConnection->hide();
//    } else {
//        ui->groupBoxLanConnection->hide();
//    }
//    ui->pushButtonBack->hide();
//    ui->pushButtonNext->hide();
//    //    ui->pushButtonDisconnect->show();
//    ui->connectionLabel->setText(labelName);
//    ui->connectionLabel->show();
//    ui->widgetAddOneSlab->hide();
//    ui->slabsTableView->hide();
//    //    ui->labelAddSlab->hide();
//    //    ui->lineEditAddSlab->hide();
//    //    ui->pushButtonDetect->hide();
//    //    ui->pushButtonOn->hide();
//    //    ui->pushButtonDetectionSlabBack->hide();
//}

//void Widget::nextClicked() {
//    qDebug() << "nextClicked";
//    ui->groupBoxSelectConnection->setEnabled(false);
//    if (state == State::DISCONNECTED) {
//        qDebug() << "first nextClicked";
//        //        ui->groupBoxSelectConnection->hide();
//        ui->pushButtonBack->show();
//        if (ui->radioButtonLan->isChecked()) {
//            state = State::LAN_SELECTED;
//            ui->groupBoxLanConnection->show();
//            QString ipRange =
//                    "(([ 0]+)|([ 0]*[0-9] *)|([0-9][0-9] )|([ "
//                    "0][0-9][0-9])|(1[0-9][0-9])|([2][0-4][0-9])|(25[0-5]))";
//            QRegularExpression ipRegex("^" + ipRange + "\\." + ipRange + "\\." +
//                                       ipRange + "\\." + ipRange + "$");
//            auto *ipValidator = new QRegularExpressionValidator(ipRegex, this);
//            ui->lineEditLan->setValidator(ipValidator);
//            ui->lineEditLan->setInputMask("000.000.000.000");
//            ui->lineEditLan->setCursorPosition(0);

//            settings->beginGroup("IP address");
//            QString ipAddress =
//                    settings->value("LAN address", "10.7.0.130").toString();
//            settings->endGroup();

//            ui->lineEditLan->setText(ipAddress);
//        } else if (ui->radioButtonUsb->isChecked()) {
//            state = State::USB_SELECTED;
//            ui->comboBoxUsb->clear();
//            ui->groupBoxUsbConnection->show();
//            QList<QString> portNames;
//            QList<QSerialPortInfo> portList = QSerialPortInfo::availablePorts();
//            std::transform(portList.cbegin(), portList.cend(),
//                           std::back_inserter(portNames),
//                           [](const QSerialPortInfo& spi) { return spi.portName(); });
//            ui->comboBoxUsb->addItems(portNames);
//        } else {
//            state = State::ERROR;
//            QMessageBox::critical(this, "Internal error",
//                                  "Push buttons unexpected behaviour");
//        }
//    } else {
//        if (ui->radioButtonLan->isChecked()) {
//            qDebug() << "LAN nextClicked";
//            QString ipAddress = ui->lineEditLan->text();
//            emit connectLan(ipAddress, PORT);
//        } else if (ui->radioButtonUsb->isChecked()) {
//            qDebug() << "Serial nextClicked";
//            QString portName = ui->comboBoxUsb->currentText();
//            if (!portName.isEmpty()) {
//                serial->setPortName(portName);
//                if (!serial->open(QIODevice::ReadWrite)) {
//                    state = State::ERROR;
//                    QMessageBox::critical(
//                            this, "Connection error",
//                            "Unable to connect to the specified serial port");
//                } else {
//                    state = State::USB_CONNECTED;
//                    showDetectonSlabs(USB_CONNECTION_LABEL_TEXT + portName,
//                                      Connection::SERIAL);
//                    if (serial->waitForReadyRead(READ_READY_SERIAL_TIME)) {
//                        qDebug() << serial->readAll();
//                    }
//                }
//            } else {
//                QMessageBox::warning(this, "Serial port not selected",
//                                     "Serial port not selected");
//            }
//        } else {
//            QMessageBox::critical(this, "Internal error",
//                                  "The application does not work properly - report "
//                                  "the problem to its author");
//        }
//    }
//}

//void Widget::backClicked() {
//    state = State::DISCONNECTED;
//    ui->groupBoxSelectConnection->show();
//    ui->groupBoxSelectConnection->setEnabled(true);
//    ui->pushButtonBack->hide();
//    ui->groupBoxLanConnection->hide();
//    ui->groupBoxUsbConnection->hide();
//}

//void Widget::disconnectClicked() {
//    if (ui->radioButtonLan->isChecked()) {
//        emit closeLanConnection();
//    } else {
//        serial->close();
//        ui->groupBoxDetectionSlabs->hide();
//        ui->connectionLabel->hide();
//        ui->pushButtonDisconnect->hide();
//        ui->groupBoxSelectConnection->show();
//        ui->pushButtonNext->show();
//    }
//}

//void Widget::disconnected() {
//    ui->groupBoxDetectionSlabs->hide();
//    ui->connectionLabel->hide();
//    ui->pushButtonDisconnect->hide();
//    ui->groupBoxSelectConnection->show();
//    ui->groupBoxSelectConnection->setEnabled(true);
//    ui->pushButtonNext->show();
//    state = State::DISCONNECTED;
//    qDebug() << "disconnected";
//}

//void Widget::connectionError(QAbstractSocket::SocketError se) {
//    qDebug() << "Error: " << se;
//}

void Widget::slabNumberSelection() {
//        ui->radioButtOneSlab->hide();
//    //    ui->radioButtonManySlabs->hide();
//    ui->radioButtonOneSlab->setEnabled(false);
//    ui->radioButtonManySlabs->setEnabled(false);
//    ui->pushButtonDetectionSlabsNext->hide();
//    if (ui->radioButtonOneSlab->isChecked()) {
////        ui->pushButtonBackSlabsChoice->hide();
//        ui->widgetAddOneSlab->show();
//        //        ui->labelAddSlab->show();
//        //        ui->lineEditAddSlab->show();
//        //        ui->pushButtonDetect->show();
//    } else if(ui->radioButtonManySlabs->isChecked()){
//        manySlabsAtOnceDialog = new ManySlabsAtOnce(lanConnection, this);
//        manySlabsAtOnceDialog->show();
//    } else {
//        qDebug() << "Internal error in slabNumberSelection";
//    }
//    ui->pushButtonDetectionSlabBack->show();
    if (ui->radioButtonOneSlab->isChecked()){
        ui->pushButtonDetectionSlabsNext->hide();
        ui->groupBoxSelectConnection->show();
        ui->connectionTypeNextPushButton->show();
        ui->widgetNextBack->show();
        ui->slabsNumberFrame->setEnabled(false);
    }else if(ui->radioButtonManySlabs->isChecked()){
                manySlabsAtOnceDialog = new ManySlabsAtOnce(lanConnection, this);
                manySlabsAtOnceDialog->show();
    }
}

void Widget::connectionTypeBackClicked()
{
    ui->pushButtonDetectionSlabsNext->show();
    ui->groupBoxSelectConnection->hide();
    ui->connectionTypeNextPushButton->hide();
    ui->widgetNextBack->hide();
    ui->slabsNumberFrame->setEnabled(true);
    adjustSize();
}

//void Widget::detectionSlabsBackClicked() {
//    ui->radioButtonOneSlab->setEnabled(true);
//    ui->radioButtonManySlabs->setEnabled(true);
//    ui->radioButtonOneSlab->show();
//    ui->radioButtonManySlabs->show();
//    ui->pushButtonDetectionSlabsNext->show();
////    ui->pushButtonBackSlabsChoice->show();
////    ui->widgetAddOneSlab->hide();
//    //    ui->labelAddSlab->hide();
//    //    ui->pushButtonDetect->hide();
//    //    ui->lineEditAddSlab->hide();
//}

//void Widget::setMasterStatusColor(Slab &slab)
//{
//    std::shared_ptr<Sipm> masterSipm = slab.getMaster();
//    if (masterSipm->getStatus() != "OK" || masterSipm->getMeasuredVoltage() < Widget::MINIMAL_VOLTAGE) {
//        masterSipm->setStatusColor(StatusColor::Red);
//    } else if (masterSipm->getMeasuredVoltage() >= Widget::MINIMAL_VOLTAGE
//                && masterSipm->getMeasuredVoltage() < Widget::MINIMAL_OPERATIONAL_VOLTAGE){
//        masterSipm->setStatusColor(StatusColor::Yellow);
//    } else {
//        masterSipm->setStatusColor(StatusColor::Green);
//    }
//}

void Widget::setSlaveStatusColor(Slab & slab)
{
    std::shared_ptr<Sipm> slaveSipm = slab.getSlave();
    if (slaveSipm->getStatus() != "OK" || slaveSipm->getMeasuredVoltage() < Widget::MINIMAL_VOLTAGE) {
        slaveSipm->setStatusColor(StatusColor::Red);
    } else if (slaveSipm->getMeasuredVoltage() >= Widget::MINIMAL_VOLTAGE && slaveSipm->getMeasuredVoltage() < Widget::MINIMAL_OPERATIONAL_VOLTAGE) {
        slaveSipm->setStatusColor(StatusColor::Yellow);
    } else {
        slaveSipm->setStatusColor(StatusColor::Green);
    }
}

//void Widget::appendSlabToModel(Slab slab) {
//    QString result = model->appendSlab(slab);
//    QString message;
//    if (result != "OK") {
//        message = "Adding detection slab error";
//        QMessageBox::information(this, message, result);
//    } else {
//        setMasterStatusColor(slab);
//        setSlaveStatusColor(slab);
//    }
//}

//void Widget::updateSlabInModel(Slab slab) {
//    QString result = model->updateSlab(slab);
//    QString message;
//    if (result != "OK") {
//        message = "Updating detection slab error";
//        QMessageBox::information(this, message, result);
//    } else{
//        setMasterStatusColor(slab);
//        setSlaveStatusColor(slab);
//    }
//}

//void Widget::addWidgetsToTable(quint16 id) {
//    addSetWidgets();
//    addPowerWidgets();
//    int rowCount = ui->slabsTableView->model()->rowCount();
//    QModelIndex masterSetIndex = model->index(rowCount - 2, SET_COLUMN_INDEX);
//    QWidget *masterSetWidget = ui->slabsTableView->indexWidget(masterSetIndex);
//    QModelIndex slaveSetIndex = model->index(rowCount - 1, SET_COLUMN_INDEX);
//    QWidget *slaveSetWidget = ui->slabsTableView->indexWidget(slaveSetIndex);
//    if (state == State::LAN_SLAB_DETECTING) {
//        float masterSetVoltage = model->findSlab(id).getMaster()->getSetVoltage();
//        float slaveSetVoltage = model->findSlab(id).getSlave()->getSetVoltage();
//        if(masterSetVoltage != 0.0f){
//            masterSetWidget->findChildren<QLineEdit *>().at(0)->setText(QString::number(masterSetVoltage));
//        } else {
//            masterSetWidget->findChildren<QLineEdit *>().at(0)->setText("");
//        }
//        if(slaveSetVoltage != 0.0f){
//            slaveSetWidget->findChildren<QLineEdit *>().at(0)->setText(QString::number(slaveSetVoltage));
//        } else {
//            slaveSetWidget->findChildren<QLineEdit *>().at(0)->setText("");
//        }
//        slabStates[id] = SlabState::Detected;
//    } else if (state == State::LAN_SLAB_INITIALIZING) {
//        masterSetWidget->findChildren<QLineEdit *>().at(0)->setText(QString::number(Sipm::INITIAL_VOLTAGE));
//        slaveSetWidget->findChildren<QLineEdit *>().at(0)->setText(QString::number(Sipm::INITIAL_VOLTAGE));
//        slabStates[id] = SlabState::On;
//    }
//    ui->slabsTableView->show();
//    ui->pushButtonDetect->setDisabled(false);
//    ui->pushButtonOn->setDisabled(false);
//    ui->pushButtonDetectionSlabBack->setDisabled(false);
//    ui->slabsTableView->show();
//}

//void Widget::addSetWidgets() {
//    auto *setVoltageWidgetMaster = new QWidget(this);
//    auto *setVoltageLineEditMaster = new QLineEdit(setVoltageWidgetMaster);
//    auto *setVoltageButtonMaster = new QPushButton("Set", setVoltageWidgetMaster);
//    auto *setVoltageLayoutMaster = new QHBoxLayout(setVoltageWidgetMaster);
//    setVoltageLayoutMaster->addWidget(setVoltageLineEditMaster);
//    setVoltageLayoutMaster->addWidget(setVoltageButtonMaster);

//    auto *setVoltageWidgetSlave = new QWidget(this);
//    auto *setVoltageLineEditSlave = new QLineEdit(setVoltageWidgetSlave);
//    auto *setVoltageButtonSlave =
//            new QPushButton("Set", setVoltageWidgetSlave);
//    auto *setVoltageLayoutSlave = new QHBoxLayout(setVoltageWidgetSlave);
//    setVoltageLayoutSlave->addWidget(setVoltageLineEditSlave);
//    setVoltageLayoutSlave->addWidget(setVoltageButtonSlave);

//    int masterRow = model->rowCount() - 2;
//    QModelIndex masterSetIndex = model->index(masterRow, SET_COLUMN_INDEX);
//    ui->slabsTableView->setIndexWidget(masterSetIndex, setVoltageWidgetMaster);
//    connect(setVoltageButtonMaster, &QPushButton::clicked, setMasterSignalMapper,
//            static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
//    int masterSlabId = model->data(model->index(masterRow, 0)).toInt();
//    setMasterSignalMapper->setMapping(setVoltageButtonMaster, masterSlabId);

//    int slaveRow = model->rowCount() - 1;
//    QModelIndex slaveSetIndex = model->index(slaveRow, SET_COLUMN_INDEX);
//    ui->slabsTableView->setIndexWidget(slaveSetIndex, setVoltageWidgetSlave);
//    connect(setVoltageButtonSlave, &QPushButton::clicked, setSlaveSignalMapper,static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
//    int slaveSlabId = model->data(model->index(slaveRow, 0)).toInt();
//    setSlaveSignalMapper->setMapping(setVoltageButtonSlave, slaveSlabId);
//}

//void Widget::addPowerWidgets() {
//    auto *powerWidget = new QWidget(this);
//    auto *powerOnButton = new QPushButton("On", powerWidget);
//    auto *powerOffButton = new QPushButton("Off", powerWidget);
//    auto *powerLayout = new QHBoxLayout(powerWidget);

//    powerLayout->addWidget(powerOnButton);
//    int row = model->rowCount() - 2;
//    int slabId = model->data(model->index(row, 0)).toInt();

//    QModelIndex powerIndex = model->index(row, POWER_COLUMN_INDEX);
//    powerLayout->addWidget(powerOffButton);
//    ui->slabsTableView->setIndexWidget(powerIndex, powerWidget);
////    ui->slabsTableView->setSpan(row, POWER_COLUMN_INDEX, row + 1, POWER_COLUMN_INDEX);


//    connect(powerOnButton, &QPushButton::clicked, onSignalMapper,
//            static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
//    onSignalMapper->setMapping(powerOnButton, slabId);

//    connect(powerOffButton, &QPushButton::clicked, offSignalMapper,
//            static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
//    offSignalMapper->setMapping(powerOffButton, slabId);
//}

//void Widget::detectSlab() {
//    quint16 slabId = ui->lineEditAddSlab->text().toUInt();
//    state = State::LAN_SLAB_DETECTING;
//    ui->pushButtonDetect->setDisabled(true);
//    ui->pushButtonOn->setDisabled(true);
//    ui->pushButtonDetectionSlabBack->setDisabled(true);
//    Slab slab(slabId, std::make_shared<Sipm>(), std::make_shared<Sipm>());
//    emit slabRequired(slab, AfeType::Both);
//}

//void Widget::detectAndOnSlab() {
//    quint16 slabId = ui->lineEditAddSlab->text().toUInt();
//    state = State::LAN_SLAB_INITIALIZING;
//    ui->pushButtonDetect->setDisabled(true);
//    ui->pushButtonOn->setDisabled(true);
//    ui->pushButtonDetectionSlabBack->setDisabled(true);
//    Slab slab(slabId, std::make_shared<Sipm>(), std::make_shared<Sipm>());
//    emit initializationRequired(slab);
//}

//void Widget::onHubClicked()
//{

//}

//void Widget::offHubClicked()
//{
//    if(QMessageBox::Yes == QMessageBox::warning(this,
//                                                 "Warning", "Are you sure you want to power down the HUB?\n"
//                                                 "If you do this, you will turn off all slabs connected to it.",
//                                                 QMessageBox::Yes | QMessageBox::No)){
//        emit hubShutdownConfirmed();
//    }
//}

//void Widget::writingErrorLanHandler(const QJsonArray &command) {
//    if (command[0].toString() == LanConnection::CLOSE[0].toString()) {
//        QMessageBox::critical(this, "LAN error",
//                              "Unable to send data to close connection to HUB");
//    }
//}

//void Widget::setMasterVoltageClicked(int slabId) {
//    state = State::LAN_SLAB_SETTING;
//    Slab slab = model->findSlab(slabId);
////    QWidget *setWidget = ui->slabsTableView->indexWidget(model->findIndexOfMasterSlabSetButton(slabId));
////    QString setVoltageText = setWidget->findChildren<QLineEdit*>().at(0)->text();
////    slab.getMaster()->setSetVoltage(setVoltageText.toFloat());
//    if(slabStates[slabId] != SlabState::SetSlave && slabStates[slabId] != SlabState::SetAll){
//        slabStates[slabId] = SlabState::SetMaster;
//    } else {
//        slabStates[slabId] = SlabState::SetAll;
//    }
//    qDebug() << "Slab in Widget::setMasterVoltageClicked: "<< '\n' << "Id:" << slab.getId() << '\t' << "Set Master Voltage: " << slab.getMaster()->getSetVoltage();
//    emit setMasterVoltageRequired(slab);
//}

// TO DO zdebagować jak zmienia się model podczas wywoływania tej funkcji
//void Widget::setSlaveVoltageClicked(int slabId) {
//    state = State::LAN_SLAB_SETTING;
//        Slab slab = model->findSlab(slabId);
////        QWidget *setWidget = ui->slabsTableView->indexWidget(model->findIndexOfSlaveSlabSetButton(slabId));
////        QString setVoltageText = setWidget->findChildren<QLineEdit*>().at(0)->text();
////        slab.getSlave()->setSetVoltage(setVoltageText.toFloat());
//        if(slabStates[slabId] != SlabState::SetMaster && slabStates[slabId] != SlabState::SetAll){
//            slabStates[slabId] = SlabState::SetSlave;
//        } else {
//            slabStates[slabId] = SlabState::SetAll;
//        }
//        emit setSlaveVoltageRequired(slab);
//}

//void Widget::onClicked(int slabId) {
//    state = State::LAN_SLAB_INITIALIZING;
//    Slab slab = model->findSlab(slabId);
//    slab.getMaster()->setStatusColor(StatusColor::Yellow);
//    slabStates[slabId] = SlabState::On;
//    QModelIndex masterSetIndex = model->findIndexOfMasterSlabSetButton(slabId);
////    QWidget *masterSetWidget = ui->slabsTableView->indexWidget(masterSetIndex);
//    QModelIndex slaveSetIndex = model->findIndexOfSlaveSlabSetButton(slabId);
////    QWidget *slaveSetWidget = ui->slabsTableView->indexWidget(slaveSetIndex);
////    masterSetWidget->findChildren<QLineEdit *>().at(0)->setText(QString::number(Sipm::INITIAL_VOLTAGE));
////    slaveSetWidget->findChildren<QLineEdit *>().at(0)->setText(QString::number(Sipm::INITIAL_VOLTAGE));
//    emit onRequired(slab);
//}

//void Widget::offClicked(int slabId) {
//    state = State::LAN_SLAB_OFF;
//    Slab slab = model->findSlab(slabId);
//    slab.getMaster()->setStatusColor(StatusColor::Transparent);
//    slabStates[slabId] = SlabState::Off;
//    QModelIndex masterSetIndex = model->findIndexOfMasterSlabSetButton(slabId);
////    QWidget *masterSetWidget = ui->slabsTableView->indexWidget(masterSetIndex);
//    QModelIndex slaveSetIndex = model->findIndexOfSlaveSlabSetButton(slabId);
////    QWidget *slaveSetWidget = ui->slabsTableView->indexWidget(slaveSetIndex);
////    masterSetWidget->findChildren<QLineEdit *>().at(0)->setText("");
////    slaveSetWidget->findChildren<QLineEdit *>().at(0)->setText("");
//    emit offRequired(slab);
//}

//void Widget::initFailedLanHandler(quint16 id, const QString &message) {
//    QMessageBox::critical(this, "Init slab error", QString("Slab: ") + id + "Message: " + message);
//}

//void Widget::setMasterFailedLanHandler(quint16 id, const QString &message){
//    QMessageBox::critical(this, "Set master slab error", QString("Slab: ") + id + "Message: " + message);
//}

//void Widget::setSlaveFailedLanHandler(quint16 id, const QString &message){
//    QMessageBox::critical(this, "Set slave slab error", QString("Slab: ") + id + "Message: " + message);
//}

//void Widget::onFailedLanHandler(quint16 id, const QString &message) {
//    QMessageBox::critical(this, "On slab error", QString("Slab: ") + id + "Message: " + message);
//}

//void Widget::refreshSlab(Slab slab) {
//    emit slabRequired(slab, AfeType::Both);
//}

//void Widget::tableUpdate() {
//    QSet<quint16> slabIds = *(model->getSetId());
//    for (quint16 slabId: slabIds) {
//        Slab   slab = model->findSlab(slabId);
//        emit slabUpdateRequired(slab);
//    }
//}

//void Widget::saveSlabToFile(Slab slab)
//{
//    int id = slab.getId();
//    outTextData << QDateTime::currentDateTime().toString() << '\t' << id << '\t';
//    if(slabStates[id] == SlabState::SetMaster || slabStates[id] == SlabState::On)
//    {
//        outTextData << slab.getMaster()->getSetVoltage() << '\t'
//                << slab.getMaster()->getMeasuredVoltage() << '\t'
//                << slab.getMaster()->getCurrent() << '\t'
//                << slab.getMaster()->getTemperature() << '\t'
//                << "Null" << '\t'
//                << slab.getSlave()->getMeasuredVoltage() << '\t'
//                << slab.getSlave()->getCurrent() << '\t'
//                << slab.getSlave()->getTemperature() << '\t'
//                << '\n';
//    }
//    else if(slabStates[id] == SlabState::SetSlave || slabStates[id] == SlabState::On)
//    {
//        outTextData << slab.getMaster()->getSetVoltage() << '\t'
//                << slab.getMaster()->getMeasuredVoltage() << '\t'
//                << slab.getMaster()->getCurrent() << '\t'
//                << slab.getMaster()->getTemperature() << '\t'
//                << slab.getSlave()->getSetVoltage() << '\t'
//                << slab.getSlave()->getMeasuredVoltage() << '\t'
//                << slab.getSlave()->getCurrent() << '\t'
//                << slab.getSlave()->getTemperature() << '\t'
//                << '\n';
//    } else if(slabStates[id] == SlabState::SetAll || slabStates[id] == SlabState::On){
//        outTextData << "Null" << '\t'
//                    << slab.getMaster()->getMeasuredVoltage() << '\t'
//                    << slab.getMaster()->getCurrent() << '\t'
//                    << slab.getMaster()->getTemperature() << '\t'
//                    << slab.getSlave()->getSetVoltage() << '\t'
//                    << slab.getSlave()->getMeasuredVoltage() << '\t'
//                    << slab.getSlave()->getCurrent() << '\t'
//                    << slab.getSlave()->getTemperature() << '\t'
//                    << '\n';
//    } else {
//        outTextData << "Null" << '\t'
//                    << slab.getMaster()->getMeasuredVoltage() << '\t'
//                    << slab.getMaster()->getCurrent() << '\t'
//                    << slab.getMaster()->getTemperature() << '\t'
//                    << "Null" << '\t'
//                    << slab.getSlave()->getMeasuredVoltage() << '\t'
//                    << slab.getSlave()->getCurrent() << '\t'
//                    << slab.getSlave()->getTemperature() << '\t'
//                    << '\n';
//    }
//}


// void Widget::backSlabsChoiceClicked()
//{

//}
