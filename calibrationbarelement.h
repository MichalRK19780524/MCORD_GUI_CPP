#ifndef CALIBRATIONBARELEMENT_H
#define CALIBRATIONBARELEMENT_H

#include <QWidget>
#include <QFrame>

namespace Ui {
class CalibrationBarElement;
}

class CalibrationBarElement : public QFrame
{
    Q_OBJECT

public:
    explicit CalibrationBarElement(QWidget *parent = nullptr);
    ~CalibrationBarElement();

private:
    Ui::CalibrationBarElement *ui;

};

#endif // CALIBRATIONBARELEMENT_H
