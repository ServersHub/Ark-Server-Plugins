#include "mysql_widget.h"
#include "ui_mysql_widget.h"

#include "internal/mysql_configuration.h"

MySQLWidget::MySQLWidget(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui_(new Ui::MySQLWidget)
{
    ui_->setupUi(this);

    connect(ui_->submitBtn, &QPushButton::clicked, [this]() {
        if(this->data_)
        {
            this->data_->SetHost(this->ui_->hostEdit->text());
            this->data_->SetUser(this->ui_->userEdit->text());
            this->data_->SetPassword(this->ui_->passEdit->text());
            this->data_->SetDatabase(this->ui_->dbEdit->text());
        }
        this->setVisible(false);
    });
    connect(ui_->cancelBtn, &QPushButton::clicked, [this]() {
        this->setVisible(false);
    });

    ui_->hostEdit->setText("");
    ui_->userEdit->setText("");
    ui_->passEdit->setText("");
    ui_->dbEdit->setText("");
}

MySQLWidget::~MySQLWidget()
{
    delete ui_;
}

void MySQLWidget::Show(LootBox::MySQLConfiguration *data_ptr)
{
    data_ = data_ptr;
    if(data_)
    {
        ui_->hostEdit->setText(data_->GetHost());
        ui_->userEdit->setText(data_->GetUser());
        ui_->passEdit->setText(data_->GetPassword());
        ui_->dbEdit->setText(data_->GetDatabase());
    }
    setVisible(true);
}
