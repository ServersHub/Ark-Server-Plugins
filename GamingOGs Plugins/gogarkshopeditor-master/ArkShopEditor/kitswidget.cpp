#include "kitswidget.h"
#include "ui_kitswidget.h"

#include "addkitwidget.h"
#include "editkitwidget.h"

#include "internal/kits.h"
#include "internal/kit.h"

#include <QMessageBox>

KitsWidget::KitsWidget(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui_(new Ui::KitsWidget)
{
    ui_->setupUi(this);

    add_kit_widget_ = new AddKitWidget(this);
    edit_kit_widget_ = new EditKitWidget(this);

    QTableWidget* table = ui_->table;
    QHeaderView* hHeader = table->horizontalHeader();
    QHeaderView* vHeader = table->verticalHeader();

    table->setVisible(false);
    table->setContentsMargins(2, 2, 2, 2);
    table->setColumnCount(11);

    hHeader->setFixedHeight(20);
    hHeader->setSectionResizeMode(0, QHeaderView::Fixed);
    hHeader->setSectionResizeMode(9, QHeaderView::Fixed);
    hHeader->setSectionResizeMode(10, QHeaderView::Fixed);
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
    table->setColumnWidth(9, 20);
    table->setColumnWidth(10, 20);
    hHeader->setStretchLastSection(false);

    table->setHorizontalHeaderItem(0, new QTableWidgetItem(""));
    table->setHorizontalHeaderItem(1, new QTableWidgetItem("Name"));
    table->setHorizontalHeaderItem(2, new QTableWidgetItem("Description"));
    table->setHorizontalHeaderItem(3, new QTableWidgetItem("Permissions"));
    table->setHorizontalHeaderItem(4, new QTableWidgetItem("Default amount"));
    table->setHorizontalHeaderItem(5, new QTableWidgetItem("Price"));
    table->setHorizontalHeaderItem(6, new QTableWidgetItem("Min level"));
    table->setHorizontalHeaderItem(7, new QTableWidgetItem("Max level"));
    table->setHorizontalHeaderItem(8, new QTableWidgetItem("Only for spawn"));
    table->setHorizontalHeaderItem(9, new QTableWidgetItem(""));
    table->setHorizontalHeaderItem(10, new QTableWidgetItem(""));

    vHeader->setSectionResizeMode(QHeaderView::Fixed);
    vHeader->setDefaultSectionSize(20);
    vHeader->setVisible(false);

    table->setFocusPolicy(Qt::NoFocus);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    table->setVisible(true);

    connect(table, SIGNAL(cellClicked(int, int)), this, SLOT(CellClickedSlot(int, int)));
    connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(CellDoubleClickedSlot(int,int)));
    connect(add_kit_widget_, SIGNAL(SubmitCreationSignal()), this, SLOT(OnSubmitCreationSlot()));
    connect(add_kit_widget_, SIGNAL(CancelCreationSignal()), this, SLOT(OnCancelCreationSlot()));
    connect(edit_kit_widget_, SIGNAL(SubmitEditingSignal()), this, SLOT(OnSubmitEditingSlot()));
    connect(edit_kit_widget_, SIGNAL(CancelEditingSignal()), this, SLOT(OnCancelEditingSlot()));
}

KitsWidget::~KitsWidget()
{
    delete add_kit_widget_;
    delete edit_kit_widget_;
    delete ui_;
}

void KitsWidget::SetKits(Shop::Kits *kits)
{
    kits_ = kits;
}

void KitsWidget::Open()
{
    DrawTable();
    show();
}

void KitsWidget::closeEvent(QCloseEvent* event)
{
    QWidget::closeEvent(event);
    emit OnCloseSignal();
}

void KitsWidget::DrawTable()
{
    ui_->table->setVisible(false);
    ui_->table->clearContents();

    auto& kits = kits_->GetKits();

    ui_->table->setRowCount(static_cast<int>(kits.size()) + 1);
    ui_->table->setColumnCount(11);

    int row = 0;
    for(auto kit : kits)
    {
        DrawTableRow(kit, row);
        row++;
    }

    QTableWidgetItem* add_item = new QTableWidgetItem();
    add_item->setIcon(QIcon(":/icons/icons/add.png"));
    ui_->table->setItem(row, 10, add_item);

    ui_->table->setVisible(true);
}

