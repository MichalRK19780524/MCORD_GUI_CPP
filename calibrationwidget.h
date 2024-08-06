#ifndef CALIBRATIONWIDGET_H
#define CALIBRATIONWIDGET_H

#include <QWidget>
#include "basewidget.h"
#include "wizard.h"

namespace Ui {
class CalibrationWidget;
}

class CalibrationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CalibrationWidget(LanConnection *lanConnection,  QString ipAddress, QWizard &wizard, QWidget *parent = nullptr);
    ~CalibrationWidget();

private:
    Ui::CalibrationWidget *ui;
    BaseWidget* base;
    QWizard &wizard;

signals:
    void closeLanConnection();

private slots:
    void newConnectionClicked();
};

#endif // CALIBRATIONWIDGET_H
