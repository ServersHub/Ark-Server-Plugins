#include "sellstoreitemwidget.h"
#include "ui_sellstoreitemwidget.h"

#include "internal/sellstoreitem.h"

SellStoreItemWidget::SellStoreItemWidget(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui_(new Ui::SellStoreItemWidget)
{
    ui_->setupUi(this);

    connect(ui_->submitBtn, &QPushButton::clicked, [this]() {
        this->Submit();
    });
    connect(ui_->cancelBtn, &QPushButton::clicked, [this]() {
        if(this->creation_mode_)
        {
            emit SellStoreItemWidget::CancelCreationSignal();
        }
        else
        {
            emit SellStoreItemWidget::CancelEditingSignal();
        }
        this->hide();
    });
}

SellStoreItemWidget::~SellStoreItemWidget()
{
    delete ui_;
}

void SellStoreItemWidget::Open(Shop::SellStoreItem *item_ptr, bool is_creation_mode)
{
    item_ = item_ptr;
    creation_mode_ = is_creation_mode;

    ui_->nameEdit->setText(item_->Name() ? *item_->Name() : "");
    ui_->typeEdit->setText(item_->Type() ? *item_->Type() : "");
    ui_->descEdit->setText(item_->Description() ? *item_->Description() : "");
    ui_->amountSpin->setValue(item_->Amount() ? *item_->Amount() : -1);
    ui_->priceSpin->setValue(item_->Price() ? *item_->Price() : -1);
    ui_->bpEdit->setText(item_->Blueprint() ? *item_->Blueprint() : "");
    ui_->errorLabel->setText("");

    show();
}

void SellStoreItemWidget::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    if(creation_mode_)
    {
        emit CancelCreationSignal();
    }
    else
    {
        emit CancelEditingSignal();
    }
}

void SellStoreItemWidget::Submit()
{
    const auto name = ui_->nameEdit->text();
    const auto type = "item";
    const auto desc = ui_->descEdit->text();
    const auto price = ui_->priceSpin->value();
    const auto amount = ui_->amountSpin->value();
    const auto bp = ui_->bpEdit->text();

    if(name.isEmpty())
    {
        ui_->errorLabel->setText("Name shouldn't be empty");
        return;
    }
    if(price < 0)
    {
        ui_->errorLabel->setText("Price should be positive integer");
        return;
    }
    if(amount < 1)
    {
        ui_->errorLabel->setText("Amount should be positive integer");
        return;
    }
    if(bp.isEmpty())
    {
        ui_->errorLabel->setText("Blueprint shouldn't be empty");
        return;
    }

    item_->Name() = name;
    item_->Type() = type;
    item_->Description() = desc;
    item_->Price() = price;
    item_->Amount() = amount;
    item_->Blueprint() = bp;

    if(creation_mode_)
    {
        emit SubmitCreationSignal();
    }
    else
    {
        emit SubmitEditingSignal();
    }

    hide();
}
