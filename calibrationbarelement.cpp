#include "calibrationbarelement.h"
#include "ui_calibrationbarelement.h"

CalibrationBarElement::CalibrationBarElement(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::CalibrationBarElement)
{
    ui->setupUi(this);
}

CalibrationBarElement::~CalibrationBarElement()
{
    delete ui;
}
