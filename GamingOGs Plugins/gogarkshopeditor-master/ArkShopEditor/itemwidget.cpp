#include "itemwidget.h"
#include "ui_itemwidget.h"

#include "internal/item.h"

ItemWidget::ItemWidget(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui_(new Ui::ItemWidget)
{
    ui_->setupUi(this);

    connect(ui_->submitBtn, &QPushButton::clicked, [this]() {
        this->Submit();
    });
    connect(ui_->cancelBtn, &QPushButton::clicked, [this]() {
        if(this->creation_mode_)
        {
            emit ItemWidget::CancelCreationSignal();
        }
        else
        {
            emit ItemWidget::CancelEditingSignal();
        }
        this->hide();
    });
}

ItemWidget::~ItemWidget()
{
    delete ui_;
}

void ItemWidget::Open(Shop::Item* item_ptr, bool is_creation_mode)
{
    item_ = item_ptr;
    creation_mode_ = is_creation_mode;

    ui_->blueprintEdit->setText(item_->Blueprint() ? *item_->Blueprint() : "");
    ui_->amountSpin->setValue(item_->Amount() ? *item_->Amount() : -1);
    ui_->qualitySpin->setValue(item_->Quality() ? *item_->Quality() : -1);
    ui_->forceBpCombo->setCurrentIndex(item_->ForceBlueprint() ? *item_->ForceBlueprint() == true ? 1 : 2 : 0);
    ui_->errorLabel->setText("");

    show();
}

void ItemWidget::Submit()
{
    const auto bp = ui_->blueprintEdit->text();
    const auto amount = ui_->amountSpin->value();
    const auto quality = ui_->qualitySpin->value();
    const auto force_bp_index = ui_->forceBpCombo->currentIndex();

    if(bp.isEmpty())
    {
        ui_->errorLabel->setText("Blueprint shouldn't be empty");
        return;
    }
    if(amount <= 0)
    {
        ui_->errorLabel->setText("Amount should be positive integer");
        return;
    }
    if(quality < 0)
    {
        ui_->errorLabel->setText("Quality should be positive integer");
        return;
    }
    if(force_bp_index == 0)
    {
        ui_->errorLabel->setText("Force blueprint flag should be True or False");
        return;
    }

    item_->Blueprint() = bp;
    item_->Amount() = amount;
    item_->Quality() = quality;
    item_->ForceBlueprint() = force_bp_index == 1 ? true : false;

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

void ItemWidget::closeEvent(QCloseEvent *event)
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
