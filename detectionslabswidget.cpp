#include "detectionslabswidget.h"
#include "slab.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QString>
#include <QLineEdit>

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

        QString errorMessage = lanConnection->downloadMeasuredVoltage(slab, afeType);
        if(errorMessage != "OK")
        {
            QMessageBox::critical(this, "Internal error", errorMessage);
        }
        else if (slab->getMaster()->getStatus() != "OK" || slab->getSlave()->getStatus() != "OK" )
        {
            QMessageBox::warning(this, "Adding detection slab error", QString( "Failed to add slab number %1 - probably it is not connected to the hub.").arg(id));
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
            QLabel *labelMaster = new QLabel("Master");
            QLabel *labelSlave = new QLabel("Slave");
            QLabel *labelStatusGrey = new QLabel("Gray");
            QLabel *labelStatusGreen = new QLabel("Green");
//            QLabel *labelStatusRed = new QLabel("Red");

            QLabel *labelMV = new QLabel();
            QLabel *labelSV = new QLabel();

            QLabel *labelMA = new QLabel();
            QLabel *labelSA = new QLabel();

            QLabel *labelMT = new QLabel();
            QLabel *labelST = new QLabel();

            connect(buttonSetMaster, &QPushButton::clicked, signalMapper, static_cast<void(QSignalMapper::*)()>(&QSignalMapper::map));
            connect(buttonSetSlave, &QPushButton::clicked, signalMapper, static_cast<void(QSignalMapper::*)()>(&QSignalMapper::map));



            float measuredMasterVoltage = slab->getMaster()->getMeasuredVoltage();
            if(measuredMasterVoltage > 0)
            {
                labelMV->setText(QString::number(measuredMasterVoltage));
            }
            else
            {
                labelMV->setText("Null");
            }

            labelMA->setText("Null");
            labelMT->setText("Null");

            hBoxLayoutMaster->insertWidget(0, labelMT);
            hBoxLayoutMaster->insertWidget(1, labelMA);
            hBoxLayoutMaster->insertWidget(2, labelMV);
            hBoxLayoutMaster->insertWidget(3, new QPushButton("Off"));
            hBoxLayoutMaster->insertWidget(4, new QPushButton("On"));
            hBoxLayoutMaster->insertWidget(5, buttonSetMaster);
            hBoxLayoutMaster->insertWidget(6, new QLineEdit);
            hBoxLayoutMaster->insertWidget(7, labelMaster);
            hBoxLayoutMaster->insertWidget(8, labelStatusGrey);


            float measuredSlaveVoltage = slab->getSlave()->getMeasuredVoltage();
            if(measuredSlaveVoltage > 0)
            {
                labelSV->setText(QString::number(measuredSlaveVoltage));
            }
            else
            {
                labelSV->setText("Null");
            }

            labelSA->setText("Null");
            labelST->setText("Null");

            hBoxLayoutSlave->insertWidget(0, labelST);
            hBoxLayoutSlave->insertWidget(1, labelSA);
            hBoxLayoutSlave->insertWidget(2, labelSV);
            hBoxLayoutSlave->insertWidget(3, new QPushButton("Off"));
            hBoxLayoutSlave->insertWidget(4, new QPushButton("On"));
            hBoxLayoutSlave->insertWidget(5, buttonSetSlave);
            hBoxLayoutSlave->insertWidget(6, new QLineEdit);
            hBoxLayoutSlave->insertWidget(7, labelSlave);
            hBoxLayoutSlave->insertWidget(8, labelStatusGreen);

            vBoxLayoutSlab->addLayout(hBoxLayoutMaster);
            vBoxLayoutSlab->addLayout(hBoxLayoutSlave);

            hBoxLayoutSlab->addLayout(vBoxLayoutSlab);

            hBoxLayoutSlab->insertWidget(-1,labelId);

            vBoxLayout->addLayout(hBoxLayoutSlab);
            setLayout(vBoxLayout);
        }
    }
}
