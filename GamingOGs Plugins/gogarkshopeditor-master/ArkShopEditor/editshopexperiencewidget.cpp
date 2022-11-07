#include "editshopexperiencewidget.h"
#include "ui_editshopexperiencewidget.h"

#include "internal/shopexperience.h"

EditShopExperienceWidget::EditShopExperienceWidget(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui_(new Ui::EditShopExperienceWidget)
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

EditShopExperienceWidget::~EditShopExperienceWidget()
{
    delete ui_;
}

void EditShopExperienceWidget::Open(Shop::ShopExperience *node)
{
    item_ = node;

    ui_->nameEdit->setText(node->Name() ? *node->Name() : "");
    ui_->descEdit->setText(node->Description() ? *node->Description() : "");
    ui_->minLevel->setValue(node->MinLevel() ? *node->MinLevel() : -1);
    ui_->maxLevel->setValue(node->MaxLevel() ? *node->MaxLevel() : -1);
    ui_->price->setValue(node->Price() ? *node->Price() : -1);
    ui_->amount->setValue(node->Amount() ? *node->Amount() : -1);
    ui_->giveTo->setCurrentIndex(node->GiveToDino() ? *node->GiveToDino() ? 1 : 2 : 0);
    ui_->error->setText("");

    show();
}

void EditShopExperienceWidget::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    emit OnCancel();
}

void EditShopExperienceWidget::Submit()
{
    const auto name = ui_->nameEdit->text();
    const auto desc = ui_->descEdit->text();
    const auto min_level = ui_->minLevel->value();
    const auto max_level = ui_->maxLevel->value();
    const auto price = ui_->price->value();
    const auto amount = ui_->amount->value();
    const auto give_to_index = ui_->giveTo->currentIndex();

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
    if(amount == -1 || amount < 1)
    {
        ui_->error->setText("Amount should be a positive integer");
        return;
    }
    if(give_to_index == 0)
    {
        ui_->error->setText("Give to dino flag should be set");
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
    item_->Amount() = amount;
    item_->GiveToDino() = give_to_index == 1 ? true : false;

    hide();
    emit OnSubmit();
}
