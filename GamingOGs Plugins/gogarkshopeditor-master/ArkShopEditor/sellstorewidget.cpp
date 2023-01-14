#include "sellstorewidget.h"
#include "ui_sellstorewidget.h"

#include "sellstoreitemwidget.h"
#include "internal/sellstore.h"
#include "internal/sellstoreitem.h"

#include <QMessageBox>
#include <QTableWidget>

SellStoreWidget::SellStoreWidget(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui_(new Ui::SellStoreWidget)
{
    ui_->setupUi(this);

    item_widget_ = new SellStoreItemWidget(this);

    QTableWidget* table = ui_->table;
    QHeaderView* hHeader = table->horizontalHeader();
    QHeaderView* vHeader = table->verticalHeader();

    table->setVisible(false);
    table->setContentsMargins(2, 2, 2, 2);
    table->setColumnCount(9);

    hHeader->setFixedHeight(20);
    hHeader->setSectionResizeMode(0, QHeaderView::Fixed);
    hHeader->setSectionResizeMode(7, QHeaderView::Fixed);
    hHeader->setSectionResizeMode(8, QHeaderView::Fixed);
    hHeader->setSectionResizeMode(1, QHeaderView::Stretch);
    hHeader->setSectionResizeMode(2, QHeaderView::Stretch);
    hHeader->setSectionResizeMode(3, QHeaderView::Stretch);
    hHeader->setSectionResizeMode(4, QHeaderView::Stretch);
    hHeader->setSectionResizeMode(5, QHeaderView::Stretch);
    hHeader->setSectionResizeMode(6, QHeaderView::Stretch);
    hHeader->setSectionResizeMode(7, QHeaderView::Stretch);
    hHeader->setSectionResizeMode(8, QHeaderView::Stretch);
    table->setMinimumWidth(20);
    table->setColumnWidth(0, 20);
    table->setColumnWidth(7, 20);
    table->setColumnWidth(8, 20);
    hHeader->setStretchLastSection(false);

    table->setHorizontalHeaderItem(0, new QTableWidgetItem(""));
    table->setHorizontalHeaderItem(1, new QTableWidgetItem("Name"));
    table->setHorizontalHeaderItem(2, new QTableWidgetItem("Type"));
    table->setHorizontalHeaderItem(3, new QTableWidgetItem("Description"));
    table->setHorizontalHeaderItem(4, new QTableWidgetItem("Blueprint"));
    table->setHorizontalHeaderItem(5, new QTableWidgetItem("Price"));
    table->setHorizontalHeaderItem(6, new QTableWidgetItem("Amount"));
    table->setHorizontalHeaderItem(7, new QTableWidgetItem(""));
    table->setHorizontalHeaderItem(8, new QTableWidgetItem(""));

    vHeader->setSectionResizeMode(QHeaderView::Fixed);
    vHeader->setDefaultSectionSize(20);
    vHeader->setVisible(false);

    table->setFocusPolicy(Qt::NoFocus);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    table->setVisible(true);

    connect(table, SIGNAL(cellClicked(int, int)), this, SLOT(CellClickedSlot(int, int)));
    connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(CellDoubleClickedSlot(int,int)));
    connect(item_widget_, SIGNAL(SubmitCreationSignal()), this, SLOT(OnSubmitCreationSlot()));
    connect(item_widget_, SIGNAL(CancelCreationSignal()), this, SLOT(OnCancelCreationSlot()));
    connect(item_widget_, SIGNAL(SubmitEditingSignal()), this, SLOT(OnSubmitEditingSlot()));
    connect(item_widget_, SIGNAL(CancelEditingSignal()), this, SLOT(OnCancelEditingSlot()));
}

SellStoreWidget::~SellStoreWidget()
{
    delete ui_;
}

void SellStoreWidget::SetSellStore(Shop::SellStore* store)
{
    store_ = store;
}

void SellStoreWidget::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    emit OnCloseSignal();
}

void SellStoreWidget::Open()
{
    DrawTable();
    show();
}

