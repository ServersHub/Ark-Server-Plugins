#include "editshopbeaconwidget.h"
#include "ui_editshopbeaconwidget.h"

#include "internal/shopbeacon.h"

EditShopBeaconWidget::EditShopBeaconWidget(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui_(new Ui::EditShopBeaconWidget)
{
    ui_->setupUi(this);

    connect(ui_->submitBtn, &QPushButton::clicked, [this]() {
        this->Submit();
    });
    connect(ui_->cancelBtn, &QPushButton::clicked, [this]() {
        emit OnCancel();
        hide();
    });
}

EditShopBeaconWidget::~EditShopBeaconWidget()
{
    delete ui_;
}

void EditShopBeaconWidget::Open(Shop::ShopBeacon *node)
{
    item_ = node;

    ui_->nameEdit->setText(node->Name() ? *node->Name() : "");
    ui_->descEdit->setText(node->Description() ? *node->Description() : "");
    ui_->minLevel->setValue(node->MinLevel() ? *node->MinLevel() : -1);
    ui_->maxLevel->setValue(node->MaxLevel() ? *node->MaxLevel() : -1);
    ui_->price->setValue(node->Price() ? *node->Price() : -1);
    ui_->className->setText(node->ClassName() ? *node->ClassName() : "");
    ui_->error->setText("");

    show();
}

void EditShopBeaconWidget::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    emit OnCancel();
}

void EditShopBeaconWidget::Submit()
{
    const auto name = ui_->nameEdit->text();
    const auto desc = ui_->descEdit->text();
    const auto min_level = ui_->minLevel->value();
    const auto max_level = ui_->maxLevel->value();
    const auto price = ui_->price->value();
    const auto class_name = ui_->className->text();

    if(name.isEmpty())
    {
        ui_->error->setText("Name shouldn't be empty");
        return;
    }
    if(min_level < -1 || min_level == 0)
    {
        ui_->error->setText("Min level should be > 0 or -1");
        return;
    }
    if(max_level < -1 || max_level == 0)
    {
        ui_->error->setText("Max level should be > 0 or -1");
        return;
    }
    if(price < 0)
    {
        ui_->error->setText("Price should be a positive integer");
        return;
    }
    if(class_name.isEmpty())
    {
        ui_->error->setText("Class name shouldn't be empty");
        return;
    }

    item_->Name() = name;
    if(!desc.isEmpty()) item_->Description().setNull();
    else item_->Description() = desc;

    if(min_level > 0) item_->MinLevel() = min_level;
    else item_->MinLevel().setNull();

    if(max_level > 0) item_->MaxLevel() = max_level;
    else item_->MaxLevel().setNull();

    item_->Price() = price;
    item_->ClassName() = class_name;

    hide();
    emit OnSubmit();
}
