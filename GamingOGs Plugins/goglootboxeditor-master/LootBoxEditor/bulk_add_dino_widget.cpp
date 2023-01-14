#include "bulk_add_dino_widget.h"
#include "ui_bulk_add_dino_widget.h"

#include "internal/box.h"
#include "internal/dino_block.h"
#include "internal/dino.h"
#include "internal/json_utils.h"

BulkAddDinoWidget::BulkAddDinoWidget(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui_(new Ui::BulkAddDinoWidget)
{
    ui_->setupUi(this);

    connect(ui_->submitButton, SIGNAL(clicked(bool)), this, SLOT(SubmitButtonClickedSlot(bool)));
    connect(ui_->cancelButton, SIGNAL(clicked(bool)), this, SLOT(CancelButtonClickedSlot(bool)));
}

BulkAddDinoWidget::~BulkAddDinoWidget()
{
    delete ui_;
}

void BulkAddDinoWidget::Open(LootBox::Box* box)
{
    box_ = box;
    ResetUI();
    show();
}

bool BulkAddDinoWidget::CheckSettings()
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

bool BulkAddDinoWidget::CheckTemplate()
{
    const int level = ui_->levelSpin->value();
    const int imprint_amount = ui_->imprintAmountSpin->value();
    const int is_baby = ui_->isBabyComboBox->currentIndex();
    const int is_neutered = ui_->isNeuteredComboBox->currentIndex();

    if(level <= 0)
    {
        ui_->templateTooltip->setText("Level must be a positive non-zero integer");
        return false;
    }
    else if(imprint_amount < 0 || imprint_amount > 100)
    {
        ui_->templateTooltip->setText("Imprint amount must be a positive integer in range of 1-100");
        return false;
    }
    else if(is_baby == 0)
    {
        ui_->templateTooltip->setText("Baby boolean value must be specified");
        return false;
    }
    else if(is_neutered == 0)
    {
        ui_->templateTooltip->setText("Neutered boolean value must be specified");
        return false;
    }

    return true;
}

void BulkAddDinoWidget::CreateElements()
{
    QStringList blueprints = ui_->blueprintsEdit->toPlainText().split('\n', Qt::SkipEmptyParts);
    for(const QString& blueprint : blueprints)
    {
        LootBox::DinoBlock* node = new LootBox::DinoBlock(box_);
        LootBox::Dino* dino = new LootBox::Dino(node);

        node->SetDescription(ui_->descriptionEdit->text());
        node->SetProbability(ui_->probabilitySpin->value());

        dino->SetBP(LootBox::Utils::UnescapeString(blueprint));
        dino->SetSaddleBP("");
        dino->SetLevel(ui_->levelSpin->value());
        dino->SetImprintAmount(ui_->imprintAmountSpin->value());
        dino->SetSaddleQuality(0);
        dino->SetBabyFlag(static_cast<LootBox::BooleanValue>(ui_->isBabyComboBox->currentIndex()));
        dino->SetNeuteredFlag(static_cast<LootBox::BooleanValue>(ui_->isNeuteredComboBox->currentIndex()));

        node->AddDino(dino);
        box_->AddNode(node);
    }
}

void BulkAddDinoWidget::ResetUI()
{
    ui_->descriptionEdit->setText("");
    ui_->probabilitySpin->setValue(50);

    ui_->levelSpin->setValue(0);
    ui_->imprintAmountSpin->setValue(0);
    ui_->isBabyComboBox->setCurrentIndex(2);
    ui_->isNeuteredComboBox->setCurrentIndex(2);
    ui_->blueprintsEdit->setText("");
}

void BulkAddDinoWidget::SubmitButtonClickedSlot(bool)
{
    if(!CheckSettings() || !CheckTemplate())
    {
        return;
    }

    CreateElements();

    emit SubmittedAddingSignal();
    hide();
}

void BulkAddDinoWidget::CancelButtonClickedSlot(bool)
{
    box_ = nullptr;
    hide();
}
