#include "shopstorewidget.h"
#include "ui_shopstorewidget.h"

#include "createshopitemwidget.h"
#include "internal/node.h"
#include "internal/shopstore.h"
#include "internal/shopbeacon.h"
#include "internal/shopcommand.h"
#include "internal/shopdino.h"
#include "internal/shopitem.h"
#include "internal/shopengram.h"
#include "internal/shopexperience.h"
#include "internal/json_utils.h"

#include "editshopbeaconwidget.h"
#include "editshopcommandwidget.h"
#include "editshopdinowidget.h"
#include "editshopengramwidget.h"
#include "editshopexperiencewidget.h"
#include "editshopitemwidget.h"

#include <QMessageBox>

ShopStoreWidget::ShopStoreWidget(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui_(new Ui::ShopStoreWidget)
{
    ui_->setupUi(this);

    create_item_widget_ = new CreateShopItemWidget(this);
    beacon_widget_ = new EditShopBeaconWidget(this);
    command_widget_ = new EditShopCommandWidget(this);
    dino_widget_ = new EditShopDinoWidget(this);
    engram_widget_ = new EditShopEngramWidget(this);
    exp_widget_ = new EditShopExperienceWidget(this);
    item_widget_ = new EditShopItemWidget(this);

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
    table->setHorizontalHeaderItem(4, new QTableWidgetItem("Min level"));
    table->setHorizontalHeaderItem(5, new QTableWidgetItem("Max level"));
    table->setHorizontalHeaderItem(6, new QTableWidgetItem("Price"));
    table->setHorizontalHeaderItem(7, new QTableWidgetItem(""));
    table->setHorizontalHeaderItem(8, new QTableWidgetItem(""));

    vHeader->setSectionResizeMode(QHeaderView::Fixed);
    vHeader->setDefaultSectionSize(20);
    vHeader->setVisible(false);

    table->setFocusPolicy(Qt::NoFocus);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    table->setVisible(true);

    connect(create_item_widget_, SIGNAL(OnCreatedSignal()), this, SLOT(OnSubmitNewItemSlot()));
    connect(create_item_widget_, SIGNAL(OnCancelledSignal()), this, SLOT(OnCancelNewItemSlot()));

    connect(beacon_widget_, SIGNAL(OnSubmit()), this, SLOT(OnSubmitEditItemSlot()));
    connect(command_widget_, SIGNAL(OnSubmit()), this, SLOT(OnSubmitEditItemSlot()));
    connect(dino_widget_, SIGNAL(OnSubmit()), this, SLOT(OnSubmitEditItemSlot()));
    connect(engram_widget_, SIGNAL(OnSubmit()), this, SLOT(OnSubmitEditItemSlot()));
    connect(exp_widget_, SIGNAL(OnSubmit()), this, SLOT(OnSubmitEditItemSlot()));
    connect(item_widget_, SIGNAL(OnSubmit()), this, SLOT(OnSubmitEditItemSlot()));

    connect(beacon_widget_, SIGNAL(OnCancel()), this, SLOT(OnCancelEditItemSlot()));
    connect(command_widget_, SIGNAL(OnCancel()), this, SLOT(OnCancelEditItemSlot()));
    connect(dino_widget_, SIGNAL(OnCancel()), this, SLOT(OnCancelEditItemSlot()));
    connect(engram_widget_, SIGNAL(OnCancel()), this, SLOT(OnCancelEditItemSlot()));
    connect(exp_widget_, SIGNAL(OnCancel()), this, SLOT(OnCancelEditItemSlot()));
    connect(item_widget_, SIGNAL(OnCancel()), this, SLOT(OnCancelEditItemSlot()));

    connect(table, SIGNAL(cellClicked(int, int)), this, SLOT(CellClickedSlot(int, int)));
    connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(CellDoubleClickedSlot(int,int)));
}

ShopStoreWidget::~ShopStoreWidget()
{
    delete ui_;
}

void ShopStoreWidget::SetShopStore(Shop::ShopStore* store)
{
    store_ = store;
}

void ShopStoreWidget::Open()
{
    DrawTable();
    show();
}

void ShopStoreWidget::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    emit OnCloseSignal();
}

void ShopStoreWidget::DrawTable()
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

void ShopStoreWidget::DrawTableRow(Shop::ShopNode *item, int row_num)
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
    table->setItem(row_num, 2, (new QTableWidgetItem(Shop::Utils::TypeEnum2String(item->GetType()))));
    table->setItem(row_num, 3, (item->Description()) ? new QTableWidgetItem(*item->Description()) : new QTableWidgetItem("-"));
    table->setItem(row_num, 4, (item->MinLevel()) ? new QTableWidgetItem(QString::number(*item->MinLevel())) : new QTableWidgetItem("-"));
    table->setItem(row_num, 5, (item->MaxLevel()) ? new QTableWidgetItem(QString::number(*item->MaxLevel())) : new QTableWidgetItem("-"));
    table->setItem(row_num, 6, (item->Price()) ? new QTableWidgetItem(QString::number(*item->Price())) : new QTableWidgetItem("-"));
    table->setItem(row_num, 7, edit_item);
    table->setItem(row_num, 8, delete_item);

    QTableWidgetItem* data_item = table->item(row_num, 0);
    data_item->setData(Qt::UserRole, QVariant::fromValue(item));
}

