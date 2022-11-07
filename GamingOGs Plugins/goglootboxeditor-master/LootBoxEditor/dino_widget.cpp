#include "dino_widget.h"
#include "ui_dino_widget.h"

#include "internal/dino.h"
#include "internal/json_utils.h"

DinoWidget::DinoWidget(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui_(new Ui::DinoWidget),
    dino_(nullptr),
    is_creation_mode_(false)
{
    ui_->setupUi(this);
    connect(ui_->submitButton, &QPushButton::clicked, [this]() { this->Submit(); });
    connect(ui_->cancelButton, &QPushButton::clicked, [this]() { this->Cancel(false); });
}

DinoWidget::~DinoWidget()
{
    delete ui_;
}

void DinoWidget::Open(LootBox::Dino* dino, bool is_creation)
{
    dino_ = dino;
    is_creation_mode_ = is_creation;

    if(is_creation)
    {
        setWindowTitle("Add dino");
        ResetUI();
    }
    else
    {
        setWindowTitle("Edit dino");
        ResetUI();
        ui_->blueprintEdit->setText(dino->GetBP());
        ui_->saddleBlueprintEdit->setText(dino->GetSaddleBP());
        ui_->levelSpin->setValue(dino->GetLevel());
        ui_->imprintAmountSpin->setValue(dino->GetImprintAmount());
        ui_->saddleQualitySpin->setValue(dino->GetSaddleQuality());
        ui_->isBabyComboBox->setCurrentIndex(static_cast<int>(dino->GetBabyFlag()));
        ui_->isNeuteredComboBox->setCurrentIndex(static_cast<int>(dino->GetNeuteredFlag()));
    }

    show();
}

void DinoWidget::Submit()
{
    const QString bp = LootBox::Utils::UnescapeString(ui_->blueprintEdit->toPlainText());
    const QString saddle_bp = LootBox::Utils::UnescapeString(ui_->saddleBlueprintEdit->toPlainText());
    const int level = ui_->levelSpin->value();
    const int imprint_amount = ui_->imprintAmountSpin->value();
    const int saddle_quality = ui_->saddleQualitySpin->value();
    const int is_baby = ui_->isBabyComboBox->currentIndex();
    const int is_neutered = ui_->isNeuteredComboBox->currentIndex();

    bool is_valid = true;

    if(bp.isEmpty())
    {
        ui_->tooltipLabel->setText("Blueprint can't be empty");
        is_valid = false;
    }
    else if(!saddle_bp.isEmpty() && saddle_quality <= 0)
    {
        ui_->tooltipLabel->setText("Saddle quality must be a positive non-zero integer");
        is_valid = false;
    }
    else if(level <= 0)
    {
        ui_->tooltipLabel->setText("Level must be a positive non-zero integer");
        is_valid = false;
    }
    else if(imprint_amount < 0 || imprint_amount > 100)
    {
        ui_->tooltipLabel->setText("Imprint amount must be a positive integer in range of 1-100");
        is_valid = false;
    }
    else if(is_baby == 0)
    {
        ui_->tooltipLabel->setText("Baby boolean value must be specified");
        is_valid = false;
    }
    else if(is_neutered == 0)
    {
        ui_->tooltipLabel->setText("Neutered boolean value must be specified");
        is_valid = false;
    }

    if(!is_valid)
    {
        return;
    }

    dino_->SetBP(bp);
    dino_->SetSaddleBP(saddle_bp);
    dino_->SetLevel(level);
    dino_->SetImprintAmount(imprint_amount);
    dino_->SetSaddleQuality(saddle_quality);
    dino_->SetBabyFlag(static_cast<LootBox::BooleanValue>(is_baby));
    dino_->SetNeuteredFlag(static_cast<LootBox::BooleanValue>(is_neutered));

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

void DinoWidget::Cancel(bool hidden)
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

void DinoWidget::ResetUI()
{
    ui_->blueprintEdit->setText("");
    ui_->saddleBlueprintEdit->setText("");
    ui_->levelSpin->setValue(0);
    ui_->imprintAmountSpin->setValue(0);
    ui_->saddleQualitySpin->setValue(0);
    ui_->isBabyComboBox->setCurrentIndex(0);
    ui_->isNeuteredComboBox->setCurrentIndex(0);
}

void DinoWidget::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    Cancel(true);
}
