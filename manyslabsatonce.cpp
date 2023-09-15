#include <QWidget>
#include <QMessageBox>
#include <QTimer>

#include "widget.h"
#include "statusicondelegate.h"
#include "manyslabsatonce.h"
#include "ui_manyslabsatonce.h"

ManySlabsAtOnce::ManySlabsAtOnce(LanConnection *lanConnection, QSettings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ManySlabsAtOnce), settings(settings), base(new BaseWidget(lanConnection))
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

    connect(ui->pushButtonOnAll, &QPushButton::clicked, this, &ManySlabsAtOnce::onAllClicked);
    connect(this, &ManySlabsAtOnce::initializationManySlabsRequired, lanConnection, &LanConnection::initAndOnManySlabs);
    connect(lanConnection, &LanConnection::appendManySlabsToTableRequired, this, &ManySlabsAtOnce::appendManySlabsToModel);

    connect(ui->pushButtonSetAll, &QPushButton::clicked, this, &ManySlabsAtOnce::setAllClicked);
    connect(this, &ManySlabsAtOnce::setManySlabsRequired, lanConnection, &LanConnection::setManySlabs);
    connect(lanConnection, &LanConnection::updateManySlabsToTableRequired, this, &ManySlabsAtOnce::updateManySlabsInModel);

    connect(ui->pushButtonOffAll, &QPushButton::clicked, this, &ManySlabsAtOnce::offAllClicked);
    connect(this, &ManySlabsAtOnce::offManySlabsRequired, lanConnection, &LanConnection::offManySlabs);

    auto *updateTableTimer = new QTimer(this);
    connect(updateTableTimer, &QTimer::timeout, this, &ManySlabsAtOnce::tableUpdate);
    updateTableTimer->start(BaseWidget::UPDATE_TABLE_TIME);

    connect(this, &ManySlabsAtOnce::manySlabsUpdateRequired, lanConnection, &LanConnection::updateManySlabs);
    connect(lanConnection, &LanConnection::manySlabsDataRetrieved, this, &ManySlabsAtOnce::updateManySlabsInModel);
    connect(setIdSignalMapper, &QSignalMapper::mappedInt, this, &ManySlabsAtOnce::idEditingFinished);

    addIdWidgets();
    addPowerWidgets();
    addSetWidgets();
}

ManySlabsAtOnce::~ManySlabsAtOnce()
{
    delete ui;
}

void ManySlabsAtOnce::addPowerWidgets() {
    for (int i = 0; i < model->rowCount(); i+=2) {
        auto *powerWidget = new QWidget(this);
        auto *powerOnButton = new QPushButton("On", powerWidget);
        auto *powerOffButton = new QPushButton("Off", powerWidget);
        auto *powerLayout = new QHBoxLayout(powerWidget);

        powerLayout->addWidget(powerOnButton);
        int slabId = model->data(model->index(i, 0)).toInt();

        QModelIndex powerIndex = model->index(i, BaseWidget::POWER_COLUMN_INDEX);
        powerLayout->addWidget(powerOffButton);
        ui->slabsTableView->setIndexWidget(powerIndex, powerWidget);
        //    ui->slabsTableView->setSpan(row, POWER_COLUMN_INDEX, row + 1, POWER_COLUMN_INDEX);


        connect(powerOnButton, &QPushButton::clicked, onSignalMapper,
                static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
        onSignalMapper->setMapping(powerOnButton, slabId);

        connect(powerOffButton, &QPushButton::clicked, offSignalMapper,
                static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
        offSignalMapper->setMapping(powerOffButton, slabId);
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
        int masterSlabId = model->data(model->index(i, BaseWidget::ID_COLUMN_INDEX)).toInt();
        setMasterSignalMapper->setMapping(setVoltageButtonMaster, masterSlabId);

        QModelIndex slaveSetIndex = model->index(i + 1, BaseWidget::SET_COLUMN_INDEX);
        ui->slabsTableView->setIndexWidget(slaveSetIndex, setVoltageWidgetSlave);
        connect(setVoltageButtonSlave, &QPushButton::clicked, setSlaveSignalMapper,static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
        int slaveSlabId = model->data(model->index(i + 1, BaseWidget::ID_COLUMN_INDEX)).toInt();
        setSlaveSignalMapper->setMapping(setVoltageButtonSlave, slaveSlabId);
    }
}

void ManySlabsAtOnce::addIdWidgets(){
    for (int i = 0; i < model->rowCount(); i+=2) {
        auto *setIdWidget = new QWidget(this);
        auto *setIdLayout = new QHBoxLayout(this);
        setIdLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
        setIdLayout->setAlignment(Qt::AlignHCenter);
        QModelIndex idIndex = model->index(i, BaseWidget::ID_COLUMN_INDEX);
        auto *setIdLineEditWidget = new QLineEdit(setIdWidget);
        setIdLineEditWidget->setMaximumWidth(100);
        settings->beginGroup("ids");
            QString readId = settings->value(QString::number(i)).toString();
        settings->endGroup();
        if(!readId.isEmpty()){
            setIdLineEditWidget->setText(readId);
        }
        setIdLayout->addWidget(setIdLineEditWidget);
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

void ManySlabsAtOnce::onAllClicked()
{
    emit initializationManySlabsRequired(takeSlabsIds());
}

void ManySlabsAtOnce::offAllClicked(){
    emit offManySlabsRequired(takeSlabsIds());
}

void ManySlabsAtOnce::setAllClicked()
{
    QList<Slab> slabs;
    bool okSet;
    QString slabSetString = ui->lineEditSetAll->text();
    float slabSet = slabSetString.toFloat(&okSet);
    for (int i = 0; i < model->rowCount(); i+=2){
        QWidget* masterSlabIdWidget = ui->slabsTableView->indexWidget(model->index(i, BaseWidget::ID_COLUMN_INDEX));
        QString slabIdString = masterSlabIdWidget->findChild<QLineEdit*>()->text();
        bool okId;
        quint16 slabId = slabIdString.toUShort(&okId);

//        QWidget* masterSlabSetWidget = ui->slabsTableView->indexWidget(model->index(i, BaseWidget::SET_COLUMN_INDEX));
//        QString masterSlabSetString = masterSlabSetWidget->findChild<QLineEdit*>()->text();
//        bool okMasterSet;
//        float masterSlabSet = masterSlabSetString.toFloat(&okMasterSet);

//        QWidget* slaveSlabSetWidget = ui->slabsTableView->indexWidget(model->index(i + 1, BaseWidget::SET_COLUMN_INDEX));
//        QString slaveSlabSetString = slaveSlabSetWidget->findChild<QLineEdit*>()->text();
//        bool okSlaveSet;
//        float slaveSlabSet = masterSlabSetString.toFloat(&okSlaveSet);

        Slab slab;
        if(okId && okSet){
            slab = Slab(slabId, std::make_shared<Sipm>(), std::make_shared<Sipm>());
            slab.getMaster()->setSetVoltage(slabSet);
            slab.getSlave()->setSetVoltage(slabSet);
        } else {
            slab = Slab(-1, std::make_shared<Sipm>(), std::make_shared<Sipm>());
        }
        slabs.append(slab);
    }
    emit setManySlabsRequired(slabs);
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
    for(Slab slab: slabs){
        QString result = model->updateSlab(slab);
        QString message;
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
    settings->beginGroup("ids");
        settings->setValue(QString::number(position), idLineEdit->text());
    settings->endGroup();
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