void KitsWidget::DrawTableRow(Shop::Kit *kit, int row_num)
{
    const bool is_valid = kit->IsValid();
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
    table->setItem(row_num, 1, (kit->Name()) ? new QTableWidgetItem(*kit->Name()) : new QTableWidgetItem("-"));
    table->setItem(row_num, 2, (kit->Description()) ? new QTableWidgetItem(*kit->Description()) : new QTableWidgetItem("-"));
    table->setItem(row_num, 3, (kit->Permissions()) ? new QTableWidgetItem(*kit->Permissions()) : new QTableWidgetItem("-"));
    table->setItem(row_num, 4, (kit->DefaultAmount()) ? new QTableWidgetItem(QString::number(*kit->DefaultAmount())) : new QTableWidgetItem("-"));
    table->setItem(row_num, 5, (kit->Price()) ? new QTableWidgetItem(QString::number(*kit->Price())) : new QTableWidgetItem("-"));
    table->setItem(row_num, 6, (kit->MinLevel()) ? new QTableWidgetItem(QString::number(*kit->MinLevel())) : new QTableWidgetItem("-"));
    table->setItem(row_num, 7, (kit->MaxLevel()) ? new QTableWidgetItem(QString::number(*kit->MaxLevel())) : new QTableWidgetItem("-"));
    table->setItem(row_num, 8, (kit->IsOnlyFromSpawn()) ? (*kit->IsOnlyFromSpawn()) ? new QTableWidgetItem("True") : new QTableWidgetItem("False") : new QTableWidgetItem("-"));
    table->setItem(row_num, 9, edit_item);
    table->setItem(row_num, 10, delete_item);

    QTableWidgetItem* data_item = table->item(row_num, 0);
    data_item->setData(Qt::UserRole, QVariant::fromValue(kit));
}

void KitsWidget::CellClickedSlot(int row, int col)
{
    if(col != 9 && col != 10)
    {
        return;
    }

    if(col == 9 && row <= ui_->table->rowCount() - 2)
    {
        QTableWidgetItem* data_item = ui_->table->item(row, 0);
        current_kit_ = data_item->data(Qt::UserRole).value<Shop::Kit*>();
        edit_kit_widget_->Open(current_kit_);
    }
    else if(col == 10)
    {
        int rows_count = ui_->table->rowCount();
        if(row <= rows_count - 2)
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Delete kit", "Are you sure you want to delete the kit?",
                                          QMessageBox::Yes|QMessageBox::No);
            if(reply == QMessageBox::Yes)
            {
                QTableWidgetItem* data_item = ui_->table->item(row, 0);
                Shop::Kit* kit = data_item->data(Qt::UserRole).value<Shop::Kit*>();
                kits_->RemoveKit(kit);
                delete kit;
                DrawTable();
            }
        }
        else if(row == rows_count - 1)
        {
            current_kit_ = new Shop::Kit("");
            add_kit_widget_->Open(current_kit_);
        }
    }
}

void KitsWidget::CellDoubleClickedSlot(int row, int col)
{
    if(row <= ui_->table->rowCount() - 2 && (col > 0 && col < 9))
    {
        QTableWidgetItem* data_item = ui_->table->item(row, 0);
        current_kit_ = data_item->data(Qt::UserRole).value<Shop::Kit*>();
        edit_kit_widget_->Open(current_kit_);
    }
}

void KitsWidget::OnSubmitCreationSlot()
{
    kits_->AddKit(current_kit_);
    current_kit_ = nullptr;
    DrawTable();
}

void KitsWidget::OnCancelCreationSlot()
{
    delete current_kit_;
    current_kit_ = nullptr;
}

void KitsWidget::OnSubmitEditingSlot()
{
    current_kit_ = nullptr;
    DrawTable();
}

void KitsWidget::OnCancelEditingSlot()
{
    current_kit_ = nullptr;
}
