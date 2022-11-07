#include "beacon_block_widget.h"
#include "ui_beacon_block_widget.h"

#include "internal/beacon_block.h"

BeaconBlockWidget::BeaconBlockWidget(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui_(new Ui::BeaconBlockWidget)
{
    ui_->setupUi(this);
    connect(ui_->submitButton, &QPushButton::clicked, [this]() { this->Submit(); });
    connect(ui_->cancelButton, &QPushButton::clicked, [this]() { this->Cancel(false); });
}

BeaconBlockWidget::~BeaconBlockWidget()
{
    delete ui_;
}

void BeaconBlockWidget::Open(LootBox::BeaconBlock* node)
{
    node_ = node;
    ResetUI();
    ui_->descriptionEdit->setText(node->GetDescription());
    ui_->probabilitySpin->setValue(node->GetProbability());
    ui_->classNameEdit->setText(node->GetClassName());
    show();
}

void BeaconBlockWidget::Submit()
{
    const QString description = ui_->descriptionEdit->text();
    const int probability = ui_->probabilitySpin->value();
    const QString class_name = ui_->classNameEdit->text();

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
    else if(class_name.isEmpty())
    {
        ui_->tooltipLabel->setText("Class name can't be empty");
        is_valid = false;
    }

    if(!is_valid)
    {
        return;
    }

    node_->SetDescription(description);
    node_->SetProbability(probability);
    node_->SetClassName(class_name);

    hide();
    emit SubmitSignal();
}

void BeaconBlockWidget::Cancel(bool hidden)
{
    if(!hidden)
    {
        hide();
    }
    emit CancelSignal();
}

void BeaconBlockWidget::ResetUI()
{
    ui_->descriptionEdit->setText("");
    ui_->probabilitySpin->setValue(0);
    ui_->classNameEdit->setText("");
}

void BeaconBlockWidget::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    Cancel(true);
}
