#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QThread>
#include <QGridLayout>
#include <QMessageBox>

#include "wizard.h"
#include "manyslabsatonce.h"



Wizard::Wizard(QWidget *parent) :
    QWizard(parent)
{
    EnterIpAddressPage* enterIpAddressPage = new EnterIpAddressPage;
    setPage(static_cast<int>(Pages::SelectOneMany), new SelectOneManyPage);
    setPage(static_cast<int>(Pages::SelectLanUsb), new SelectLanUsbPage);
    setPage(static_cast<int>(Pages::EnterIpAddress), enterIpAddressPage);
//    ui->setupUi(this);
    setStartId(static_cast<int>(Pages::SelectOneMany));
    setWizardStyle(ClassicStyle);

    connect(enterIpAddressPage, &EnterIpAddressPage::connectManySlabsLan, this, &Wizard::connectManySlabs);
    connect(enterIpAddressPage, &EnterIpAddressPage::connectOneSlabLan, this, &Wizard::connectOneSlab);
}

Wizard::~Wizard()
{
    for(LanConnection* connection: lanConnections){ //TODO zastanowić się nad ostrzeżeniem
        if(connection != nullptr){
            delete connection;
            connection = nullptr;
        }
    }
}

LanConnection *Wizard::findLanConnection(QHostAddress address)
{
    return lanConnections.find(address).value();
}

void Wizard::insertLanConnection(QHostAddress address, LanConnection *lanConnection)
{
    lanConnections.insert(address, lanConnection);
}

bool Wizard::containsAddress(QHostAddress address)
{
    return lanConnections.contains(address);
}

void Wizard::connectManySlabs(QString ipAddress, quint16 port)
{
//    LanConnection* lc = findLanConnection(QHostAddress(ipAddress));

    emit connectionRequst(ipAddress, LanConnection::PORT);
}

void Wizard::connectOneSlab(QString ipAddress, quint16 port)
{

}

SelectOneManyPage::SelectOneManyPage(QWidget *parent): QWizardPage(parent)
{
    setTitle(tr("Select One or Many Slabs Mode"));
    slabsAmountButtonGroup = new QButtonGroup(this);
    oneByOneSlabRadioButton = new QRadioButton(tr("&One by one"), this);
    manySlabsRadioButton = new QRadioButton(tr("&Many slabs at once"), this);
    slabsAmountButtonGroup->addButton(oneByOneSlabRadioButton);
    slabsAmountButtonGroup->addButton(manySlabsRadioButton);
    manySlabsRadioButton->setChecked(true);

    registerField("one.by.one", oneByOneSlabRadioButton);
    registerField("many.slabs", manySlabsRadioButton);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(oneByOneSlabRadioButton, 0, 0);
    layout->addWidget(manySlabsRadioButton, 0, 1);
    setLayout(layout);
}

SelectOneManyPage::~SelectOneManyPage()
{
    delete slabsAmountButtonGroup;
    slabsAmountButtonGroup = nullptr;

    delete oneByOneSlabRadioButton;
    oneByOneSlabRadioButton = nullptr;

    delete manySlabsRadioButton;
    manySlabsRadioButton = nullptr;
}

int SelectOneManyPage::nextId() const
{
    if (oneByOneSlabRadioButton->isChecked()) {
        return static_cast<int>(Wizard::Pages::SelectLanUsb);
    } else {
        return static_cast<int>(Wizard::Pages::EnterIpAddress);
    }
}

QFile ManySlabsAtOnce::file {"current_slab_ids.txt"};
QTextStream ManySlabsAtOnce::textIds(&ManySlabsAtOnce::file);
QHash<QString, std::tuple<QString, QList<int>>> *const ManySlabsAtOnce::hubsComentsAndIds = new QHash<QString, std::tuple<QString, QList<int>>>;

SelectLanUsbPage::SelectLanUsbPage(QWidget *parent): QWizardPage(parent)
{
    setTitle(tr("Select LAN or USB connection Mode"));
    slabsAmountButtonGroup = new QButtonGroup(this);
    oneByOneSlabRadioButton = new QRadioButton(tr("&One by one"), this);
    manySlabsRadioButton = new QRadioButton(tr("&Many slabs at once"), this);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(oneByOneSlabRadioButton, 0, 0);
    layout->addWidget(manySlabsRadioButton, 0, 1);
    setLayout(layout);


    if(!ManySlabsAtOnce::file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, "Error", "File not open");
    }
    static QRegularExpression regex("\\s+");
    while(!ManySlabsAtOnce::textIds.atEnd()){
        QString firstLine = ManySlabsAtOnce::textIds.readLine();
        // QStringList firstLineList = firstLine.split(regex);
        // QString section = firstLineList.last();
        QString hubIpAddress = ManySlabsAtOnce::textIds.readLine();
        QString idsSeries = ManySlabsAtOnce::textIds.readLine();
        QStringList idsStrings = idsSeries.split(regex);
        QList<int> idList;
        for(const QString &id : idsStrings){
            bool ok;
            idList.append(id.toInt(&ok));
            if(!ok){
                qDebug() << "Reading Ids From File Error";
                //                emit readIdsFromFileError(); //Chyba nie zadziała
                break;
            }
        }
        ManySlabsAtOnce::hubsComentsAndIds->insert(hubIpAddress, std::make_tuple(firstLine, idList));
    }
    ManySlabsAtOnce::file.close();
}

SelectLanUsbPage::~SelectLanUsbPage()
{
    delete slabsAmountButtonGroup;
    slabsAmountButtonGroup = nullptr;

    delete oneByOneSlabRadioButton;
    oneByOneSlabRadioButton = nullptr;

    delete manySlabsRadioButton;
    manySlabsRadioButton = nullptr;
}