void ShopStoreWidget::CellClickedSlot(int row, int col)
{
    if(col != 7 && col != 8)
    {
        return;
    }

    if(col == 7 && row <= ui_->table->rowCount() - 2)
    {
        QTableWidgetItem* data_item = ui_->table->item(row, 0);
        current_node_ = data_item->data(Qt::UserRole).value<Shop::ShopNode*>();
        const auto type = current_node_->GetType();
        switch (type)
        {
        case Shop::NodeType::ShopItem:
            item_widget_->Open(static_cast<Shop::ShopItem*>(current_node_));
            break;
        case Shop::NodeType::ShopDino:
            dino_widget_->Open(static_cast<Shop::ShopDino*>(current_node_));
            break;
        case Shop::NodeType::ShopBeacon:
            beacon_widget_->Open(static_cast<Shop::ShopBeacon*>(current_node_));
            break;
        case Shop::NodeType::ShopExperience:
            exp_widget_->Open(static_cast<Shop::ShopExperience*>(current_node_));
            break;
        case Shop::NodeType::ShopEngram:
            engram_widget_->Open(static_cast<Shop::ShopEngram*>(current_node_));
            break;
        case Shop::NodeType::ShopCommand:
            command_widget_->Open(static_cast<Shop::ShopCommand*>(current_node_));
            break;
        }
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
                Shop::ShopNode* item = data_item->data(Qt::UserRole).value<Shop::ShopNode*>();
                store_->RemoveItem(item);
                delete item;
                DrawTable();
            }
        }
        else if(row == rows_count - 1)
        {
            create_item_widget_->Open(&current_node_);
        }
    }
}

void ShopStoreWidget::CellDoubleClickedSlot(int row, int col)
{
    if(row <= ui_->table->rowCount() - 2 && (col > 0 && col < 7))
    {
        QTableWidgetItem* data_item = ui_->table->item(row, 0);
        current_node_ = data_item->data(Qt::UserRole).value<Shop::ShopNode*>();
        const auto type = current_node_->GetType();
        switch (type)
        {
        case Shop::NodeType::ShopItem:
            item_widget_->Open(static_cast<Shop::ShopItem*>(current_node_));
            break;
        case Shop::NodeType::ShopDino:
            dino_widget_->Open(static_cast<Shop::ShopDino*>(current_node_));
            break;
        case Shop::NodeType::ShopBeacon:
            beacon_widget_->Open(static_cast<Shop::ShopBeacon*>(current_node_));
            break;
        case Shop::NodeType::ShopExperience:
            exp_widget_->Open(static_cast<Shop::ShopExperience*>(current_node_));
            break;
        case Shop::NodeType::ShopEngram:
            engram_widget_->Open(static_cast<Shop::ShopEngram*>(current_node_));
            break;
        case Shop::NodeType::ShopCommand:
            command_widget_->Open(static_cast<Shop::ShopCommand*>(current_node_));
            break;
        }
    }
}

void ShopStoreWidget::OnSubmitNewItemSlot()
{
    store_->AddItem(current_node_);
    DrawTable();
    const auto type = current_node_->GetType();

    switch (type)
    {
    case Shop::NodeType::ShopItem:
        item_widget_->Open(static_cast<Shop::ShopItem*>(current_node_));
        break;
    case Shop::NodeType::ShopDino:
        dino_widget_->Open(static_cast<Shop::ShopDino*>(current_node_));
        break;
    case Shop::NodeType::ShopBeacon:
        beacon_widget_->Open(static_cast<Shop::ShopBeacon*>(current_node_));
        break;
    case Shop::NodeType::ShopExperience:
        exp_widget_->Open(static_cast<Shop::ShopExperience*>(current_node_));
        break;
    case Shop::NodeType::ShopEngram:
        engram_widget_->Open(static_cast<Shop::ShopEngram*>(current_node_));
        break;
    case Shop::NodeType::ShopCommand:
        command_widget_->Open(static_cast<Shop::ShopCommand*>(current_node_));
        break;
    }
}

void ShopStoreWidget::OnCancelNewItemSlot()
{
    current_node_ = nullptr;
}

void ShopStoreWidget::OnSubmitEditItemSlot()
{
    DrawTable();
    current_node_ = nullptr;
}

void ShopStoreWidget::OnCancelEditItemSlot()
{
    DrawTable();
    current_node_ = nullptr;
}
