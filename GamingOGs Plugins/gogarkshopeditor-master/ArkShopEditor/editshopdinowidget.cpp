#include "editshopdinowidget.h"
#include "ui_editshopdinowidget.h"

#include "internal/shopdino.h"

EditShopDinoWidget::EditShopDinoWidget(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui_(new Ui::EditShopDinoWidget)
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

EditShopDinoWidget::~EditShopDinoWidget()
{
    delete ui_;
}

void EditShopDinoWidget::Open(Shop::ShopDino *node)
{
    item_ = node;

    ui_->nameEdit->setText(node->Name() ? *node->Name() : "");
    ui_->descEdit->setText(node->Description() ? *node->Description() : "");
    ui_->minLevel->setValue(node->MinLevel() ? *node->MinLevel() : -1);
    ui_->maxLevel->setValue(node->MaxLevel() ? *node->MaxLevel() : -1);
    ui_->price->setValue(node->Price() ? *node->Price() : -1);
    ui_->bp->setText(node->Blueprint() ? *node->Blueprint() : "");
    ui_->level->setValue(node->Level() ? *node->Level() : -1);
    ui_->neutered->setCurrentIndex(node->Neutered() ? *node->Neutered() ? 1 : 2 : 0);
    ui_->error->setText("");

    show();
}

void EditShopDinoWidget::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    emit OnCancel();
}

void EditShopDinoWidget::Submit()
{
    const auto name = ui_->nameEdit->text();
    const auto desc = ui_->descEdit->text();
    const auto min_level = ui_->minLevel->value();
    const auto max_level = ui_->maxLevel->value();
    const auto price = ui_->price->value();
    const auto bp = ui_->bp->text();
    const auto level = ui_->level->value();
    const auto neutered_index = ui_->neutered->currentIndex();

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
    if(bp.isEmpty())
    {
        ui_->error->setText("Blueprint shouldn't be empty");
        return;
    }
    if(level < 1)
    {
        ui_->error->setText("Level should be a positive integer");
        return;
    }

    item_->Name() = name;
    if(desc.isEmpty()) item_->Description().setNull();
    else item_->Description() = desc;

    if(min_level > 0) item_->MinLevel() = min_level;
    else item_->MinLevel().setNull();

    if(max_level > 0) item_->MaxLevel() = max_level;
    else item_->MaxLevel().setNull();

    item_->Price() = price;
    item_->Blueprint() = bp;
    item_->Level() = level;

    if(neutered_index == 0) item_->Neutered().setNull();
    else item_->Neutered() = neutered_index == 1 ? true : false;

    hide();
    emit OnSubmit();
}
