#include "calibrationwidget.h"
#include "ui_calibrationwidget.h"
#include "calibrationbarelement.h"

CalibrationWidget::CalibrationWidget(LanConnection *lanConnection,  QString ipAddress, QWizard &wizard, QWidget *parent)
    : QWidget(parent),
    wizard(wizard),
    ui(new Ui::CalibrationWidget), base(new BaseWidget(lanConnection))
{
    ui->setupUi(this);
    connect(ui->newConnectionPushButton, &QPushButton::clicked, this, &CalibrationWidget::newConnectionClicked);
    connect(ui->addPushButton, &QPushButton::clicked, this, &CalibrationWidget::addNewBarClicked);
}

CalibrationWidget::~CalibrationWidget()
{
    delete ui;
}

void CalibrationWidget::newConnectionClicked()
{
    wizard.show();
}

void CalibrationWidget::addNewBarClicked()
{
    auto *layout = ui->barListWidget->layout();
    // QVBoxLayout *layout = new QVBoxLayout(ui->barListWidget);
    layout->addWidget(new CalibrationBarElement);
}
