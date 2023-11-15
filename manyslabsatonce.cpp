#include <QWidget>
#include <QMessageBox>
#include <QTimer>
#include <QLineEdit>

#include "widget.h"
#include "statusicondelegate.h"
#include "manyslabsatonce.h"
#include "ui_manyslabsatonce.h"

QHash<QString, QList<int>>* ManySlabsAtOnce::hubsIds = new QHash<QString, QList<int>>;

ManySlabsAtOnce::ManySlabsAtOnce(LanConnection *lanConnection, /*QSettings *settings,*/ QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManySlabsAtOnce), /*settings(settings),*/ base(new BaseWidget(lanConnection))
{
    ui->setupUi(this);
    model = new DetectorTableModel(Widget::HEADERS, 8, this);
    setMasterSignalMapper = new QSignalMapper(this);
    onSignalMapper = new QSignalMapper(this);
    offSignalMapper = new QSignalMapper(this);
    setSlaveSignalMapper = new QSignalMapper(this);
    setIdSignalMapper = new QSignalMapper(this);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::darkRed);
    setAutoFillBackground(true);
    setPalette(pal);
    ui->slabsTableView->setModel(model);
    ui->slabsTableView->setItemDelegate(new StatusIconDelegate());
    ui->slabsTableView->setShowGrid(false);
    ui->slabsTableView->setAlternatingRowColors(true);
    ui->slabsTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->slabsTableView->setMinimumSize(QSize(0,0));
    ui->slabsTableView->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);


    ui->slabsTableWidget->setShowGrid(false);
    ui->slabsTableWidget->setColumnCount(Widget::HEADERS.size());
    ui->slabsTableWidget->setHorizontalHeaderLabels(Widget::HEADERS);
    ui->slabsTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->slabsTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->slabsTableWidget->setRowCount(1);

    ui->slabsTableWidget->setMinimumSize(QSize(0,0));
    ui->slabsTableWidget->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

    auto *powerAllWidget = new QWidget(this);
    auto *powerAllOnButton = new QPushButton("On All", powerAllWidget);
    connect(powerAllOnButton, &QPushButton::clicked, this, &ManySlabsAtOnce::onAllClicked);

    auto *powerAllOffButton = new QPushButton("Off All", powerAllWidget);
    connect(powerAllOffButton, &QPushButton::clicked, this, &ManySlabsAtOnce::offAllClicked);

    auto *powerAllLayout = new QHBoxLayout(powerAllWidget);
    powerAllLayout->addWidget(powerAllOnButton);
    powerAllLayout->addWidget(powerAllOffButton);
    ui->slabsTableWidget->setCellWidget(0,2, powerAllWidget);

    auto *setVoltageAllWidget = new QWidget(this);
    auto *setVoltageAllLineEdit = new QLineEdit(setVoltageAllWidget);
    auto *setVoltageAllButton = new QPushButton("Set All", setVoltageAllWidget);
    connect(setVoltageAllButton, &QPushButton::clicked, this, &ManySlabsAtOnce::setAllClicked);
    auto *setVoltageAllLayout = new QHBoxLayout(setVoltageAllWidget);
    setVoltageAllLayout->addWidget(setVoltageAllLineEdit);
    setVoltageAllLayout->addWidget(setVoltageAllButton);
    ui->slabsTableWidget->setCellWidget(0,4, setVoltageAllWidget);

    auto *chartUWidget = new QWidget(this);
    auto *chartULayout = new QHBoxLayout(chartUWidget);
    auto *chartUButton = new QPushButton("Chart U[V]", chartUWidget);
    chartULayout->addWidget(chartUButton, Qt::AlignCenter);

    ui->slabsTableWidget->setCellWidget(0,5, chartUWidget);

    auto *chartIWidget = new QWidget(this);
    auto *chartILayout = new QHBoxLayout(chartIWidget);
    auto *chartIButton = new QPushButton("Chart I[nA]", chartIWidget);
    chartILayout->addWidget(chartIButton, Qt::AlignCenter);

    ui->slabsTableWidget->setCellWidget(0,6, chartIWidget);

    auto *chartTWidget = new QWidget(this);
    auto *chartTLayout = new QHBoxLayout(chartTWidget);
    auto *chartTButton = new QPushButton("Chart T[C]", chartTWidget);
    chartTLayout->addWidget(chartTButton, Qt::AlignCenter);

    ui->slabsTableWidget->setCellWidget(0,7, chartTWidget);

    auto *consoleWidget = new QWidget(this);
    auto *consoleLayout = new QHBoxLayout(consoleWidget);
    auto *consoleButton = new QPushButton("Console", consoleWidget);
    consoleLayout->addWidget(consoleButton, Qt::AlignCenter);

    ui->slabsTableWidget->setCellWidget(0,1, consoleWidget);

    QPalette labelPalette = ui->labelSection->palette();
    labelPalette.setColor(QPalette::WindowText, Qt::white);
    ui->labelIp->setAutoFillBackground(true);
    ui->labelSection->setAutoFillBackground(true);
    ui->groupBoxDetectionSlabs->setAutoFillBackground(true);
