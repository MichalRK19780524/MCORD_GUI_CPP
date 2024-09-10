#include "calibrationbarelement.h"
#include "ui_calibrationbarelement.h"

CalibrationBarElement::CalibrationBarElement(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::CalibrationBarElement)
{
    ui->setupUi(this);
}

CalibrationBarElement::CalibrationBarElement(int id, QWidget* parent)
     : QFrame(parent)
     , ui(new Ui::CalibrationBarElement)
{
    ui->setupUi(this);
    qDebug()<<ui->idLabel;
    ui->idLabel->setText(QString::number(id));
}

CalibrationBarElement::~CalibrationBarElement()
{
    delete ui;
}
