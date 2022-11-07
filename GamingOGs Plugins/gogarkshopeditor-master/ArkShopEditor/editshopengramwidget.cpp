#include "editshopengramwidget.h"
#include "ui_editshopengramwidget.h"

#include "internal/shopengram.h"

EditShopEngramWidget::EditShopEngramWidget(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui_(new Ui::EditShopEngramWidget)
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

EditShopEngramWidget::~EditShopEngramWidget()
{
    delete ui_;
}

void EditShopEngramWidget::Open(Shop::ShopEngram *node)
{
    item_ = node;

    ui_->nameEdit->setText(node->Name() ? *node->Name() : "");
    ui_->descEdit->setText(node->Description() ? *node->Description() : "");
    ui_->minLevel->setValue(node->MinLevel() ? *node->MinLevel() : -1);
    ui_->maxLevel->setValue(node->MaxLevel() ? *node->MaxLevel() : -1);
    ui_->price->setValue(node->Price() ? *node->Price() : -1);
    auto& blueprints_array = *item_->Blueprints();
    QString bps = "";
    for(const auto& bp : blueprints_array)
    {
        bps.append(bp);
        bps.append('\n');
    }
    bps = bps.trimmed();
    ui_->blueprints->setPlainText(bps);
    ui_->error->setText("");

    show();
}

void EditShopEngramWidget::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    emit OnCancel();
}

void EditShopEngramWidget::Submit()
{
    const auto name = ui_->nameEdit->text();
    const auto desc = ui_->descEdit->text();
    const auto min_level = ui_->minLevel->value();
    const auto max_level = ui_->maxLevel->value();
    const auto price = ui_->price->value();
    const auto bps = ui_->blueprints->toPlainText().split('\n', Qt::SkipEmptyParts);

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
    if(bps.empty())
    {
        ui_->error->setText("Blueprints shouldn't be empty");
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
    std::vector<QString> bps_array{};
    for(auto& bp : bps)
    {
        bps_array.push_back(std::move(bp));
    }
    item_->Blueprints() = std::move(bps_array);

    hide();
    emit OnSubmit();
}
