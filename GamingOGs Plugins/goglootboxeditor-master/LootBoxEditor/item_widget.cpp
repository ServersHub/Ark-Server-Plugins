#include "item_widget.h"
#include "ui_item_widget.h"

#include "internal/node.h"
#include "internal/item.h"
#include "internal/json_utils.h"

ItemWidget::ItemWidget(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui_(new Ui::ItemWidget),
    item_(nullptr),
    is_creation_mode_(false)
{
    ui_->setupUi(this);
    connect(ui_->submitButton, &QPushButton::clicked, [this]() { this->Submit(); });
    connect(ui_->cancelButton, &QPushButton::clicked, [this]() { this->Cancel(false); });
}

ItemWidget::~ItemWidget()
{
    delete ui_;
}

void ItemWidget::Open(LootBox::Item* item, bool is_creation)
{
    item_ = item;
    is_creation_mode_ = is_creation;

    if(is_creation)
    {
        setWindowTitle("Add item");
        ResetUI();
    }
    else
    {
        setWindowTitle("Edit item");
        ResetUI();
        ui_->blueprintEdit->setText(item->GetBP());
        ui_->amountSpin->setValue(item->GetAmount());
        ui_->minQualitySpin->setValue(item->GetMinQuality());
        ui_->maxQualitySpin->setValue(item->GetMaxQuality());
        ui_->addStatSpin->setValue(item->GetAddStatFlag());
        ui_->blueprintChanceSpin->setValue(item->GetBPChance());
    }

    show();
}

void ItemWidget::Submit()
{
    const QString bp = LootBox::Utils::UnescapeString(ui_->blueprintEdit->toPlainText());
    const int amount = ui_->amountSpin->value();
    const int min_quality = ui_->minQualitySpin->value();
    const int max_quality = ui_->maxQualitySpin->value();
    const int add_stat = ui_->addStatSpin->value();
    const int bp_chance = ui_->blueprintChanceSpin->value();

    bool is_valid = true;

    if(bp.isEmpty())
    {
        ui_->tooltipLabel->setText("Blueprint can't be empty");
        is_valid = false;
    }
    else if(amount <= 0)
    {
        ui_->tooltipLabel->setText("Amount must be positive non-zero integer");
        is_valid = false;
    }
    else if(min_quality < 0 || min_quality > 100)
    {
        ui_->tooltipLabel->setText("Min quality must be in range of 0-100");
        is_valid = false;
    }
    else if(max_quality < 0 || max_quality > 100)
    {
        ui_->tooltipLabel->setText("Max quality must be in range of 0-100");
        is_valid = false;
    }
    else if(bp_chance < 0 || bp_chance > 100)
    {
        ui_->tooltipLabel->setText("Blueprint chance must be in range of 0-100");
        is_valid = false;
    }

    if(!is_valid)
    {
        return;
    }

    item_->SetBP(bp);
    item_->SetAmount(amount);
    item_->SetMinQuality(min_quality);
    item_->SetMaxQuality(max_quality);
    item_->SetAddStatFlag(add_stat);
    item_->SetBPChance(bp_chance);

    hide();

    if(is_creation_mode_)
    {
        emit SubmitCreationSignal();
    }
    else
    {
        emit SubmitEditionSignal();
    }
}

void ItemWidget::Cancel(bool hidden)
{
    if(!hidden)
    {
        hide();
    }

    if(is_creation_mode_)
    {
        emit CancelCreationSignal();
    }
    else
    {
        emit CancelEditionSignal();
    }
}

void ItemWidget::ResetUI()
{
    ui_->tooltipLabel->setText("");
    ui_->blueprintEdit->setText("");
    ui_->amountSpin->setValue(0);
    ui_->minQualitySpin->setValue(0);
    ui_->maxQualitySpin->setValue(0);
    ui_->addStatSpin->setValue(0);
    ui_->blueprintChanceSpin->setValue(0);
}

void ItemWidget::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    Cancel(true);
}
