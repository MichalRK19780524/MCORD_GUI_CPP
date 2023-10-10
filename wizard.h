#ifndef WIZARD_H
#define WIZARD_H

#include <QWizard>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLineEdit>
#include <QLabel>
#include <QSettings>

#include "lanconnection.h"

namespace Ui {
class Wizard;
}

class Wizard : public QWizard
{
    Q_OBJECT

public:
    enum class Pages {SelectOneMany, SelectLanUsb, EnterIpAddress };

    explicit Wizard(QWidget *parent = nullptr);
    ~Wizard();

    LanConnection* findLanConnection(QHostAddress address);
    void insertLanConnection(QHostAddress address, LanConnection* lanConnection);
    bool containsAddress(QHostAddress address);


private:
    QHash<QHostAddress, LanConnection*> lanConnections;

private slots:
    void connectManySlabs(QString ipAddress, quint16 port);
    void connectOneSlab(QString ipAddress, quint16 port);
signals:
    void connectionRequst(QString ipAddress, quint16 port);
};

class SelectOneManyPage : public QWizardPage
{
    Q_OBJECT

public:
    SelectOneManyPage(QWidget *parent = nullptr);
    virtual ~SelectOneManyPage();

    int nextId() const override;

private:
    QButtonGroup *slabsAmountButtonGroup;
    QRadioButton *oneByOneSlabRadioButton;
    QRadioButton *manySlabsRadioButton;
};


class SelectLanUsbPage : public QWizardPage
{
    Q_OBJECT

public:
    SelectLanUsbPage(QWidget *parent = nullptr);
    virtual  ~SelectLanUsbPage();

    void initializePage() override;
    int nextId() const override;

private:
    QButtonGroup *slabsAmountButtonGroup;
    QRadioButton *oneByOneSlabRadioButton;
    QRadioButton *manySlabsRadioButton;
};


class EnterIpAddressPage : public QWizardPage
{
    Q_OBJECT

public:
    EnterIpAddressPage(QWidget *parent = nullptr);
    virtual ~EnterIpAddressPage();

    void setVisible(bool visible) override;
    void initializePage() override;
    int nextId() const override;

private:
    QButtonGroup *slabsAmountButtonGroup;
    QRadioButton *oneByOneSlabRadioButton;
    QRadioButton *manySlabsRadioButton;
    QLabel *ipLabel;
    QLineEdit *ipLineEdit;
    QSettings *settings = nullptr;

private slots:
           void connectButtonClicked(int which);

signals:
           void connectManySlabsLan(QString ipAddress, quint16 port);
           void connectOneSlabLan(QString ipAddress, quint16 port);
};
#endif // WIZARD_H
