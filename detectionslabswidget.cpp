#include "detectionslabswidget.h"
#include "slab.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QString>

DetectionSlabsWidget::DetectionSlabsWidget(LanConnection *lanConnection, QWidget *parent): QWidget(parent),
                                                                                           lanConnection(lanConnection),
                                                                                           signalMapper(new QSignalMapper),
                                                                                           slabs(new QList<Slab*>),
                                                                                           vBoxLayout(new QVBoxLayout),
                                                                                           setId(new QSet<quint16>)
{  
}

DetectionSlabsWidget::~DetectionSlabsWidget()
{
    delete signalMapper;
    signalMapper = nullptr;
    delete slabs;
    slabs = nullptr;
    delete vBoxLayout;
    vBoxLayout = nullptr;
    delete setId;
    setId = nullptr;
}

void DetectionSlabsWidget::addDetectionSlab(Slab* slab, AfeType afeType)
{

    quint16 id = slab->getId();
    if(setId->contains(id))
    {
        QMessageBox::information(this, "Adding detection slab error", QString( "Slab number %1 has already been added.").arg(id));
    }
    else
    {
        lanConnection->downloadMeasuredVoltage(slab, afeType);
        if(slab == nullptr)
        {
            QMessageBox::critical(this, "Internal error", "Inconsistent data in the slab");
        }
        else
        {
            slabs->append(slab);
            setId->insert(id);
            QHBoxLayout *hBoxLayoutSlab = new QHBoxLayout;
            QVBoxLayout *vBoxLayoutSlab = new QVBoxLayout;
            QHBoxLayout *hBoxLayoutMaster = new QHBoxLayout;
            QHBoxLayout *hBoxLayoutSlave = new QHBoxLayout;
            QPushButton *buttonSetMaster = new QPushButton("Set");
            QPushButton *buttonSetSlave = new QPushButton("Set");
            QLabel *labelId = new QLabel(QString::number(id));
            QLabel *labelMaster = new QLabel("Master: ");
            QLabel *labelSlave = new QLabel("Slave: ");

            QLabel *labelMV = new QLabel();
            QLabel *labelSV = new QLabel();

            connect(buttonSetMaster, &QPushButton::clicked, signalMapper, static_cast<void(QSignalMapper::*)()>(&QSignalMapper::map));
            connect(buttonSetSlave, &QPushButton::clicked, signalMapper, static_cast<void(QSignalMapper::*)()>(&QSignalMapper::map));

            hBoxLayoutMaster->insertWidget(0,buttonSetMaster);
            hBoxLayoutMaster->insertWidget(1,labelMaster);

            float measuredMasterVoltage = slab->getMaster()->getMeasuredVoltage();
            if(measuredMasterVoltage > 0)
            {
                labelMV->setText("V: " + QString::number(measuredMasterVoltage));
            }
            else
            {
                labelMV->setText("V: Off");
            }

            hBoxLayoutMaster->insertWidget(1, labelMV);

            hBoxLayoutSlave->insertWidget(0,buttonSetSlave);
            hBoxLayoutSlave->insertWidget(1,labelSlave);

            float measuredSlaveVoltage = slab->getSlave()->getMeasuredVoltage();
            if(measuredSlaveVoltage > 0)
            {
                labelSV->setText("V: " + QString::number(measuredSlaveVoltage));
            }
            else
            {
                labelSV->setText("V: Off");
            }

            hBoxLayoutSlave->insertWidget(1, labelSV);

            vBoxLayoutSlab->addLayout(hBoxLayoutMaster);
            vBoxLayoutSlab->addLayout(hBoxLayoutSlave);

            hBoxLayoutSlab->addLayout(vBoxLayoutSlab);

            hBoxLayoutSlab->insertWidget(-1,labelId);

            vBoxLayout->addLayout(hBoxLayoutSlab);
            setLayout(vBoxLayout);
        }
    }
}
