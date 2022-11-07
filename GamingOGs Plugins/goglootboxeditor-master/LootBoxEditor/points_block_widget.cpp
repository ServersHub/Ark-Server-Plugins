#include "points_block_widget.h"
#include "ui_points_block_widget.h"

#include "internal/points_block.h"

PointsBlockWidget::PointsBlockWidget(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui_(new Ui::PointsBlockWidget)
{
    ui_->setupUi(this);
    connect(ui_->submitButton, &QPushButton::clicked, [this]() { this->Submit(); });
    connect(ui_->cancelButton, &QPushButton::clicked, [this]() { this->Cancel(false); });
}

PointsBlockWidget::~PointsBlockWidget()
{
    delete ui_;
}

void PointsBlockWidget::Open(LootBox::PointsBlock* node)
{
    node_ = node;
    ResetUI();
    ui_->descriptionEdit->setText(node->GetDescription());
    ui_->probabilitySpin->setValue(node->GetProbability());
    ui_->minPointsSpin->setValue(node->GetMinAmount());
    ui_->maxPointsSpin->setValue(node->GetMaxAmount());
    show();
}

void PointsBlockWidget::Submit()
{
    const QString description = ui_->descriptionEdit->text();
    const int probability = ui_->probabilitySpin->value();
    const int min_points = ui_->minPointsSpin->value();
    const int max_points = ui_->maxPointsSpin->value();

    bool is_valid = true;

    if(description.isEmpty())
    {
        ui_->tooltipLabel->setText("Description can't be empty");
        is_valid = false;
    }
    else if(probability <= 0 || probability > 100)
    {
        ui_->tooltipLabel->setText("Probability must be in range of 0 - 100");
        is_valid = false;
    }
    else if(min_points <= 0)
    {
        ui_->tooltipLabel->setText("Min points must be a positive non-zero integer");
        is_valid = false;
    }
    else if(max_points <= 0 || max_points < min_points)
    {
        ui_->tooltipLabel->setText("Max points must be a positive non-zero integer and greater or equal to min points");
        is_valid = false;
    }

    if(!is_valid)
    {
        return;
    }

    node_->SetDescription(description);
    node_->SetProbability(probability);
    node_->SetMinAmount(min_points);
    node_->SetMaxAmount(max_points);

    hide();
    emit SubmitSignal();
}

void PointsBlockWidget::Cancel(bool hidden)
{
    if(!hidden)
    {
        hide();
    }
    emit CancelSignal();
}

void PointsBlockWidget::ResetUI()
{
    ui_->descriptionEdit->setText("");
    ui_->probabilitySpin->setValue(0);
    ui_->minPointsSpin->setValue(0);
    ui_->maxPointsSpin->setValue(0);
}

void PointsBlockWidget::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    Cancel(true);
}
