#include "box_widget.h"
#include "ui_box_widget.h"

#include <QMessageBox>
#include <QtDebug>

#include "internal/node.h"
#include "internal/item_block.h"
#include "internal/dino_block.h"
#include "internal/beacon_block.h"
#include "internal/command_block.h"
#include "internal/points_block.h"

#include "create_block_node_widget.h"
#include "bulk_add_item_widget.h"
#include "bulk_add_dino_widget.h"
#include "item_block_widget.h"
#include "dino_block_widget.h"
#include "beacon_block_widget.h"
#include "command_block_widget.h"
#include "points_block_widget.h"

BoxWidget::BoxWidget(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui_(new Ui::BoxWidget),
    new_node_ptr_(nullptr)
{
    ui_->setupUi(this);
    create_block_widget_ = new CreateBlockNodeWidget(this);
    item_block_widget_ = new ItemBlockWidget(this);
    dino_block_widget_ = new DinoBlockWidget(this);
    beacon_block_widget_ = new BeaconBlockWidget(this);
    command_block_widget_ = new CommandBlockWidget(this);
    points_block_widget_ = new PointsBlockWidget(this);
    bulk_add_item_widget_ = new BulkAddItemWidget(this);
    bulk_add_dino_widget_ = new BulkAddDinoWidget(this);

    QTableWidget* table = ui_->tableWidget;
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
    table->setHorizontalHeaderItem(1, new QTableWidgetItem("Type"));
    table->setHorizontalHeaderItem(2, new QTableWidgetItem("Description"));
    table->setHorizontalHeaderItem(3, new QTableWidgetItem("Probability"));
    table->setHorizontalHeaderItem(4, new QTableWidgetItem("Elements count"));
    table->setHorizontalHeaderItem(5, new QTableWidgetItem(""));
    table->setHorizontalHeaderItem(6, new QTableWidgetItem(""));


    vHeader->setSectionResizeMode(QHeaderView::Fixed);
    vHeader->setDefaultSectionSize(20);
    vHeader->setVisible(false);

    table->setFocusPolicy(Qt::NoFocus);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    table->setVisible(true);

    connect(ui_->submitButton, &QPushButton::clicked, [this]() { this->Submit(); });
    connect(ui_->cancelButton, &QPushButton::clicked, [this]() { this->Cancel(); });
    connect(ui_->bulkAddItemButton, &QPushButton::clicked, [this]() { this->bulk_add_item_widget_->Open(this->box_); });
    connect(ui_->bulkAddDinoButton, &QPushButton::clicked, [this]() { this->bulk_add_dino_widget_->Open(this->box_); });
    connect(bulk_add_item_widget_, SIGNAL(SubmittedAddingSignal()), this, SLOT(BulkAddedSlot()));
    connect(bulk_add_dino_widget_, SIGNAL(SubmittedAddingSignal()), this, SLOT(BulkAddedSlot()));
    connect(ui_->tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(CellClickedSlot(int, int)));
    connect(ui_->tableWidget, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(CellDoubleClickedSlot(int,int)));
    connect(create_block_widget_, SIGNAL(SubmitSignal()), this, SLOT(SubmitNodeCreationSlot()));
    connect(create_block_widget_, SIGNAL(CancelSignal()), this, SLOT(CancelNodeCreationSlot()));
    connect(item_block_widget_, SIGNAL(SubmitSignal()), this, SLOT(SubmitNodeEditionSlot()));
    connect(dino_block_widget_, SIGNAL(SubmitSignal()), this, SLOT(SubmitNodeEditionSlot()));
    connect(beacon_block_widget_, SIGNAL(SubmitSignal()), this, SLOT(SubmitNodeEditionSlot()));
    connect(command_block_widget_, SIGNAL(SubmitSignal()), this, SLOT(SubmitNodeEditionSlot()));
    connect(points_block_widget_, SIGNAL(SubmitSignal()), this, SLOT(SubmitNodeEditionSlot()));
    connect(item_block_widget_, SIGNAL(CancelSignal()), this, SLOT(CancelNodeEditionSlot()));
    connect(dino_block_widget_, SIGNAL(CancelSignal()), this, SLOT(CancelNodeEditionSlot()));
    connect(beacon_block_widget_, SIGNAL(CancelSignal()), this, SLOT(CancelNodeEditionSlot()));
    connect(command_block_widget_, SIGNAL(CancelSignal()), this, SLOT(CancelNodeEditionSlot()));
    connect(points_block_widget_, SIGNAL(CancelSignal()), this, SLOT(CancelNodeEditionSlot()));
}

BoxWidget::~BoxWidget()
{
    delete ui_;
    delete create_block_widget_;
    delete item_block_widget_;
    delete dino_block_widget_;
    delete beacon_block_widget_;
    delete command_block_widget_;
    delete points_block_widget_;
    delete bulk_add_item_widget_;
    delete bulk_add_dino_widget_;
}

void BoxWidget::Open(LootBox::Box* box, bool is_creation)
{
    is_creation_mode_ = is_creation;
    box_ = box;

    TableEventFilter* f = new TableEventFilter(ui_->tableWidget);

    ui_->tableWidget->installEventFilter(f);

    if(is_creation)
    {
        SetupForCreate();
    }
    else
    {
        SetupForEdit();
    }

    show();
}

void BoxWidget::SetupForCreate()
{
    ResetUI();
    UpdateTableWidget();
    setWindowTitle("Add box");

    ui_->bulkAddItemButton->setEnabled(false);
    ui_->bulkAddDinoButton->setEnabled(false);
}

void BoxWidget::SetupForEdit()
{
    ResetUI();
    UpdateTableWidget();
    setWindowTitle("Edit box");

    ui_->bulkAddItemButton->setEnabled(true);
    ui_->bulkAddDinoButton->setEnabled(true);

    ui_->boxNameEdit->setText(box_->GetBoxName());
    ui_->gamblePercentSpin->setValue(box_->GetGamblePercent());
    ui_->gamblePriceSpin->setValue(box_->GetGamblePrice());
    ui_->permissionEdit->setText(box_->GetPermission());
    ui_->minAmountSpin->setValue(box_->GetMinAmount());
    ui_->maxAmountSpin->setValue(box_->GetMaxAmount());
}

void BoxWidget::ResetUI()
{
    ui_->boxNameEdit->setText("");
    ui_->gamblePercentSpin->setValue(0);
    ui_->gamblePriceSpin->setValue(0);
    ui_->permissionEdit->setText("");
    ui_->minAmountSpin->setValue(0);
    ui_->maxAmountSpin->setValue(0);
    ui_->tableWidget->clearContents();
}

void BoxWidget::UpdateTableWidget()
{
    QTableWidget* table = ui_->tableWidget;
    disconnect(table, SIGNAL(cellChanged(int, int)), this, SLOT(CellChangedSlot(int, int)));

    table->setVisible(false);
    table->clearContents();

    std::vector<LootBox::BlockNode*>& nodes = box_->GetNodes();
    table->setRowCount(static_cast<int>(nodes.size()) + 1);
    table->setColumnCount(7);

    int row = 0;
    for(LootBox::BlockNode* node : nodes)
    {
        InsertTableNode(node, row);
        row++;
    }

    QTableWidgetItem* add_item = new QTableWidgetItem();
    add_item->setIcon(QIcon(":/icons/icons/add.png"));
    table->setItem(row, 6, add_item);

    table->setVisible(true);

    connect(table, SIGNAL(cellChanged(int, int)), this, SLOT(CellChangedSlot(int, int)));
}

void BoxWidget::InsertTableNode(LootBox::BlockNode* node, int row)
{
    const bool is_valid = node->CheckValidity();
    QTableWidget* table = ui_->tableWidget;

    QTableWidgetItem* status_item = new QTableWidgetItem();
    status_item->setIcon(is_valid ? QIcon(":/icons/icons/valid.png") : QIcon(":/icons/icons/invalid.png"));
    status_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    QTableWidgetItem* edit_item = new QTableWidgetItem();
    edit_item->setIcon(QIcon(":/icons/icons/edit.png"));
    edit_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    QTableWidgetItem* delete_item = new QTableWidgetItem();
    delete_item->setIcon(QIcon(":/icons/icons/delete.png"));
    delete_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

    table->setItem(row, 0, status_item);
    QTableWidgetItem* type_item;
    switch(node->GetType())
    {
    case LootBox::NodeType::ItemBlock:
        type_item = new QTableWidgetItem("Item");
        break;
    case LootBox::NodeType::DinoBlock:
        type_item = new QTableWidgetItem("Dino");
        break;
    case LootBox::NodeType::BeaconBlock:
        type_item = new QTableWidgetItem("Beacon");
        break;
    case LootBox::NodeType::CommandBlock:
        type_item = new QTableWidgetItem("Command");
        break;
    case LootBox::NodeType::PointsBlock:
        type_item = new QTableWidgetItem("Points");
        break;
    default:
        type_item = new QTableWidgetItem("Unknown");
    }
    type_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    table->setItem(row, 1, type_item);
    table->setItem(row, 2, new QTableWidgetItem(node->GetDescription()));
    table->setItem(row, 3, new QTableWidgetItem(QString::number(node->GetProbability())));
    QTableWidgetItem* count_item;
    switch(node->GetType())
    {
    case LootBox::NodeType::ItemBlock:
        count_item = new QTableWidgetItem(QString::number(reinterpret_cast<LootBox::ItemBlock*>(node)->GetItems().size()));
        break;
    case LootBox::NodeType::DinoBlock:
        count_item = new QTableWidgetItem(QString::number(reinterpret_cast<LootBox::DinoBlock*>(node)->GetDinos().size()));
        break;
    case LootBox::NodeType::BeaconBlock:
    case LootBox::NodeType::CommandBlock:
    case LootBox::NodeType::PointsBlock:
        count_item = new QTableWidgetItem(QString::number(1));
        break;
    default:
        count_item = new QTableWidgetItem(QString::number(0));
    }
    count_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    table->setItem(row, 4, count_item);
    table->setItem(row, 5, edit_item);
    table->setItem(row, 6, delete_item);

    QTableWidgetItem* data_item = table->item(row, 0);
    data_item->setData(Qt::UserRole, QVariant::fromValue(node));
}

void BoxWidget::closeEvent(QCloseEvent* event)
{
    Cancel();
    QWidget::closeEvent(event);
}

void BoxWidget::Submit()
{
    const QString box_name = ui_->boxNameEdit->text();
    const int gamble_percent = ui_->gamblePercentSpin->value();
    const int gamble_price = ui_->gamblePriceSpin->value();
    const QString permission = ui_->permissionEdit->text();
    const int min_amount = ui_->minAmountSpin->value();
    const int max_amount = ui_->maxAmountSpin->value();

    bool is_valid = true;

    if(box_name.isEmpty())
    {
        ui_->tooltipEdit->setText("Box name must not be empty");
        is_valid = false;
    }
    else if(gamble_percent <= 0)
    {
        ui_->tooltipEdit->setText("Gamble percent must be greater than zero");
        is_valid = false;
    }
    else if(gamble_price <= 0)
    {
        ui_->tooltipEdit->setText("Gamble price must be greater than zero");
        is_valid = false;
    }
    else if(min_amount < 0)
    {
        ui_->tooltipEdit->setText("Minimum amount must be a positive integer");
        is_valid = false;
    }
    else if(max_amount < 0 || ui_->maxAmountSpin->value() < ui_->minAmountSpin->value())
    {
        ui_->tooltipEdit->setText("Maximum amount must be a positive integer and greater or equal to minimum amount");
        is_valid = false;
    }

    if(is_valid)
    {
        box_->SetBoxName(box_name);
        box_->SetGamblePercent(gamble_percent);
        box_->SetGamblePrice(gamble_price);
        box_->SetPermission(permission);
        box_->SetMinAmount(min_amount);
        box_->SetMaxAmount(max_amount);

        if(isVisible())
        {
            hide();
        }

        if(is_creation_mode_)
        {
            emit SubmitCreationSignal();
        }
        else
        {
            emit SubmitEditionSignal();
        }
    }
}

void BoxWidget::Cancel()
{
    if(is_creation_mode_)
    {
        emit CancelCreationSignal();
    }
    else
    {
        emit CancelEditionSignal();
    }
    hide();
}

void BoxWidget::EditNode(LootBox::BlockNode* node)
{
    switch(node->GetType())
    {
    case LootBox::NodeType::ItemBlock:
        item_block_widget_->Open(reinterpret_cast<LootBox::ItemBlock*>(node));
        break;
    case LootBox::NodeType::DinoBlock:
        dino_block_widget_->Open(reinterpret_cast<LootBox::DinoBlock*>(node));
        break;
    case LootBox::NodeType::BeaconBlock:
        beacon_block_widget_->Open(reinterpret_cast<LootBox::BeaconBlock*>(node));
        break;
    case LootBox::NodeType::CommandBlock:
        command_block_widget_->Open(reinterpret_cast<LootBox::CommandBlock*>(node));
        break;
    case LootBox::NodeType::PointsBlock:
        points_block_widget_->Open(reinterpret_cast<LootBox::PointsBlock*>(node));
        break;
    default:
        break;
    }
}

void BoxWidget::CellClickedSlot(int row, int col)
{
    if(col != 5 && col != 6)
    {
        return;
    }

    if(col == 5 && row <= ui_->tableWidget->rowCount() - 2)
    {
        QTableWidgetItem* data_item = ui_->tableWidget->item(row, 0);
        LootBox::BlockNode* node = data_item->data(Qt::UserRole).value<LootBox::BlockNode*>();
        EditNode(node);
    }
    else if(col == 6)
    {
        int rows_count = ui_->tableWidget->rowCount();
        if(row <= rows_count - 2)
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Delete box", "Are you sure you want to delete the block?",
                                          QMessageBox::Yes|QMessageBox::No);
            if(reply == QMessageBox::Yes)
            {
                QTableWidgetItem* item = ui_->tableWidget->item(row, 0);
                LootBox::BlockNode* node = item->data(Qt::UserRole).value<LootBox::BlockNode*>();
                box_->RemoveNode(node);
                delete node;
                UpdateTableWidget();
            }
        }
        else if(row == rows_count - 1)
        {
            create_block_widget_->Open(box_, &new_node_ptr_);
        }
    }
}

