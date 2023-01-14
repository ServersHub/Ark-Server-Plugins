#include "editshopcommandwidget.h"
#include "ui_editshopcommandwidget.h"

#include "internal/shopcommand.h"

EditShopCommandWidget::EditShopCommandWidget(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui_(new Ui::EditShopCommandWidget)
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

EditShopCommandWidget::~EditShopCommandWidget()
{
    delete ui_;
}

void EditShopCommandWidget::Open(Shop::ShopCommand *node)
{
    item_ = node;

    ui_->nameEdit->setText(node->Name() ? *node->Name() : "");
    ui_->descEdit->setText(node->Description() ? *node->Description() : "");
    ui_->minLevel->setValue(node->MinLevel() ? *node->MinLevel() : -1);
    ui_->maxLevel->setValue(node->MaxLevel() ? *node->MaxLevel() : -1);
    ui_->price->setValue(node->Price() ? *node->Price() : -1);
    auto& commands_array = *item_->Commands();
    QString commands = "";
    for(const auto& command : commands_array)
    {
        commands.append(command);
        commands.append('\n');
    }
    commands = commands.trimmed();
    ui_->command->setPlainText(commands);
    ui_->error->setText("");

    show();
}

void EditShopCommandWidget::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    emit OnCancel();
}

void EditShopCommandWidget::Submit()
{
    const auto name = ui_->nameEdit->text();
    const auto desc = ui_->descEdit->text();
    const auto min_level = ui_->minLevel->value();
    const auto max_level = ui_->maxLevel->value();
    const auto price = ui_->price->value();
    const auto commands = ui_->command->toPlainText().split('\n', Qt::SkipEmptyParts);

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
    if(commands.empty())
    {
        ui_->error->setText("Commands shouldn't be empty");
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
    std::vector<QString> commands_array{};
    for(auto& command : commands)
    {
        commands_array.push_back(std::move(command));
    }
    item_->Commands() = std::move(commands_array);

    hide();
    emit OnSubmit();
}
