#include "addkitwidget.h"
#include "ui_addkitwidget.h"

#include "internal/kit.h"

AddKitWidget::AddKitWidget(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui_(new Ui::AddKitWidget)
{
    ui_->setupUi(this);

    connect(ui_->submitBtn, &QPushButton::clicked, [this]() { this->Submit(); });
    connect(ui_->cancelBtn, &QPushButton::clicked, [this]() {
        this->hide();
        emit CancelCreationSignal();
    });
}

AddKitWidget::~AddKitWidget()
{
    delete ui_;
}

void AddKitWidget::Open(Shop::Kit* kit_ptr)
{
    if(!kit_ptr)
    {
        return;
    }

    current_kit_ = kit_ptr;

    ui_->errorLabel->setText("");
    ui_->nameEdit->setText("");
    ui_->descEdit->setText("");
    ui_->permissionsEdit->setPlainText("");
    ui_->defAmountSpin->setValue(-1);
    ui_->minLevelSpin->setValue(-1);
    ui_->maxLevelSpin->setValue(-1);
    ui_->priceSpin->setValue(-1);
    ui_->spawnCombo->setCurrentIndex(0);

    show();
}

void AddKitWidget::Submit()
{
    const auto name = ui_->nameEdit->text();
    const auto desc = ui_->descEdit->text();
    const auto permissions = ui_->permissionsEdit->toPlainText();
    const auto def_amount = ui_->defAmountSpin->value();
    const auto min_level = ui_->minLevelSpin->value();
    const auto max_level = ui_->maxLevelSpin->value();
    const auto price = ui_->priceSpin->value();
    const auto spawnFlagIndex = ui_->spawnCombo->currentIndex();

    if(name.isEmpty())
    {
        ui_->errorLabel->setText("Name can't be empty");
        return;
    }

    current_kit_->Name() = name;
    if(!desc.isEmpty()) current_kit_->Description() = desc;
    if(!permissions.isEmpty()) current_kit_->Permissions() = permissions;
    if(def_amount != -1) current_kit_->DefaultAmount() = def_amount;
    if(min_level != -1) current_kit_->MinLevel() = min_level;
    if(max_level != -1) current_kit_->MaxLevel() = max_level;
    if(price != -1) current_kit_->Price() = price;
    if(spawnFlagIndex != 0) current_kit_->IsOnlyFromSpawn() = spawnFlagIndex == 1 ? true : false;

    hide();
    emit SubmitCreationSignal();
}

void AddKitWidget::closeEvent(QCloseEvent* event)
{
    QWidget::closeEvent(event);
    emit CancelCreationSignal();
}