void BoxWidget::CellChangedSlot(int row, int col)
{
    if(row <= ui_->tableWidget->rowCount() - 2 && (col == 2 || col == 3))
    {
        QTableWidgetItem* item = ui_->tableWidget->item(row, col);
        LootBox::BlockNode* node = ui_->tableWidget->item(row, 0)->data(Qt::UserRole).value<LootBox::BlockNode*>();
        if(col == 2)
        {
            if(item->text().isEmpty())
            {
                item->setText(node->GetDescription());
                return;
            }
            node->SetDescription(item->text());
        }
        else if(col == 3)
        {
            if(item->text().toLong() < 0)
            {
                item->setText(QString::number(node->GetProbability()));
                return;
            }
            node->SetProbability(item->text().toInt());
        }

        const bool is_valid = node->CheckValidity();
        QTableWidgetItem* status_item = ui_->tableWidget->item(row, 0);
        status_item->setIcon(is_valid ? QIcon(":/icons/icons/valid.png") : QIcon(":/icons/icons/invalid.png"));
    }
}

void BoxWidget::CellDoubleClickedSlot(int row, int col)
{
    if(row <= ui_->tableWidget->rowCount() - 2 && (col == 1 || col == 4))
    {
        QTableWidgetItem* data_item = ui_->tableWidget->item(row, 0);
        LootBox::BlockNode* node = data_item->data(Qt::UserRole).value<LootBox::BlockNode*>();
        EditNode(node);
    }
}

void BoxWidget::SubmitNodeCreationSlot()
{
    if(new_node_ptr_)
    {
        box_->AddNode(new_node_ptr_);
        UpdateTableWidget();
        EditNode(new_node_ptr_);
        new_node_ptr_ = nullptr;
    }
}

void BoxWidget::CancelNodeCreationSlot()
{
    new_node_ptr_ = nullptr;
}

void BoxWidget::SubmitNodeEditionSlot()
{
    UpdateTableWidget();
}

void BoxWidget::CancelNodeEditionSlot()
{
}

void BoxWidget::BulkAddedSlot()
{
    UpdateTableWidget();
}