//    palette.setColor(ui->pLabel->foregroundRole(), Qt::yellow);
    ui->labelSection->setPalette(labelPalette);
    ui->labelIp->setPalette(labelPalette);
    ui->groupBoxDetectionSlabs->setPalette(labelPalette);

    QHeaderView *verticalHeader = ui->slabsTableView->verticalHeader();
    verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader->setDefaultSectionSize(48);
    verticalHeader->setStretchLastSection(true);

    showMaximized();

    connect(ui->onHvHubPushButton, &QPushButton::clicked, lanConnection, &LanConnection::onHub);
    connect(ui->offHvHubPushButton, &QPushButton::clicked, lanConnection, &LanConnection::offHub);

    connect(this, &ManySlabsAtOnce::initializationManySlabsRequired, lanConnection, &LanConnection::initAndOnManySlabs);
    connect(lanConnection, &LanConnection::appendManySlabsToTableRequired, this, &ManySlabsAtOnce::appendManySlabsToModel);

    connect(this, &ManySlabsAtOnce::setManySlabsRequired, lanConnection, &LanConnection::setManySlabs);
    connect(lanConnection, &LanConnection::updateManySlabsToTableRequired, this, &ManySlabsAtOnce::updateManySlabsInModel);

    connect(this, &ManySlabsAtOnce::offManySlabsRequired, lanConnection, &LanConnection::offManySlabs);

    auto *updateTableTimer = new QTimer(this);
    connect(updateTableTimer, &QTimer::timeout, this, &ManySlabsAtOnce::tableUpdate);
    updateTableTimer->start(BaseWidget::UPDATE_TABLE_TIME);

    connect(this, &ManySlabsAtOnce::manySlabsUpdateRequired, lanConnection, &LanConnection::updateManySlabs);
    connect(lanConnection, &LanConnection::manySlabsDataRetrieved, this, &ManySlabsAtOnce::updateManySlabsInModel);
//    connect(setIdSignalMapper, &QSignalMapper::mappedInt, this, &ManySlabsAtOnce::idEditingFinished);

    connect(lanConnection, &LanConnection::connectionSucceeded, this, &ManySlabsAtOnce::loadIdNumbers);
    connect(this, &ManySlabsAtOnce::loadIdNumbersSucceded, lanConnection, &LanConnection::loadAllSetSipmVoltageFromHub);
    connect(lanConnection, &LanConnection::loadAllSetSipmVoltageCompleted, this, &ManySlabsAtOnce::loadSetVoltages);

    connect(setMasterSignalMapper, &QSignalMapper::mappedInt, this, &ManySlabsAtOnce::setMasterVoltageClicked);
    connect(setSlaveSignalMapper, &QSignalMapper::mappedInt, this, &ManySlabsAtOnce::setSlaveVoltageClicked);
    connect(onSignalMapper,&QSignalMapper::mappedInt, this, &ManySlabsAtOnce::onClicked);
    connect(offSignalMapper, &QSignalMapper::mappedInt, this, &ManySlabsAtOnce::offClicked);

    connect(this, &ManySlabsAtOnce::onRequired, lanConnection, &LanConnection::initAndOnExistingSlab);
    connect(this, &ManySlabsAtOnce::offRequired, lanConnection, &LanConnection::offSlab);
    connect(this, &ManySlabsAtOnce::setMasterVoltageRequired, lanConnection, &LanConnection::setMasterVoltage);
    connect(this, &ManySlabsAtOnce::setSlaveVoltageRequired, lanConnection, &LanConnection::setSlaveVoltage);
    connect(this, &ManySlabsAtOnce::destroyed, lanConnection , &LanConnection::deleteLater);

