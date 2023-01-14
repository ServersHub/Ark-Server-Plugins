#include "editshopitemwidget.h"
#include "ui_editshopitemwidget.h"

#include "internal/shopitem.h"
#include "itemwidget.h"

EditShopItemWidget::EditShopItemWidget(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui_(new Ui::EditShopItemWidget)
{
    ui_->setupUi(this);

    widget_ = new ItemWidget(this);

    QTableWidget* table = ui_->table;
    QHeaderView* hHeader = table->horizontalHeader();
    QHeaderView* vHeader = table->verticalHeader();

    table->setVisible(false);
    table->setContentsMargins(2, 2, 2, 2);
    table->setColumnCount(7);

    hHeader->setFixedHeight(20);
    hHeader->setSectionResizeMode(0, QHeaderView::Fixed);
    hHeader->setSectionResizeMode(5, QHeaderView::Fixed);
    hHeader->setSectionResizeMode(6, QHeaderView::Fixed);
    hHeader->setSectionResizeMode(1, QHeaderView::Stretch);
    hHeader->setSectionResizeMode(2, QHeaderView::Stretch);
    hHeader->setSectionResizeMode(3, QHeaderView::Stretch);
    hHeader->setSectionResizeMode(4, QHeaderView::Stretch);
    table->setMinimumWidth(20);
    table->setColumnWidth(0, 20);
    table->setColumnWidth(5, 20);
    table->setColumnWidth(6, 20);
    hHeader->setStretchLastSection(false);

    table->setHorizontalHeaderItem(0, new QTableWidgetItem(""));
    table->setHorizontalHeaderItem(1, new QTableWidgetItem("Blueprint"));
    table->setHorizontalHeaderItem(2, new QTableWidgetItem("Quality"));
    table->setHorizontalHeaderItem(3, new QTableWidgetItem("Amount"));
    table->setHorizontalHeaderItem(4, new QTableWidgetItem("Force blueprint"));
    table->setHorizontalHeaderItem(5, new QTableWidgetItem(""));
    table->setHorizontalHeaderItem(6, new QTableWidgetItem(""));

    vHeader->setSectionResizeMode(QHeaderView::Fixed);
    vHeader->setDefaultSectionSize(20);
    vHeader->setVisible(false);

    table->setFocusPolicy(Qt::NoFocus);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    table->setVisible(true);

    connect(table, SIGNAL(cellClicked(int, int)), this, SLOT(CellClickedSlot(int, int)));
    connect(table, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(CellDoubleClickedSlot(int, int)));
    connect(widget_, SIGNAL(SubmitCreationSignal()), this, SLOT(OnSubmitItemCreationSlot()));
    connect(widget_, SIGNAL(CancelCreationSignal()), this, SLOT(OnCancelItemCreationSlot()));
    connect(widget_, SIGNAL(SubmitEditingSignal()), this, SLOT(OnSubmitItemEditingSlot()));
    connect(widget_, SIGNAL(CancelEditingSignal()), this, SLOT(OnCancelItemEditingSlot()));

    connect(ui_->submitBtn, &QPushButton::clicked, [this]() {
        this->Submit();
    });
    connect(ui_->cancelBtn, &QPushButton::clicked, [this]() {
        emit OnCancel();
        hide();
    });
}

EditShopItemWidget::~EditShopItemWidget()
{
    delete ui_;
}

void EditShopItemWidget::Open(Shop::ShopItem *node)
{
    item_ = node;

    ui_->nameEdit->setText(node->Name() ? *node->Name() : "");
    ui_->descEdit->setText(node->Description() ? *node->Description() : "");
    ui_->minLevel->setValue(node->MinLevel() ? *node->MinLevel() : -1);
    ui_->maxLevel->setValue(node->MaxLevel() ? *node->MaxLevel() : -1);
    ui_->price->setValue(node->Price() ? *node->Price() : -1);
    ui_->error->setText("");
    DrawTable();

    show();
}

void EditShopItemWidget::DrawTable()
{
    ui_->table->setVisible(false);
    ui_->table->clearContents();

    auto& items = *item_->Items();

    ui_->table->setRowCount(static_cast<int>(items.size()) + 1);
    ui_->table->setColumnCount(7);

    int row = 0;
    for(auto item : items)
    {
        DrawItemsRow(item, row);
        row++;
    }

    QTableWidgetItem* add_item = new QTableWidgetItem();
    add_item->setIcon(QIcon(":/icons/icons/add.png"));
    ui_->table->setItem(row, 6, add_item);

    ui_->table->setVisible(true);
}

void EditShopItemWidget::DrawItemsRow(Shop::Item *item, int row_num)
{
    const bool is_valid = item->IsValid();
    QTableWidget* table = ui_->table;

    QTableWidgetItem* status_item = new QTableWidgetItem();
    status_item->setIcon(is_valid ? QIcon(":/icons/icons/valid.png") : QIcon(":/icons/icons/invalid.png"));
    status_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    QTableWidgetItem* edit_item = new QTableWidgetItem();
    edit_item->setIcon(QIcon(":/icons/icons/edit.png"));
    edit_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    QTableWidgetItem* delete_item = new QTableWidgetItem();
    delete_item->setIcon(QIcon(":/icons/icons/delete.png"));
    delete_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

    table->setItem(row_num, 0, status_item);
    table->setItem(row_num, 1, (item->Blueprint()) ? new QTableWidgetItem(*item->Blueprint()) : new QTableWidgetItem("-"));
    table->setItem(row_num, 2, (item->Quality()) ? new QTableWidgetItem(QString::number(*item->Quality())) : new QTableWidgetItem("-"));
    table->setItem(row_num, 3, (item->Amount()) ? new QTableWidgetItem(QString::number(*item->Amount())) : new QTableWidgetItem("-"));
    table->setItem(row_num, 4, item->ForceBlueprint() ? (*item->ForceBlueprint() ? new QTableWidgetItem("True") : new QTableWidgetItem("False")) : new QTableWidgetItem("-"));
    table->setItem(row_num, 5, edit_item);
    table->setItem(row_num, 6, delete_item);

    QTableWidgetItem* data_item = table->item(row_num, 0);
    data_item->setData(Qt::UserRole, QVariant::fromValue(item));
}

void EditShopItemWidget::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    emit OnCancel();
}

