#include<QLineEdit>
#include<QPushButton>
#include<QHBoxLayout>

#include "detectortablemodel.h"
#include "basewidget.h"

const QStringList BaseWidget::HEADERS{
                                  "Slab No.", "Status", "Power", "Type", "Set SiPM Volt.",  "U[V]", "I[nA]", "T[C]"};

LanConnection *BaseWidget::getLanConnection() const
{
    return lanConnection;
}

BaseWidget::BaseWidget(LanConnection *lanConnection, QWidget *parent)
    : QWidget{parent}, lanConnection{lanConnection}
{

}

void BaseWidget::addSetWidgets() {
    auto *setVoltageWidgetMaster = new QWidget();
    auto *setVoltageLineEditMaster = new QLineEdit(setVoltageWidgetMaster);
    auto *setVoltageButtonMaster = new QPushButton("Set", setVoltageWidgetMaster);
    auto *setVoltageLayoutMaster = new QHBoxLayout(setVoltageWidgetMaster);
    setVoltageLayoutMaster->addWidget(setVoltageLineEditMaster);
    setVoltageLayoutMaster->addWidget(setVoltageButtonMaster);

    auto *setVoltageWidgetSlave = new QWidget();
    auto *setVoltageLineEditSlave = new QLineEdit(setVoltageWidgetSlave);
    auto *setVoltageButtonSlave =
        new QPushButton("Set", setVoltageWidgetSlave);
    auto *setVoltageLayoutSlave = new QHBoxLayout(setVoltageWidgetSlave);
    setVoltageLayoutSlave->addWidget(setVoltageLineEditSlave);
    setVoltageLayoutSlave->addWidget(setVoltageButtonSlave);

    int masterRow = model->rowCount() - 2;
    QModelIndex masterSetIndex = model->index(masterRow, SET_COLUMN_INDEX);
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
}
