#include "dino_block_widget.h"
#include "ui_dino_block_widget.h"

#include <QMessageBox>

#include "internal/dino.h"
#include "internal/dino_block.h"
#include "dino_widget.h"

DinoBlockWidget::DinoBlockWidget(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui_(new Ui::DinoBlockWidget)
{
    ui_->setupUi(this);
    dino_widget_ = new DinoWidget(this);

    QTableWidget* table = ui_->tableWidget;
    QHeaderView* hHeader = table->horizontalHeader();
    QHeaderView* vHeader = table->verticalHeader();

    table->setVisible(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setContentsMargins(2, 2, 2, 2);
    table->setColumnCount(10);

    hHeader->setFixedHeight(20);
    hHeader->setSectionResizeMode(0, QHeaderView::Fixed);
    hHeader->setSectionResizeMode(8, QHeaderView::Fixed);
    hHeader->setSectionResizeMode(9, QHeaderView::Fixed);
    hHeader->setSectionResizeMode(1, QHeaderView::Stretch);
    hHeader->setSectionResizeMode(2, QHeaderView::Stretch);
    hHeader->setSectionResizeMode(3, QHeaderView::Stretch);
    hHeader->setSectionResizeMode(4, QHeaderView::Stretch);
    hHeader->setSectionResizeMode(5, QHeaderView::Stretch);
    hHeader->setSectionResizeMode(6, QHeaderView::Stretch);
    hHeader->setSectionResizeMode(7, QHeaderView::Stretch);
    table->setMinimumWidth(20);
    table->setColumnWidth(0, 20);
    table->setColumnWidth(8, 20);
    table->setColumnWidth(9, 20);
    hHeader->setStretchLastSection(false);

    table->setHorizontalHeaderItem(0, new QTableWidgetItem(""));
    table->setHorizontalHeaderItem(1, new QTableWidgetItem("Blueprint"));
    table->setHorizontalHeaderItem(2, new QTableWidgetItem("Saddle blueprint"));
    table->setHorizontalHeaderItem(3, new QTableWidgetItem("Level"));
    table->setHorizontalHeaderItem(4, new QTableWidgetItem("Imprint amount"));
    table->setHorizontalHeaderItem(5, new QTableWidgetItem("Saddle quality"));
    table->setHorizontalHeaderItem(6, new QTableWidgetItem("Baby"));
    table->setHorizontalHeaderItem(7, new QTableWidgetItem("Neutered"));
    table->setHorizontalHeaderItem(8, new QTableWidgetItem(""));
    table->setHorizontalHeaderItem(9, new QTableWidgetItem(""));


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
    connect(dino_widget_, SIGNAL(SubmitCreationSignal()), this, SLOT(SubmitDinoCreationSlot()));
    connect(dino_widget_, SIGNAL(CancelCreationSignal()), this, SLOT(CancelDinoCreationSlot()));
    connect(dino_widget_, SIGNAL(SubmitEditionSignal()), this, SLOT(SubmitDinoEditionSlot()));
    connect(dino_widget_, SIGNAL(CancelEditionSignal()), this, SLOT(CancelDinoEditionSlot()));
}

DinoBlockWidget::~DinoBlockWidget()
{
    delete ui_;
    delete dino_widget_;
}

void DinoBlockWidget::Open(LootBox::DinoBlock* node)
{
    node_ = node;
    ResetUI();
    ui_->descriptionEdit->setText(node->GetDescription());
    ui_->probabilitySpin->setValue(node->GetProbability());
    UpdateTableWidget();
    show();
}

void DinoBlockWidget::Submit()
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

void DinoBlockWidget::Cancel(bool hidden)
{
    if(!hidden)
    {
        hide();
    }
    emit CancelSignal();
}

void DinoBlockWidget::ResetUI()
{
    ui_->descriptionEdit->setText("");
    ui_->probabilitySpin->setValue(0);
    ui_->tableWidget->clearContents();
}

void DinoBlockWidget::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    Cancel(true);
}

void DinoBlockWidget::UpdateTableWidget()
{
    QTableWidget* table = ui_->tableWidget;
    table->setVisible(false);
    table->clearContents();

    std::vector<LootBox::Dino*>& dinos = node_->GetDinos();
    int row = 0;
    table->setRowCount(static_cast<int>(dinos.size()) + 1);
    for(LootBox::Dino* dino : dinos)
    {
        InsertTableRow(dino, row);
        row++;
    }

    QTableWidgetItem* add_item = new QTableWidgetItem();
    add_item->setIcon(QIcon(":/icons/icons/add.png"));
    table->setItem(row, 9, add_item);

    table->setVisible(true);
}

void DinoBlockWidget::InsertTableRow(LootBox::Dino* dino, int row)
{
    const bool is_valid = dino->CheckValidity();
    QTableWidget* table = ui_->tableWidget;

    QTableWidgetItem* status_item = new QTableWidgetItem();
    status_item->setIcon(is_valid ? QIcon(":/icons/icons/valid.png") : QIcon(":/icons/icons/invalid.png"));
    QTableWidgetItem* edit_item = new QTableWidgetItem();
    edit_item->setIcon(QIcon(":/icons/icons/edit.png"));
    QTableWidgetItem* delete_item = new QTableWidgetItem();
    delete_item->setIcon(QIcon(":/icons/icons/delete.png"));

    table->setItem(row, 0, status_item);
    QTableWidgetItem* bp_item = new QTableWidgetItem(dino->GetBP());
    bp_item->setToolTip(dino->GetBP());
    table->setItem(row, 1, bp_item);
    QTableWidgetItem* saddle_bp_item = new QTableWidgetItem(dino->GetSaddleBP());
    saddle_bp_item->setToolTip(dino->GetSaddleBP());
    table->setItem(row, 2, saddle_bp_item);
    table->setItem(row, 3, new QTableWidgetItem(QString::number(dino->GetLevel())));
    table->setItem(row, 4, new QTableWidgetItem(QString::number(dino->GetImprintAmount())));
    table->setItem(row, 5, new QTableWidgetItem(QString::number(dino->GetSaddleQuality())));
    switch(dino->GetBabyFlag())
    {
    case LootBox::BooleanValue::None:
        table->setItem(row, 6, new QTableWidgetItem("Unspecified"));
        break;
    case LootBox::BooleanValue::True:
        table->setItem(row, 6, new QTableWidgetItem("True"));
        break;
    case LootBox::BooleanValue::False:
        table->setItem(row, 6, new QTableWidgetItem("False"));
        break;
    }
    switch(dino->GetNeuteredFlag())
    {
    case LootBox::BooleanValue::None:
        table->setItem(row, 7, new QTableWidgetItem("Unspecified"));
        break;
    case LootBox::BooleanValue::True:
        table->setItem(row, 7, new QTableWidgetItem("True"));
        break;
    case LootBox::BooleanValue::False:
        table->setItem(row, 7, new QTableWidgetItem("False"));
        break;
    }
    table->setItem(row, 8, edit_item);
    table->setItem(row, 9, delete_item);

    QTableWidgetItem* data_item = table->item(row, 0);
    data_item->setData(Qt::UserRole, QVariant::fromValue(dino));
}

void DinoBlockWidget::CellClickedSlot(int row, int col)
{
    if(col != 8 && col != 9)
    {
        return;
    }

    if(col == 8 && row <= ui_->tableWidget->rowCount() - 2)
    {
        QTableWidgetItem* data_item = ui_->tableWidget->item(row, 0);
        LootBox::Dino* dino = data_item->data(Qt::UserRole).value<LootBox::Dino*>();
        dino_widget_->Open(dino, false);
    }
    else if(col == 9)
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
                LootBox::Dino* dino = data_item->data(Qt::UserRole).value<LootBox::Dino*>();
                node_->RemoveDino(dino);
                delete dino;
                UpdateTableWidget();
            }
        }
        else if(row == rows_count - 1)
        {
            new_dino_ptr_ = reinterpret_cast<LootBox::Dino*>(LootBox::PrimalNode::Create(node_, LootBox::NodeType::Dino));
            dino_widget_->Open(new_dino_ptr_, true);
        }
    }
}

void DinoBlockWidget::CellDoubleClickedSlot(int row, int col)
{
    if(row <= ui_->tableWidget->rowCount() - 2 && (col > 0 && col < 8))
    {
        QTableWidgetItem* data_item = ui_->tableWidget->item(row, 0);
        LootBox::Dino* dino = data_item->data(Qt::UserRole).value<LootBox::Dino*>();
        dino_widget_->Open(dino, false);
    }
}

void DinoBlockWidget::SubmitDinoCreationSlot()
{
    if(new_dino_ptr_)
    {
        node_->AddDino(new_dino_ptr_);
        new_dino_ptr_ = nullptr;
        UpdateTableWidget();
    }
}

void DinoBlockWidget::CancelDinoCreationSlot()
{
    if(new_dino_ptr_)
    {
        delete new_dino_ptr_;
        new_dino_ptr_ = nullptr;
    }
}

void DinoBlockWidget::SubmitDinoEditionSlot()
{
    UpdateTableWidget();
}

void DinoBlockWidget::CancelDinoEditionSlot()
{
}