void SelectLanUsbPage::initializePage()
{
    bool selectedOneByOne = field("one.by.one").toBool();
    oneByOneSlabRadioButton->setChecked(selectedOneByOne);

    bool selectedMany = field("many.slabs").toBool();
    manySlabsRadioButton->setChecked(selectedMany);
}

int SelectLanUsbPage::nextId() const
{
    return -1;
}

EnterIpAddressPage::EnterIpAddressPage(QWidget *parent): QWizardPage(parent)
{
    setTitle(tr("Enter Ip Address"));
    slabsAmountButtonGroup = new QButtonGroup(this);
    oneByOneSlabRadioButton = new QRadioButton(tr("&One by one"), this);
    manySlabsRadioButton = new QRadioButton(tr("&Many slabs at once"), this);
    slabsAmountButtonGroup->addButton(oneByOneSlabRadioButton);
    slabsAmountButtonGroup->addButton(manySlabsRadioButton);
    oneByOneSlabRadioButton->setDisabled(true);
    manySlabsRadioButton->setDisabled(true);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(oneByOneSlabRadioButton, 0, 0);
    layout->addWidget(manySlabsRadioButton, 0, 1);

    ipLabel = new QLabel("Enter the IP address of HUB: (e.g 10.7.0.130) ", this);
    ipLineEdit = new QLineEdit(this);

    QString ipRange =
        "(([ 0]+)|([ 0]*[0-9] *)|([0-9][0-9] )|([ "
        "0][0-9][0-9])|(1[0-9][0-9])|([2][0-4][0-9])|(25[0-5]))";
    QRegularExpression ipRegex("^" + ipRange + "\\." + ipRange + "\\." +
                               ipRange + "\\." + ipRange + "$");
    auto *ipValidator = new QRegularExpressionValidator(ipRegex, this);
    ipLineEdit->setValidator(ipValidator);
    ipLineEdit->setInputMask("000.000.000.000");
    ipLineEdit->setCursorPosition(0);
    settings = new QSettings("TJ3", "MCORD_GUI");

    layout->addWidget(ipLabel, 1, 0);
    layout->addWidget(ipLineEdit, 1, 1);

    setLayout(layout);
}

EnterIpAddressPage::~EnterIpAddressPage()
{
    delete slabsAmountButtonGroup;
    slabsAmountButtonGroup = nullptr;

    delete oneByOneSlabRadioButton;
    oneByOneSlabRadioButton = nullptr;

    delete manySlabsRadioButton;
    manySlabsRadioButton = nullptr;

    delete ipLabel;
    ipLabel = nullptr;

    delete ipLineEdit;
    ipLineEdit = nullptr;
}

void EnterIpAddressPage::setVisible(bool visible)
{
    QWizardPage::setVisible(visible);
    if(visible){
        wizard()->setOption(QWizard::HaveCustomButton1, true);
        connect(wizard(), &QWizard::customButtonClicked,
                this, &EnterIpAddressPage::connectButtonClicked);
    } else {
        wizard()->setOption(QWizard::HaveCustomButton1, false);
        disconnect(wizard(), &QWizard::customButtonClicked,
                   this, &EnterIpAddressPage::connectButtonClicked);
    }
}

void EnterIpAddressPage::initializePage()
{
    bool selectedOneByOne = field("one.by.one").toBool();
    oneByOneSlabRadioButton->setChecked(selectedOneByOne);

    bool selectedMany = field("many.slabs").toBool();
    manySlabsRadioButton->setChecked(selectedMany);

    wizard()->setButtonText(QWizard::CustomButton1, tr("&Connect"));
    settings->beginGroup("IP address");
    QString ipAddress =
        settings->value("LAN address", "10.7.0.130").toString();
    settings->endGroup();

    ipLineEdit->setText(ipAddress);
}

int EnterIpAddressPage::nextId() const
{
    return -1;
}

void EnterIpAddressPage::connectButtonClicked(int which)
{
    if(which == QWizard::CustomButton1){
        Wizard* wizardPointer = qobject_cast<Wizard*>(wizard());
        QTcpSocket* s = new QTcpSocket();
        LanConnection* lc = new LanConnection(s);
        QThread *lanConnectionThread = new QThread();
        s->moveToThread(lanConnectionThread);
        lc->moveToThread(lanConnectionThread);
        lanConnectionThread->start();
        QHostAddress hostAddress;
        QString ipAddress = ipLineEdit->text();
        if(hostAddress.setAddress(ipAddress)){
            if(!wizardPointer->containsAddress(hostAddress)){
                if(manySlabsRadioButton->isChecked()){
                        wizardPointer->insertLanConnection(hostAddress, lc);
                        connect(wizardPointer, &Wizard::connectionRequst, lc, &LanConnection::connect);
                        ManySlabsAtOnce* manySlabsAtOnce = new ManySlabsAtOnce(lc, ipAddress, nullptr);
                        connect(manySlabsAtOnce, &ManySlabsAtOnce::closeLanConnection, lc, &LanConnection::closeConnection);
                        manySlabsAtOnce->show();
                        emit connectManySlabsLan(ipAddress, LanConnection::PORT);
                    }
                if(oneByOneSlabRadioButton->isChecked()){
                        wizardPointer->insertLanConnection(hostAddress, lc);
                        emit connectOneSlabLan(ipAddress, LanConnection::PORT);
                }
            }else {
                qDebug() << "You are trying to connect to a Hub that you have already connected to";
            }
        } else {
            qDebug() << "Incorrect IP address";
            delete lc;
            lc = nullptr;
        }
    }
}
