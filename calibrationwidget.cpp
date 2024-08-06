#include "calibrationwidget.h"
#include "ui_calibrationwidget.h"

CalibrationWidget::CalibrationWidget(LanConnection *lanConnection,  QString ipAddress, QWizard &wizard, QWidget *parent)
    : QWidget(parent),
    wizard(wizard),
    ui(new Ui::CalibrationWidget), base(new BaseWidget(lanConnection))
{
    ui->setupUi(this);
    connect(ui->newConnectionPushButton, &QPushButton::clicked, this, &CalibrationWidget::newConnectionClicked);
}

CalibrationWidget::~CalibrationWidget()
{
    delete ui;
}

void CalibrationWidget::newConnectionClicked()
{
    wizard.show();
}
