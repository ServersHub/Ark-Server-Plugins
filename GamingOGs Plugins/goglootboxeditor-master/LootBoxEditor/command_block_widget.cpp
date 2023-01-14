#include "command_block_widget.h"
#include "ui_command_block_widget.h"

#include "internal/command_block.h"

CommandBlockWidget::CommandBlockWidget(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui_(new Ui::CommandBlockWidget)
{
    ui_->setupUi(this);
    connect(ui_->submitButton, &QPushButton::clicked, [this]() { this->Submit(); });
    connect(ui_->cancelButton, &QPushButton::clicked, [this]() { this->Cancel(false); });
}

CommandBlockWidget::~CommandBlockWidget()
{
    delete ui_;
}

void CommandBlockWidget::Open(LootBox::CommandBlock* node)
{
    node_ = node;
    ResetUI();
    ui_->descriptionEdit->setText(node->GetDescription());
    ui_->probabilitySpin->setValue(node->GetProbability());
    ui_->commandEdit->setText(node->GetCommand());
    show();
}

void CommandBlockWidget::Submit()
{
    const QString description = ui_->descriptionEdit->text();
    const int probability = ui_->probabilitySpin->value();
    const QString command = ui_->commandEdit->text();

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
    else if(command.isEmpty() || command.count("{}") != 1)
    {
        ui_->tooltipLabel->setText("Command syntax is incorrect");
        is_valid = false;
    }

    if(!is_valid)
    {
        return;
    }

    node_->SetDescription(description);
    node_->SetProbability(probability);
    node_->SetCommand(command);

    hide();
    emit SubmitSignal();
}

void CommandBlockWidget::Cancel(bool hidden)
{
    if(!hidden)
    {
        hide();
    }
    emit CancelSignal();
}

void CommandBlockWidget::ResetUI()
{
    ui_->descriptionEdit->setText("");
    ui_->probabilitySpin->setValue(0);
    ui_->commandEdit->setText("");
}

void CommandBlockWidget::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    Cancel(true);
}
