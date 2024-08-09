#include <QMessageBox>

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
    emit closeLanConnection();
    delete ui;
    ui = nullptr;

    delete base;
    base = nullptr;
}

void CalibrationWidget::newConnectionClicked()
{
    wizard.show();
}

void CalibrationWidget::addNewBarClicked()
{
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(ui->barListWidget->layout());
    bool ok;
    QString idText = ui->addBarLineEdit->text();
    int id = idText.toInt(&ok);
    if(!ok){
        QString message{"Incorrect number format. Enter a valid value."};
        QMessageBox::information(this, message, idText);
    } else {
        Slab slab(id, std::make_shared<Sipm>(), std::make_shared<Sipm>());
        emit slabRequired(slab);
        layout->insertWidget(layout->count() - 2, new CalibrationBarElement);
    }
}