void SellStoreWidget::DrawTable()
{
    ui_->table->setVisible(false);
    ui_->table->clearContents();

    auto& items = store_->GetItems();

    ui_->table->setRowCount(static_cast<int>(items.size()) + 1);
    ui_->table->setColumnCount(11);

    int row = 0;
    for(auto item : items)
    {
        DrawTableRow(item, row);
        row++;
    }

    QTableWidgetItem* add_item = new QTableWidgetItem();
    add_item->setIcon(QIcon(":/icons/icons/add.png"));
    ui_->table->setItem(row, 8, add_item);

    ui_->table->setVisible(true);
}

void SellStoreWidget::DrawTableRow(Shop::SellStoreItem *item, int row_num)
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
    table->setItem(row_num, 1, (item->Name()) ? new QTableWidgetItem(*item->Name()) : new QTableWidgetItem("-"));
    table->setItem(row_num, 2, (item->Type()) ? new QTableWidgetItem(*item->Type()) : new QTableWidgetItem("-"));
    table->setItem(row_num, 3, (item->Description()) ? new QTableWidgetItem(*item->Description()) : new QTableWidgetItem("-"));
    table->setItem(row_num, 4, (item->Blueprint()) ? new QTableWidgetItem(*item->Blueprint()) : new QTableWidgetItem("-"));
    table->setItem(row_num, 5, (item->Price()) ? new QTableWidgetItem(QString::number(*item->Price())) : new QTableWidgetItem("-"));
    table->setItem(row_num, 6, (item->Amount()) ? new QTableWidgetItem(QString::number(*item->Amount())) : new QTableWidgetItem("-"));
    table->setItem(row_num, 7, edit_item);
    table->setItem(row_num, 8, delete_item);

    QTableWidgetItem* data_item = table->item(row_num, 0);
    data_item->setData(Qt::UserRole, QVariant::fromValue(item));
}

void SellStoreWidget::CellClickedSlot(int row, int col)
{
    if(col != 7 && col != 8)
    {
        return;
    }

    if(col == 7 && row <= ui_->table->rowCount() - 2)
    {
        QTableWidgetItem* data_item = ui_->table->item(row, 0);
        current_item_ = data_item->data(Qt::UserRole).value<Shop::SellStoreItem*>();
        item_widget_->Open(current_item_, false);
    }
    else if(col == 8)
    {
        int rows_count = ui_->table->rowCount();
        if(row <= rows_count - 2)
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Delete item", "Are you sure you want to delete the item?",
                                          QMessageBox::Yes|QMessageBox::No);
            if(reply == QMessageBox::Yes)
            {
                QTableWidgetItem* data_item = ui_->table->item(row, 0);
                Shop::SellStoreItem* item = data_item->data(Qt::UserRole).value<Shop::SellStoreItem*>();
                store_->RemoveItem(item);
                delete item;
                DrawTable();
            }
        }
        else if(row == rows_count - 1)
        {
            current_item_ = new Shop::SellStoreItem();
            item_widget_->Open(current_item_, true);
        }
    }
}

void SellStoreWidget::CellDoubleClickedSlot(int row, int col)
{
    if(row <= ui_->table->rowCount() - 2 && (col > 0 && col < 7))
    {
        QTableWidgetItem* data_item = ui_->table->item(row, 0);
        current_item_ = data_item->data(Qt::UserRole).value<Shop::SellStoreItem*>();
        item_widget_->Open(current_item_, false);
    }
}

void SellStoreWidget::OnSubmitCreationSlot()
{
    store_->AddItem(current_item_);
    current_item_ = nullptr;
    DrawTable();
}

void SellStoreWidget::OnCancelCreationSlot()
{
    delete current_item_;
    current_item_ = nullptr;
}

void SellStoreWidget::OnSubmitEditingSlot()
{
    current_item_ = nullptr;
    DrawTable();
}

void SellStoreWidget::OnCancelEditingSlot()
{
    current_item_ = nullptr;
}