//TO DO zastanowić się, czy nie przenieść wczytywania z pliku w inne miejsce
    file.setFileName("current_slab_ids.txt");
    if(!file.open(QFile::ReadWrite | QFile::Text)){
        QMessageBox::warning(this, "Error", "File not open");
    }
    static QRegularExpression regex("\\s+");
    textIds.setDevice(&file);
    while(!textIds.atEnd()){
        textIds.readLine();
        QString hubIpAddress = textIds.readLine();
        QString idsSeries = textIds.readLine();
        QStringList idsStrings = idsSeries.split(regex);
        QList<int> idList;
        for(const QString &id : idsStrings){
            bool ok;
            idList.append(id.toInt(&ok));
            if(!ok){
                qDebug() << "Reading Ids From File Error";
//                emit readIdsFromFileError(); //Chyba nie zadziała
                break;
            }
        }
        hubsIds->insert(hubIpAddress, idList);
    }

    addIdWidgets();
    addPowerWidgets();
    addSetWidgets();

}

ManySlabsAtOnce::~ManySlabsAtOnce()
{
    emit closeLanConnection();

    delete ui;
    ui = nullptr;

    delete model;
    model = nullptr;
}

void ManySlabsAtOnce::addPowerWidgets() {
    for (int i = 0; i < model->rowCount(); i+=2) {
        auto *powerWidget = new QWidget(this);
        auto *powerOnButton = new QPushButton("On", powerWidget);
        auto *powerOffButton = new QPushButton("Off", powerWidget);
        auto *powerLayout = new QHBoxLayout(powerWidget);

        powerLayout->addWidget(powerOnButton);
//        int slabId = model->data(model->index(i, 0)).toInt();

        QModelIndex powerIndex = model->index(i, BaseWidget::POWER_COLUMN_INDEX);
        powerLayout->addWidget(powerOffButton);
        ui->slabsTableView->setIndexWidget(powerIndex, powerWidget);
        //    ui->slabsTableView->setSpan(row, POWER_COLUMN_INDEX, row + 1, POWER_COLUMN_INDEX);


        connect(powerOnButton, &QPushButton::clicked, onSignalMapper,
                static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
        onSignalMapper->setMapping(powerOnButton, i);

        connect(powerOffButton, &QPushButton::clicked, offSignalMapper,
                static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
        offSignalMapper->setMapping(powerOffButton, i);
    }
}

void ManySlabsAtOnce::addSetWidgets() {
    for (int i = 0; i < model->rowCount(); i+=2) {
        auto *setVoltageWidgetMaster = new QWidget(this);
        auto *setVoltageLineEditMaster = new QLineEdit(setVoltageWidgetMaster);
        auto *setVoltageButtonMaster = new QPushButton("Set", setVoltageWidgetMaster);
        auto *setVoltageLayoutMaster = new QHBoxLayout(setVoltageWidgetMaster);
        setVoltageLayoutMaster->addWidget(setVoltageLineEditMaster);
        setVoltageLayoutMaster->addWidget(setVoltageButtonMaster);

        auto *setVoltageWidgetSlave = new QWidget(this);
        auto *setVoltageLineEditSlave = new QLineEdit(setVoltageWidgetSlave);
        auto *setVoltageButtonSlave =
            new QPushButton("Set", setVoltageWidgetSlave);
        auto *setVoltageLayoutSlave = new QHBoxLayout(setVoltageWidgetSlave);
        setVoltageLayoutSlave->addWidget(setVoltageLineEditSlave);
        setVoltageLayoutSlave->addWidget(setVoltageButtonSlave);

        QModelIndex masterSetIndex = model->index(i, BaseWidget::SET_COLUMN_INDEX);
        ui->slabsTableView->setIndexWidget(masterSetIndex, setVoltageWidgetMaster);
        connect(setVoltageButtonMaster, &QPushButton::clicked, setMasterSignalMapper,
            static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
//        int masterSlabId = model->data(model->index(i, BaseWidget::ID_COLUMN_INDEX)).toInt();
        setMasterSignalMapper->setMapping(setVoltageButtonMaster, i);

        QModelIndex slaveSetIndex = model->index(i + 1, BaseWidget::SET_COLUMN_INDEX);
        ui->slabsTableView->setIndexWidget(slaveSetIndex, setVoltageWidgetSlave);
        connect(setVoltageButtonSlave, &QPushButton::clicked, setSlaveSignalMapper,static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
//        int slaveSlabId = model->data(model->index(i + 1, BaseWidget::ID_COLUMN_INDEX)).toInt();
        setSlaveSignalMapper->setMapping(setVoltageButtonSlave, i + 1);
    }
}

void ManySlabsAtOnce::addIdWidgets(){


    for (int i = 0; i < model->rowCount(); i+=2) {
        auto *setIdWidget = new QWidget(this);
        QModelIndex idIndex = model->index(i, BaseWidget::ID_COLUMN_INDEX);
        auto *setIdLineEditWidget = new QLineEdit(setIdWidget);
        auto *setIdPushButtonWidget = new QPushButton("Set",setIdWidget);
        auto *setIdLayout = new QHBoxLayout(setIdWidget);
//        setIdLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
//        setIdLayout->setAlignment(Qt::AlignHCenter);
//        setIdLineEditWidget->setMaximumWidth(100);

//        settings->beginGroup("ids");
//            QString readId = settings->value(QString::number(i)).toString();
//        settings->endGroup();
//        if(!readId.isEmpty()){
//            setIdLineEditWidget->setText(readId);
//        }
        setIdLayout->addWidget(setIdLineEditWidget);
        setIdLayout->addWidget(setIdPushButtonWidget);
        ui->slabsTableView->setIndexWidget(idIndex, setIdWidget);

        connect(setIdLineEditWidget, &QLineEdit::editingFinished, setIdSignalMapper,static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
        setIdSignalMapper->setMapping(setIdLineEditWidget, i);
    }
}

QList<Slab> ManySlabsAtOnce::takeSlabsIds(){
    QList<Slab> slabs;
    for (int i = 0; i < model->rowCount(); i+=2){
        QWidget* masterSlabIdWidget = ui->slabsTableView->indexWidget(model->index(i, BaseWidget::ID_COLUMN_INDEX));
        QString slabIdString = masterSlabIdWidget->findChild<QLineEdit*>()->text();
        bool ok;
        quint16 slabId = slabIdString.toUShort(&ok);
        Slab slab;
        if(ok){
            slab = Slab(slabId, std::make_shared<Sipm>(), std::make_shared<Sipm>());
        } else {
            slab = Slab(-1, std::make_shared<Sipm>(), std::make_shared<Sipm>());
        }
        slabs.append(slab);
    }
    return slabs;
}

QString ManySlabsAtOnce::getIpAddress()
{
    QHostAddress ipAddress = base->getLanConnection()->getPeerAddress();
    if(ipAddress == QHostAddress::Null){
        return QString();
    } else {
        return ipAddress.toString();
    }
}

void ManySlabsAtOnce::onAllClicked()
{
    for (int i = 0; i < model->rowCount(); i+=2){
        QWidget* masterSlabIdWidget = ui->slabsTableView->indexWidget(model->index(i, BaseWidget::ID_COLUMN_INDEX));
        QString slabIdString = masterSlabIdWidget->findChild<QLineEdit*>()->text();
        bool okId;
        slabIdString.toUShort(&okId);
        if(okId){
            QWidget* masterSetVoltageWidget = ui->slabsTableView->indexWidget(model->index(i, BaseWidget::SET_COLUMN_INDEX));
            masterSetVoltageWidget->findChild<QLineEdit*>()->setText(QString::number(Sipm::INITIAL_VOLTAGE));

            QWidget* slaveSetVoltageWidget = ui->slabsTableView->indexWidget(model->index(i + 1, BaseWidget::SET_COLUMN_INDEX));
            slaveSetVoltageWidget->findChild<QLineEdit*>()->setText(QString::number(Sipm::INITIAL_VOLTAGE));
        }
    }

    emit initializationManySlabsRequired(takeSlabsIds());
}

void ManySlabsAtOnce::offAllClicked(){
    for (int i = 0; i < model->rowCount(); i+=2){
        QWidget* masterSlabIdWidget = ui->slabsTableView->indexWidget(model->index(i, BaseWidget::ID_COLUMN_INDEX));
        QString slabIdString = masterSlabIdWidget->findChild<QLineEdit*>()->text();
        bool okId;
        slabIdString.toUShort(&okId);
        if(okId){
            QWidget* masterSetVoltageWidget = ui->slabsTableView->indexWidget(model->index(i, BaseWidget::SET_COLUMN_INDEX));
            masterSetVoltageWidget->findChild<QLineEdit*>()->setText("");

            QWidget* slaveSetVoltageWidget = ui->slabsTableView->indexWidget(model->index(i + 1, BaseWidget::SET_COLUMN_INDEX));
            slaveSetVoltageWidget->findChild<QLineEdit*>()->setText("");
        }
    }
    emit offManySlabsRequired(takeSlabsIds());
}

void ManySlabsAtOnce::setAllClicked()
{
    QList<Slab> slabs;
    bool okSet;
    QWidget *setVoltageAllWidget = ui->slabsTableWidget->cellWidget(0,4);
    QString slabSetAllString = setVoltageAllWidget->findChild<QLineEdit*>()->text();//ui->lineEditSetAll->text();
    float slabSet = slabSetAllString.toFloat(&okSet);
    for (int i = 0; i < model->rowCount(); i+=2){
        QWidget* masterSlabIdWidget = ui->slabsTableView->indexWidget(model->index(i, BaseWidget::ID_COLUMN_INDEX));
        QString slabIdString = masterSlabIdWidget->findChild<QLineEdit*>()->text();
        bool okId;
        quint16 slabId = slabIdString.toUShort(&okId);

        Slab slab;
        if(okId && okSet){
            slab = Slab(slabId, std::make_shared<Sipm>(), std::make_shared<Sipm>());
            slab.getMaster()->setSetVoltage(slabSet);
            slab.getSlave()->setSetVoltage(slabSet);

            QWidget* masterSlabSetWidget = ui->slabsTableView->indexWidget(model->index(i, BaseWidget::SET_COLUMN_INDEX));
            masterSlabSetWidget->findChild<QLineEdit*>()->setText(slabSetAllString);

            QWidget* slaveSlabSetWidget = ui->slabsTableView->indexWidget(model->index(i + 1, BaseWidget::SET_COLUMN_INDEX));
            slaveSlabSetWidget->findChild<QLineEdit*>()->setText(slabSetAllString);
        } else {
            slab = Slab(-1, std::make_shared<Sipm>(), std::make_shared<Sipm>());
        }
        slabs.append(slab);
    }
    emit setManySlabsRequired(slabs);
}

void ManySlabsAtOnce::offClicked(int rowId)
{    
    QModelIndex  index = model->index(rowId, BaseWidget::ID_COLUMN_INDEX);
    int slabId = model->data(index).toInt();
    Slab slab = model->findSlab(slabId);
    //    slab.getMaster()->setStatusColor(StatusColor::Transparent);
    //    slabStates[slabId] = SlabState::On;
    QModelIndex masterSetIndex = model->findIndexOfMasterRowSetButton(rowId);
    QWidget *masterSetWidget = ui->slabsTableView->indexWidget(masterSetIndex);
    QModelIndex slaveSetIndex = model->findIndexOfSlaveRowSetButton(rowId);
    QWidget *slaveSetWidget = ui->slabsTableView->indexWidget(slaveSetIndex);
    masterSetWidget->findChildren<QLineEdit *>().at(0)->setText("");
    slaveSetWidget->findChildren<QLineEdit *>().at(0)->setText("");
    emit offRequired(slab);
}

void ManySlabsAtOnce::appendManySlabsToModel(QList<Slab> slabs) {
    for(quint8 i = 0; i < slabs.size(); ++i){
        Slab slab = slabs[i];
        QString result = model->replaceSlab(i, slab);
        QString message;
        if (result != "OK") {
            message = "Adding detection slab error";
            QMessageBox::information(this, message, result);
        } else {
            if(slab.getId() != -1){
                setMasterStatusColor(slab);
                setSlaveStatusColor(slab);
            }
        }
    }
}

void ManySlabsAtOnce::tableUpdate()
{
    QList<Slab> slabs;
    QSet<quint16> slabIds = *(model->getSetId());
    for (quint16 slabId: slabIds) {
        Slab   slab = model->findSlab(slabId);
        slabs.append(slab);
    }
    emit manySlabsUpdateRequired(slabs);
}

void ManySlabsAtOnce::updateSlabInModel(Slab slab) {
    QString result = model->updateSlab(slab);
    QString message;
    if (result != "OK") {
        message = "Updating detection slab error";
        QMessageBox::information(this, message, result);
    } else{
        setMasterStatusColor(slab);
        setSlaveStatusColor(slab);
    }
}

void ManySlabsAtOnce::updateManySlabsInModel(QList<Slab> slabs){
    QString message;
    for(Slab slab: slabs){
        QModelIndex masterSlabSetButtonIndex = model->findIndexOfMasterSlabSetButton(slab.getId());
        QWidget* masterSlabSetWidget = ui->slabsTableView->indexWidget(masterSlabSetButtonIndex);
        QString masterSetVoltage = masterSlabSetWidget->findChild<QLineEdit*>()->text();
        if(!masterSetVoltage.isEmpty()){
            bool ok;
            slab.getMaster()->setSetVoltage(masterSetVoltage.toFloat(&ok));
            if(!ok){
                message = QString("Updating detection slab ") + QString::number(slab.getId()) + "internal error";
                qDebug() << message + " incoorect string: " + masterSetVoltage;
                QMessageBox::information(this, message, masterSetVoltage);
            }
        }

        QModelIndex slaveSlabSetButtonIndex = model->findIndexOfSlaveSlabSetButton(slab.getId());
        QWidget* slaveSlabSetWidget = ui->slabsTableView->indexWidget(slaveSlabSetButtonIndex);
        QString slaveSetVoltage = slaveSlabSetWidget->findChild<QLineEdit*>()->text();
        if(!slaveSetVoltage.isEmpty()){
            bool ok;
            slab.getSlave()->setSetVoltage(slaveSetVoltage.toFloat(&ok));
            if(!ok){
                message = QString("Updating detection slab ") + QString::number(slab.getId()) + "internal error";
                qDebug() << message + " incoorect string: " + slaveSetVoltage;
                QMessageBox::information(this, message, slaveSetVoltage);
            }
        }

        QString result = model->updateSlab(slab);
        if (result != "OK") {
            message = QString("Updating detection slab ") + QString::number(slab.getId()) + " error";
            QMessageBox::information(this, message, result);
        } else{
            setMasterStatusColor(slab);
            setSlaveStatusColor(slab);
        }
    }
}

void ManySlabsAtOnce::idEditingFinished(int position)
{
    QModelIndex idIndex = model->index(position, BaseWidget::ID_COLUMN_INDEX);
    QWidget* idWidget = ui->slabsTableView->indexWidget(idIndex);
    QLineEdit* idLineEdit = idWidget->findChildren<QLineEdit *>().at(0);
//    settings->beginGroup("ids");
//        settings->setValue(QString::number(position), idLineEdit->text());
    //    settings->endGroup();
}

void ManySlabsAtOnce::loadIdNumbers(QString ipAddress){
    QList<int> idList = hubsIds->value(ipAddress);

    for(int i = 0; i < idList.size(); ++i){
        int id = idList[i];
        Slab slab(id, std::make_shared<Sipm>(), std::make_shared<Sipm>());
        model->replaceSlab(i, slab);
    }
//    QString ipAddress = getIpAddress();
    QHash<QString, QList<int>>::iterator idsIterator;
    if(!ipAddress.isNull()){
        idsIterator = hubsIds->find(ipAddress);
    } else {
        idsIterator = hubsIds->end();
    }

    QList<int>::iterator idListIterator;
    QList<int>::iterator idListEndIterator;
    if(idsIterator != hubsIds->end()){
        idListIterator = idsIterator->begin();
        idListEndIterator = idsIterator->end();
        for (int i = 0; i < model->rowCount(); i+=2){
            if(idListIterator != idListEndIterator){
                QString id = QString::number(*idListIterator);
                QModelIndex idIndex = model->index(i, BaseWidget::ID_COLUMN_INDEX);
                QWidget* idWidget = ui->slabsTableView->indexWidget(idIndex);
                QLineEdit* idLineEdit = idWidget->findChildren<QLineEdit *>().at(0);
                idLineEdit->setText(id);
                ++idListIterator;
            }
        }
    }
    emit loadIdNumbersSucceded();
}


void ManySlabsAtOnce::loadSetVoltages(QPair<QVariantHash, QVariantHash> voltages)
{
    if(voltages.first.isEmpty() && voltages.second.isEmpty()){
        return;
    }

    QVariantHash masterVoltages = voltages.first;
    QVariantHash slaveVoltages = voltages.second;

    for (int i = 0; i < model->rowCount(); i+=2){
        QModelIndex masterIdIndex = model->index(i, BaseWidget::ID_COLUMN_INDEX);
        QWidget* masterIdWidget = ui->slabsTableView->indexWidget(masterIdIndex);
        QLineEdit* masterIdLineEdit = masterIdWidget->findChildren<QLineEdit *>().at(0);
        QString masterId = masterIdLineEdit->text();

        QModelIndex masterVoltageIndex = model->index(i, BaseWidget::SET_COLUMN_INDEX);
        QWidget* masterVoltageWidget = ui->slabsTableView->indexWidget(masterVoltageIndex);
        QLineEdit* masterVoltageLineEdit = masterVoltageWidget->findChildren<QLineEdit *>().at(0);
        masterVoltageLineEdit->setText(masterVoltages.value(masterId).toString());

        QModelIndex slaveVoltageIndex = model->index(i + 1, BaseWidget::SET_COLUMN_INDEX);
        QWidget* slaveVoltageWidget = ui->slabsTableView->indexWidget(slaveVoltageIndex);
        QLineEdit* slaveVoltageLineEdit = slaveVoltageWidget->findChildren<QLineEdit *>().at(0);
        slaveVoltageLineEdit->setText(slaveVoltages.value(masterId).toString());
    }
//    for (int i = 0; i < model->rowCount(); i+=2){
//        QVariantList::iterator masterVoltagesIterator = voltages.first.begin();
//        QVariantList::iterator masterVoltagesEndIterator = voltages.first.end();

//        if(masterVoltagesIterator != masterVoltagesEndIterator){
//            QModelIndex masterVoltageIndex = model->index(i, BaseWidget::SET_COLUMN_INDEX);
//            QWidget* masterVoltageWidget = ui->slabsTableView->indexWidget(masterVoltageIndex);
//            QLineEdit* masterVoltageLineEdit = masterVoltageWidget->findChildren<QLineEdit *>().at(0);
//            masterVoltageLineEdit->setText("do uzupelnienia");
//            ++masterVoltagesIterator;
//        }

//        QVariantList::iterator slaveVoltagesIterator = voltages.second.begin();
//        QVariantList::iterator slaveVoltagesEndIterator = voltages.second.end();
//        if(slaveVoltagesIterator != slaveVoltagesEndIterator){

//            ++slaveVoltagesIterator;
//        }
//        if(idListIterator != idListEndIterator){
//            QString id = QString::number(*idListIterator);
//            QModelIndex idIndex = model->index(i, BaseWidget::ID_COLUMN_INDEX);
//            QWidget* idWidget = ui->slabsTableView->indexWidget(idIndex);
//            QLineEdit* idLineEdit = idWidget->findChildren<QLineEdit *>().at(0);
//            idLineEdit->setText(id);
//            ++idListIterator;
//        }

}

void ManySlabsAtOnce::setMasterStatusColor(Slab &slab)
{
    std::shared_ptr<Sipm> masterSipm = slab.getMaster();
    if (masterSipm->getStatus() != "OK" || masterSipm->getMeasuredVoltage() < Widget::MINIMAL_VOLTAGE) {
        masterSipm->setStatusColor(StatusColor::Red);
    } else if (masterSipm->getMeasuredVoltage() >= Widget::MINIMAL_VOLTAGE
               && masterSipm->getMeasuredVoltage() < Widget::MINIMAL_OPERATIONAL_VOLTAGE){
        masterSipm->setStatusColor(StatusColor::Yellow);
    } else {
        masterSipm->setStatusColor(StatusColor::Green);
    }
}

void ManySlabsAtOnce::setSlaveStatusColor(Slab &slab)
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

void ManySlabsAtOnce::onClicked(int rowId) {
    QModelIndex  index = model->index(rowId, BaseWidget::ID_COLUMN_INDEX);
    int slabId = model->data(index).toInt();
    Slab slab = model->findSlab(slabId);
//    slab.getMaster()->setStatusColor(StatusColor::Yellow);
//    slabStates[slabId] = SlabState::On;
    QModelIndex masterSetIndex = model->findIndexOfMasterRowSetButton(rowId);
    QWidget *masterSetWidget = ui->slabsTableView->indexWidget(masterSetIndex);
    QModelIndex slaveSetIndex = model->findIndexOfSlaveRowSetButton(rowId);
    QWidget *slaveSetWidget = ui->slabsTableView->indexWidget(slaveSetIndex);
    masterSetWidget->findChildren<QLineEdit *>().at(0)->setText(QString::number(Sipm::INITIAL_VOLTAGE));
    slaveSetWidget->findChildren<QLineEdit *>().at(0)->setText(QString::number(Sipm::INITIAL_VOLTAGE));
    emit onRequired(slab);
}

void ManySlabsAtOnce::setMasterVoltageClicked(int rowId)
{
    QModelIndex  index = model->index(rowId, BaseWidget::ID_COLUMN_INDEX);
    int slabId = model->data(index).toInt();
    Slab slab = model->findSlab(slabId);
    QWidget *setWidget = ui->slabsTableView->indexWidget(model->findIndexOfMasterSlabSetButton(slabId));
    QString setVoltageText = setWidget->findChildren<QLineEdit*>().at(0)->text();
    bool ok;
    float masterVoltage = setVoltageText.toFloat(&ok);
    if(ok){
        slab.getMaster()->setSetVoltage(masterVoltage);
    }
    qDebug() << "Slab in Widget::setMasterVoltageClicked: "<< '\n' << "Id:" << slab.getId() << '\t' << "Set Master Voltage: " << slab.getMaster()->getSetVoltage();
    emit setMasterVoltageRequired(slab);
}

void ManySlabsAtOnce::setSlaveVoltageClicked(int rowId)
{
    QModelIndex  index = model->index(rowId, BaseWidget::ID_COLUMN_INDEX);
    int slabId = model->data(index).toInt();
    Slab slab = model->findSlab(slabId);
    QWidget *setWidget = ui->slabsTableView->indexWidget(model->findIndexOfSlaveSlabSetButton(slabId));
    QString setVoltageText = setWidget->findChildren<QLineEdit*>().at(0)->text();
    bool ok;
    float slaveVoltage = setVoltageText.toFloat(&ok);
    if(ok){
        slab.getSlave()->setSetVoltage(slaveVoltage);
    }
    qDebug() << "Slab in Widget::setMasterVoltageClicked: "<< '\n' << "Id:" << slab.getId() << '\t' << "Set Master Voltage: " << slab.getMaster()->getSetVoltage();
    emit setSlaveVoltageRequired(slab);
}
