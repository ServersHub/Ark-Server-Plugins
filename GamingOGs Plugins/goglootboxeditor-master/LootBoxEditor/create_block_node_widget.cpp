#include "create_block_node_widget.h"
#include "ui_create_block_node_widget.h"

#include "internal/box.h"
#include "internal/node.h"
#include "internal/item_block.h"
#include "internal/dino_block.h"
#include "internal/command_block.h"
#include "internal/beacon_block.h"
#include "internal/points_block.h"

CreateBlockNodeWidget::CreateBlockNodeWidget(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui_(new Ui::CreateBlockNodeWidget)
{
    ui_->setupUi(this);

    connect(ui_->submitButton, &QPushButton::clicked, [this]() { this->Submit(); });
    connect(ui_->cancelButton, &QPushButton::clicked, [this]() { this->Cancel(); });
}

CreateBlockNodeWidget::~CreateBlockNodeWidget()
{
    delete ui_;
}

void CreateBlockNodeWidget::closeEvent(QCloseEvent *event)
{
    Cancel();
    QWidget::closeEvent(event);
}

void CreateBlockNodeWidget::Open(LootBox::Box* parent_box, LootBox::BlockNode** node)
{
    parent_box_ = parent_box;
    new_node_ptr_ = node;
    ResetUI();
    show();
}

void CreateBlockNodeWidget::Submit()
{
    const int selectedType = ui_->typeComboBox->currentIndex();
    const QString description = ui_->descriptionEdit->text();
    const int probability = ui_->probabilitySpin->value();

    if(description.isEmpty())
    {
        ui_->tooltipLabel->setText("Description can't be empty");
        return;
    }
    if(probability <= 0 || probability > 100)
    {
        ui_->tooltipLabel->setText("Probability must be in range of 1 - 100");
        return;
    }

    switch(selectedType)
    {
    case 0:
        *new_node_ptr_ = LootBox::BlockNode::Create(parent_box_, LootBox::NodeType::ItemBlock);
        break;
    case 1:
        *new_node_ptr_ = LootBox::BlockNode::Create(parent_box_, LootBox::NodeType::DinoBlock);
        break;
    case 2:
        *new_node_ptr_ = LootBox::BlockNode::Create(parent_box_, LootBox::NodeType::CommandBlock);
        break;
    case 3:
        *new_node_ptr_ = LootBox::BlockNode::Create(parent_box_, LootBox::NodeType::BeaconBlock);
        break;
    case 4:
        *new_node_ptr_ = LootBox::BlockNode::Create(parent_box_, LootBox::NodeType::PointsBlock);
        break;
    default:
        break;
    }

    if(new_node_ptr_)
    {
        (*new_node_ptr_)->SetDescription(description);
        (*new_node_ptr_)->SetProbability(probability);
    }

    hide();
    emit SubmitSignal();
}

void CreateBlockNodeWidget::Cancel()
{
    hide();
    emit CancelSignal();
}

void CreateBlockNodeWidget::ResetUI()
{
    ui_->typeComboBox->setCurrentIndex(0);
    ui_->descriptionEdit->setText("");
    ui_->probabilitySpin->setValue(50);
}
