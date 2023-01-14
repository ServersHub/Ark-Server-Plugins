#include "bulk_add_item_widget.h"
#include "ui_bulk_add_item_widget.h"

#include <QString>
#include <QStringList>

#include "internal/box.h"
#include "internal/item_block.h"
#include "internal/item.h"
#include "internal/json_utils.h"

BulkAddItemWidget::BulkAddItemWidget(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui_(new Ui::BulkAddItemWidget)
{
    ui_->setupUi(this);

    connect(ui_->submitButton, SIGNAL(clicked(bool)), this, SLOT(SubmitButtonClickedSlot(bool)));
    connect(ui_->cancelButton, SIGNAL(clicked(bool)), this, SLOT(CancelButtonClickedSlot(bool)));
}

BulkAddItemWidget::~BulkAddItemWidget()
{
    delete ui_;
}

void BulkAddItemWidget::Open(LootBox::Box* box)
{
    box_ = box;
    ResetUI();
    show();
}

bool BulkAddItemWidget::CheckSettings()
{
    const QString description = ui_->descriptionEdit->text();
    const int probability = ui_->probabilitySpin->value();

    if(description.isEmpty())
    {
        ui_->settingsTooltip->setText("Description can't be empty");
        return false;
    }
    if(probability <= 0 || probability > 100)
    {
        ui_->settingsTooltip->setText("Probability must be in range of 1 - 100");
        return false;
    }

    return true;
}

bool BulkAddItemWidget::CheckTemplate()
{
    const int amount = ui_->amountSpin->value();
    const int min_quality = ui_->minQualitySpin->value();
    const int max_quality = ui_->maxQualitySpin->value();
    const int bp_chance = ui_->blueprintChanceSpin->value();

    if(amount <= 0)
    {
        ui_->templateTooltip->setText("Amount must be positive non-zero integer");
        return false;
    }
    else if(min_quality < 0 || min_quality > 100)
    {
        ui_->templateTooltip->setText("Min quality must be in range of 0-100");
        return false;
    }
    else if(max_quality < 0 || max_quality > 100)
    {
        ui_->templateTooltip->setText("Max quality must be in range of 0-100");
        return false;
    }
    else if(bp_chance < 0 || bp_chance > 100)
    {
        ui_->templateTooltip->setText("Blueprint chance must be in range of 0-100");
        return false;
    }

    return true;
}

void BulkAddItemWidget::CreateElements()
{
    QStringList blueprints = ui_->blueprintsEdit->toPlainText().split('\n', Qt::SkipEmptyParts);
    for(const QString& blueprint : blueprints)
    {
        LootBox::ItemBlock* node = new LootBox::ItemBlock(box_);
        LootBox::Item* item = new LootBox::Item(node);

        node->SetDescription(ui_->descriptionEdit->text());
        node->SetProbability(ui_->probabilitySpin->value());

        item->SetBP(LootBox::Utils::UnescapeString(blueprint));
        item->SetAmount(ui_->amountSpin->value());
        item->SetMinQuality(ui_->minQualitySpin->value());
        item->SetMaxQuality(ui_->maxQualitySpin->value());
        item->SetAddStatFlag(ui_->addStatSpin->value());
        item->SetBPChance(ui_->blueprintChanceSpin->value());

        node->AddItem(item);
        box_->AddNode(node);
    }
}

void BulkAddItemWidget::ResetUI()
{
    ui_->descriptionEdit->setText("");
    ui_->probabilitySpin->setValue(50);

    ui_->amountSpin->setValue(1);
    ui_->minQualitySpin->setValue(0);
    ui_->maxQualitySpin->setValue(0);
    ui_->addStatSpin->setValue(0);
    ui_->blueprintChanceSpin->setValue(0);
    ui_->blueprintsEdit->setText("");
}

void BulkAddItemWidget::SubmitButtonClickedSlot(bool)
{
    if(!CheckSettings() || !CheckTemplate())
    {
        return;
    }

    CreateElements();

    emit SubmittedAddingSignal();
    hide();
}

void BulkAddItemWidget::CancelButtonClickedSlot(bool)
{
    box_ = nullptr;
    hide();
}