void EditShopItemWidget::Submit()
{
    const auto name = ui_->nameEdit->text();
    const auto desc = ui_->descEdit->text();
    const auto min_level = ui_->minLevel->value();
    const auto max_level = ui_->maxLevel->value();
    const auto price = ui_->price->value();

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

    item_->Name() = name;
    if(desc.isEmpty()) item_->Description().setNull();
    else item_->Description() = desc;

    if(min_level > 0) item_->MinLevel() = min_level;
    else item_->MinLevel().setNull();

    if(max_level > 0) item_->MaxLevel() = max_level;
    else item_->MaxLevel().setNull();

    item_->Price() = price;

    hide();
    emit OnSubmit();
}

void EditShopItemWidget::CellClickedSlot(int row, int col)
{
    if(col != 5 && col != 6)
    {
        return;
    }

    if(col == 5 && row <= ui_->table->rowCount() - 2)
    {
        QTableWidgetItem* data_item = ui_->table->item(row, 0);
        current_item_ = data_item->data(Qt::UserRole).value<Shop::Item*>();
        widget_->Open(current_item_, false);
    }
    else if(col == 6)
    {
        int rows_count = ui_->table->rowCount();
        if(row <= rows_count - 2)
        {
            QTableWidgetItem* data_item = ui_->table->item(row, 0);
            Shop::Item* item = data_item->data(Qt::UserRole).value<Shop::Item*>();
            auto it = std::find_if((*item_->Items()).begin(), (*item_->Items()).end(), [item](const Shop::Item* ptr) { return item == ptr; });
            if(it != (*item_->Items()).end())
            {
                (*item_->Items()).erase(it);
            }
            delete item;
            DrawTable();
        }
        else if(row == rows_count - 1)
        {
            current_item_ = new Shop::Item();
            widget_->Open(current_item_, true);
        }
    }
}

void EditShopItemWidget::CellDoubleClickedSlot(int row, int col)
{
    if(row <= ui_->table->rowCount() - 2 && (col > 0 && col < 5))
    {
        QTableWidgetItem* data_item = ui_->table->item(row, 0);
        current_item_ = data_item->data(Qt::UserRole).value<Shop::Item*>();
        widget_->Open(current_item_, false);
    }
}

void EditShopItemWidget::OnSubmitItemCreationSlot()
{
    (*item_->Items()).push_back(current_item_);
    current_item_ = nullptr;
    DrawTable();
}

void EditShopItemWidget::OnCancelItemCreationSlot()
{
    delete current_item_;
    current_item_ = nullptr;
}

void EditShopItemWidget::OnSubmitItemEditingSlot()
{
    current_item_ = nullptr;
    DrawTable();
}

void EditShopItemWidget::OnCancelItemEditingSlot()
{
    current_item_ = nullptr;
}
