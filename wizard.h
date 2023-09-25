#ifndef WIZARD_H
#define WIZARD_H

#include <QWizard>

namespace Ui {
class Wizard;
}

class Wizard : public QWizard
{
    Q_OBJECT

public:
    enum class Pages { Page_Intro, Page_Evaluate, Page_Register, Page_Details,
           Page_Conclusion };

    explicit Wizard(QWidget *parent = nullptr);
    ~Wizard();

private:
    Ui::Wizard *ui;
};

#endif // WIZARD_H
