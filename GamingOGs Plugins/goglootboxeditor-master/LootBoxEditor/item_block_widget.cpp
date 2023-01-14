#include "item_block_widget.h"
#include "ui_item_block_widget.h"

#include <QMessageBox>

#include "internal/node.h"
#include "internal/item_block.h"
#include "internal/item.h"
#include "item_widget.h"

ItemBlockWidget::ItemBlockWidget(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui_(new Ui::ItemBlockWidget)
{
    ui_->setupUi(this);

    item_widget_ = new ItemWidget(this);

    QTableWidget* table = ui_->tableWidget;
    QHeaderView* hHeader = table->horizontalHeader();
    QHeaderView* vHeader = table->verticalHeader();

    table->setVisible(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
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
    table->setMinimumWidth(20);
    table->setColumnWidth(0, 20);
    table->setColumnWidth(7, 20);
    table->setColumnWidth(8, 20);
    hHeader->setStretchLastSection(false);

    table->setHorizontalHeaderItem(0, new QTableWidgetItem(""));
    table->setHorizontalHeaderItem(1, new QTableWidgetItem("Blueprint"));
    table->setHorizontalHeaderItem(2, new QTableWidgetItem("Amount"));
    table->setHorizontalHeaderItem(3, new QTableWidgetItem("Min quality"));
    table->setHorizontalHeaderItem(4, new QTableWidgetItem("Max quality"));
    table->setHorizontalHeaderItem(5, new QTableWidgetItem("Add stat"));
    table->setHorizontalHeaderItem(6, new QTableWidgetItem("Blueprint chance"));
    table->setHorizontalHeaderItem(7, new QTableWidgetItem(""));
    table->setHorizontalHeaderItem(8, new QTableWidgetItem(""));


    vHeader->setSectionResizeMode(QHeaderView::Fixed);
    vHeader->setDefaultSectionSize(20);
    vHeader->setVisible(false);

    table->setFocusPolicy(Qt::NoFocus);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    table->setVisible(true);

    connect(ui_->submitButton, &QPushButton::clicked, [this]() { this->Submit(); });
    connect(ui_->cancelButton, &QPushButton::clicked, [this]() { this->Cancel(false); });
    connect(ui_->tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(CellClickedSlot(int, int)));
    connect(ui_->tableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(CellDoubleClickedSlot(int, int)));
    connect(item_widget_, SIGNAL(SubmitCreationSignal()), this, SLOT(SubmitItemCreationSlot()));
    connect(item_widget_, SIGNAL(CancelCreationSignal()), this, SLOT(CancelItemCreationSlot()));
    connect(item_widget_, SIGNAL(SubmitEditionSignal()), this, SLOT(SubmitItemEditionSlot()));
    connect(item_widget_, SIGNAL(CancelEditionSignal()), this, SLOT(CancelItemEditionSlot()));
}

ItemBlockWidget::~ItemBlockWidget()
{
    delete ui_;
    delete item_widget_;
}

void ItemBlockWidget::Open(LootBox::ItemBlock* node)
{
    node_ = node;
    ResetUI();
    ui_->descriptionEdit->setText(node->GetDescription());
    ui_->probabilitySpin->setValue(node->GetProbability());
    UpdateTableWidget();
    show();
}

void ItemBlockWidget::Submit()
{
    const QString description = ui_->descriptionEdit->text();
    const int probability = ui_->probabilitySpin->value();

    bool is_valid = true;

    if(description.isEmpty())
    {
        ui_->tooltipLabel->setText("Description can't be empty");
        is_valid = false;
    }
    else if(probability <= 0 || probability > 100)
    {
        ui_->tooltipLabel->setText("Probability must be in range of 0 - 100");
        is_valid = false;
    }

    if(!is_valid)
    {
        return;
    }

    node_->SetDescription(description);
    node_->SetProbability(probability);

    hide();
    emit SubmitSignal();
}

void ItemBlockWidget::Cancel(bool hidden)
{
    if(!hidden)
    {
        hide();
    }
    emit CancelSignal();
}

void ItemBlockWidget::ResetUI()
{
    ui_->descriptionEdit->setText("");
    ui_->probabilitySpin->setValue(0);
    ui_->tableWidget->clearContents();
}

void ItemBlockWidget::closeEvent(QCloseEvent* event)
{
    QWidget::closeEvent(event);
    Cancel(true);
}

void ItemBlockWidget::UpdateTableWidget()
{
    QTableWidget* table = ui_->tableWidget;
    table->setVisible(false);
    table->clearContents();

    std::vector<LootBox::Item*>& items = node_->GetItems();
    int row = 0;
    table->setRowCount(static_cast<int>(items.size()) + 1);
    for(LootBox::Item* item : items)
    {
        InsertTableRow(item, row);
        row++;
    }

    QTableWidgetItem* add_item = new QTableWidgetItem();
    add_item->setIcon(QIcon(":/icons/icons/add.png"));
    table->setItem(row, 8, add_item);

    table->setVisible(true);
}

void ItemBlockWidget::InsertTableRow(LootBox::Item* item, int row)
{
    const bool is_valid = item->CheckValidity();
    QTableWidget* table = ui_->tableWidget;

    QTableWidgetItem* status_item = new QTableWidgetItem();
    status_item->setIcon(is_valid ? QIcon(":/icons/icons/valid.png") : QIcon(":/icons/icons/invalid.png"));
    QTableWidgetItem* edit_item = new QTableWidgetItem();
    edit_item->setIcon(QIcon(":/icons/icons/edit.png"));
    QTableWidgetItem* delete_item = new QTableWidgetItem();
    delete_item->setIcon(QIcon(":/icons/icons/delete.png"));

    table->setItem(row, 0, status_item);
    QTableWidgetItem* bp_item = new QTableWidgetItem(item->GetBP());
    bp_item->setToolTip(item->GetBP());
    table->setItem(row, 1, bp_item);
    table->setItem(row, 2, new QTableWidgetItem(QString::number(item->GetAmount())));
    table->setItem(row, 3, new QTableWidgetItem(QString::number(item->GetMinQuality())));
    table->setItem(row, 4, new QTableWidgetItem(QString::number(item->GetMaxQuality())));
    table->setItem(row, 5, new QTableWidgetItem(QString::number(item->GetAddStatFlag())));
    table->setItem(row, 6, new QTableWidgetItem(QString::number(item->GetBPChance())));
    table->setItem(row, 7, edit_item);
    table->setItem(row, 8, delete_item);

    QTableWidgetItem* data_item = table->item(row, 0);
    data_item->setData(Qt::UserRole, QVariant::fromValue(item));
}

void ItemBlockWidget::CellClickedSlot(int row, int col)
{
    if(col != 7 && col != 8)
    {
        return;
    }

    if(col == 7 && row <= ui_->tableWidget->rowCount() - 2)
    {
        QTableWidgetItem* data_item = ui_->tableWidget->item(row, 0);
        LootBox::Item* item = data_item->data(Qt::UserRole).value<LootBox::Item*>();
        item_widget_->Open(item, false);
    }
    else if(col == 8)
    {
        int rows_count = ui_->tableWidget->rowCount();
        if(row <= rows_count - 2)
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Delete item", "Are you sure you want to delete the item?",
                                          QMessageBox::Yes|QMessageBox::No);
            if(reply == QMessageBox::Yes)
            {
                QTableWidgetItem* data_item = ui_->tableWidget->item(row, 0);
                LootBox::Item* item = data_item->data(Qt::UserRole).value<LootBox::Item*>();
                node_->RemoveItem(item);
                delete item;
                UpdateTableWidget();
            }
        }
        else if(row == rows_count - 1)
        {
            new_item_ptr_ = reinterpret_cast<LootBox::Item*>(LootBox::PrimalNode::Create(node_, LootBox::NodeType::Item));
            item_widget_->Open(new_item_ptr_, true);
        }
    }
}

void ItemBlockWidget::CellDoubleClickedSlot(int row, int col)
{
    if(row <= ui_->tableWidget->rowCount() - 2 && (col > 0 && col < 7))
    {
        QTableWidgetItem* data_item = ui_->tableWidget->item(row, 0);
        LootBox::Item* item = data_item->data(Qt::UserRole).value<LootBox::Item*>();
        item_widget_->Open(item, false);
    }
}

void ItemBlockWidget::SubmitItemCreationSlot()
{
    if(new_item_ptr_)
    {
        node_->AddItem(new_item_ptr_);
        new_item_ptr_ = nullptr;
        UpdateTableWidget();
    }
}

void ItemBlockWidget::CancelItemCreationSlot()
{
    if(new_item_ptr_)
    {
        delete new_item_ptr_;
        new_item_ptr_ = nullptr;
    }
}

void ItemBlockWidget::SubmitItemEditionSlot()
{
    UpdateTableWidget();
}

void ItemBlockWidget::CancelItemEditionSlot()
{
}
