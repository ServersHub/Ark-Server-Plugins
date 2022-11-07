#include "editkitwidget.h"
#include "ui_editkitwidget.h"

#include "internal/kit.h"
#include "internal/item.h"
#include "internal/dino.h"
#include "itemwidget.h"
#include "dinowidget.h"

#include <QMessageBox>

EditKitWidget::EditKitWidget(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui_(new Ui::EditKitWidget)
{
    ui_->setupUi(this);

    item_widget_ = new ItemWidget(this);
    dino_widget_ = new DinoWidget(this);

    connect(ui_->submitBtn, &QPushButton::clicked, [this]() { this->Submit(); });
    connect(ui_->cancelBtn, &QPushButton::clicked, [this]() {
        this->hide();
        emit CancelEditingSignal();
    });

    QTableWidget* itemsTable = ui_->itemsTable;
    QHeaderView* itemsHHeader = itemsTable->horizontalHeader();
    QHeaderView* itemsVHeader = itemsTable->verticalHeader();

    itemsTable->setVisible(false);
    itemsTable->setContentsMargins(2, 2, 2, 2);
    itemsTable->setColumnCount(7);

    itemsHHeader->setFixedHeight(20);
    itemsHHeader->setSectionResizeMode(0, QHeaderView::Fixed);
    itemsHHeader->setSectionResizeMode(5, QHeaderView::Fixed);
    itemsHHeader->setSectionResizeMode(6, QHeaderView::Fixed);
    itemsHHeader->setSectionResizeMode(1, QHeaderView::Stretch);
    itemsHHeader->setSectionResizeMode(2, QHeaderView::Stretch);
    itemsHHeader->setSectionResizeMode(3, QHeaderView::Stretch);
    itemsHHeader->setSectionResizeMode(4, QHeaderView::Stretch);
    itemsTable->setMinimumWidth(20);
    itemsTable->setColumnWidth(0, 20);
    itemsTable->setColumnWidth(5, 20);
    itemsTable->setColumnWidth(6, 20);
    itemsHHeader->setStretchLastSection(false);

    itemsTable->setHorizontalHeaderItem(0, new QTableWidgetItem(""));
    itemsTable->setHorizontalHeaderItem(1, new QTableWidgetItem("Blueprint"));
    itemsTable->setHorizontalHeaderItem(2, new QTableWidgetItem("Quality"));
    itemsTable->setHorizontalHeaderItem(3, new QTableWidgetItem("Amount"));
    itemsTable->setHorizontalHeaderItem(4, new QTableWidgetItem("Force blueprint"));
    itemsTable->setHorizontalHeaderItem(5, new QTableWidgetItem(""));
    itemsTable->setHorizontalHeaderItem(6, new QTableWidgetItem(""));

    itemsVHeader->setSectionResizeMode(QHeaderView::Fixed);
    itemsVHeader->setDefaultSectionSize(20);
    itemsVHeader->setVisible(false);

    itemsTable->setFocusPolicy(Qt::NoFocus);
    itemsTable->setSelectionMode(QAbstractItemView::NoSelection);
    itemsTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    itemsTable->setVisible(true);

    QTableWidget* dinosTable = ui_->dinosTable;
    QHeaderView* dinosHHeader = dinosTable->horizontalHeader();
    QHeaderView* dinosVHeader = dinosTable->verticalHeader();

    dinosTable->setVisible(false);
    dinosTable->setContentsMargins(2, 2, 2, 2);
    dinosTable->setColumnCount(6);

    dinosHHeader->setFixedHeight(20);
    dinosHHeader->setSectionResizeMode(0, QHeaderView::Fixed);
    dinosHHeader->setSectionResizeMode(4, QHeaderView::Fixed);
    dinosHHeader->setSectionResizeMode(5, QHeaderView::Fixed);
    dinosHHeader->setSectionResizeMode(1, QHeaderView::Stretch);
    dinosHHeader->setSectionResizeMode(2, QHeaderView::Stretch);
    dinosHHeader->setSectionResizeMode(3, QHeaderView::Stretch);
    dinosTable->setMinimumWidth(20);
    dinosTable->setColumnWidth(0, 20);
    dinosTable->setColumnWidth(4, 20);
    dinosTable->setColumnWidth(5, 20);
    dinosHHeader->setStretchLastSection(false);

    dinosTable->setHorizontalHeaderItem(0, new QTableWidgetItem(""));
    dinosTable->setHorizontalHeaderItem(1, new QTableWidgetItem("Blueprint"));
    dinosTable->setHorizontalHeaderItem(2, new QTableWidgetItem("Level"));
    dinosTable->setHorizontalHeaderItem(3, new QTableWidgetItem("Neutered"));
    dinosTable->setHorizontalHeaderItem(4, new QTableWidgetItem(""));
    dinosTable->setHorizontalHeaderItem(5, new QTableWidgetItem(""));

    dinosVHeader->setSectionResizeMode(QHeaderView::Fixed);
    dinosVHeader->setDefaultSectionSize(20);
    dinosVHeader->setVisible(false);

    dinosTable->setFocusPolicy(Qt::NoFocus);
    dinosTable->setSelectionMode(QAbstractItemView::NoSelection);
    dinosTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    dinosTable->setVisible(true);

    connect(itemsTable, SIGNAL(cellClicked(int, int)), this, SLOT(ItemCellClickedSlot(int, int)));
    connect(itemsTable, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(ItemCellDoubleClickedSlot(int, int)));
    connect(item_widget_, SIGNAL(SubmitCreationSignal()), this, SLOT(OnSubmitItemCreationSlot()));
    connect(item_widget_, SIGNAL(CancelCreationSignal()), this, SLOT(OnCancelItemCreationSlot()));
    connect(item_widget_, SIGNAL(SubmitEditingSignal()), this, SLOT(OnSubmitItemEditingSlot()));
    connect(item_widget_, SIGNAL(CancelEditingSignal()), this, SLOT(OnCancelItemEditingSlot()));

    connect(dinosTable, SIGNAL(cellClicked(int, int)), this, SLOT(DinoCellClickedSlot(int, int)));
    connect(dinosTable, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(DinoCellDoubleClickedSlot(int, int)));
    connect(dino_widget_, SIGNAL(SubmitCreationSignal()), this, SLOT(OnSubmitDinoCreationSlot()));
    connect(dino_widget_, SIGNAL(CancelCreationSignal()), this, SLOT(OnCancelDinoCreationSlot()));
    connect(dino_widget_, SIGNAL(SubmitEditingSignal()), this, SLOT(OnSubmitDinoEditingSlot()));
    connect(dino_widget_, SIGNAL(CancelEditingSignal()), this, SLOT(OnCancelDinoEditingSlot()));
}

EditKitWidget::~EditKitWidget()
{
    delete ui_;
    delete item_widget_;
    delete dino_widget_;
}

void EditKitWidget::Open(Shop::Kit* kit_ptr)
{
    current_kit_ = kit_ptr;

    ui_->nameEdit->setText(current_kit_->Name() ? *current_kit_->Name() : "");
    ui_->descEdit->setText(current_kit_->Description() ? *current_kit_->Description() : "");
    ui_->permsEdit->setPlainText(current_kit_->Permissions() ? *current_kit_->Permissions() : "");
    ui_->defAmountSpin->setValue(current_kit_->DefaultAmount() ? *current_kit_->DefaultAmount() : -1);
    ui_->minLevelSpin->setValue(current_kit_->MinLevel() ? *current_kit_->MinLevel() : -1);
    ui_->maxLevelSpin->setValue(current_kit_->MaxLevel() ? *current_kit_->MaxLevel() : -1);
    ui_->priceSpin->setValue(current_kit_->Price() ? *current_kit_->Price() : -1);
    ui_->spawnCombo->setCurrentIndex(current_kit_->IsOnlyFromSpawn() ? *current_kit_->IsOnlyFromSpawn() ? 1 : 2 : 0);

    DrawItemsTable();
    DrawDinosTable();

    show();
}

void EditKitWidget::Submit()
{
    const auto name = ui_->nameEdit->text();
    const auto desc = ui_->descEdit->text();
    const auto permissions = ui_->permsEdit->toPlainText();
    const auto def_amount = ui_->defAmountSpin->value();
    const auto min_level = ui_->minLevelSpin->value();
    const auto max_level = ui_->maxLevelSpin->value();
    const auto price = ui_->priceSpin->value();
    const auto spawnFlagIndex = ui_->spawnCombo->currentIndex();

    if(name.isEmpty())
    {
        ui_->errorLabel->setText("Name can't be empty");
        return;
    }

    current_kit_->Name() = name;

    if(!desc.isEmpty()) current_kit_->Description() = desc;
    else current_kit_->Description().setNull();

    if(!permissions.isEmpty()) current_kit_->Permissions() = permissions;
    else current_kit_->Permissions().setNull();

    if(def_amount != -1) current_kit_->DefaultAmount() = def_amount;
    else current_kit_->DefaultAmount().setNull();

    if(min_level != -1) current_kit_->MinLevel() = min_level;
    else current_kit_->MinLevel().setNull();

    if(max_level != -1) current_kit_->MaxLevel() = max_level;
    else current_kit_->MaxLevel().setNull();

    if(price != -1) current_kit_->Price() = price;
    else current_kit_->Price().setNull();

    if(spawnFlagIndex != 0) current_kit_->IsOnlyFromSpawn() = spawnFlagIndex == 1 ? true : false;
    else current_kit_->IsOnlyFromSpawn().setNull();

    hide();
    emit SubmitEditingSignal();
}

void EditKitWidget::closeEvent(QCloseEvent* event)
{
    QWidget::closeEvent(event);
    emit CancelEditingSignal();
}

void EditKitWidget::DrawItemsTable()
{
    ui_->itemsTable->setVisible(false);
    ui_->itemsTable->clearContents();

    auto& items = *current_kit_->Items();

    ui_->itemsTable->setRowCount(static_cast<int>(items.size()) + 1);
    ui_->itemsTable->setColumnCount(7);

    int row = 0;
    for(auto item : items)
    {
        DrawItemsRow(item, row);
        row++;
    }

    QTableWidgetItem* add_item = new QTableWidgetItem();
    add_item->setIcon(QIcon(":/icons/icons/add.png"));
    ui_->itemsTable->setItem(row, 6, add_item);

    ui_->itemsTable->setVisible(true);
}

void EditKitWidget::DrawItemsRow(Shop::Item* item, int row_num)
{
    const bool is_valid = item->IsValid();
    QTableWidget* table = ui_->itemsTable;

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

void EditKitWidget::DrawDinosTable()
{
    ui_->dinosTable->setVisible(false);
    ui_->dinosTable->clearContents();

    auto& dinos = *current_kit_->Dinos();

    ui_->dinosTable->setRowCount(static_cast<int>(dinos.size()) + 1);
    ui_->dinosTable->setColumnCount(6);

    int row = 0;
    for(auto dino : dinos)
    {
        DrawDinosRow(dino, row);
        row++;
    }

    QTableWidgetItem* add_item = new QTableWidgetItem();
    add_item->setIcon(QIcon(":/icons/icons/add.png"));
    ui_->dinosTable->setItem(row, 5, add_item);

    ui_->dinosTable->setVisible(true);
}

void EditKitWidget::DrawDinosRow(Shop::Dino* dino, int row_num)
{
    const bool is_valid = dino->IsValid();
    QTableWidget* table = ui_->dinosTable;

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
    table->setItem(row_num, 1, (dino->Blueprint()) ? new QTableWidgetItem(*dino->Blueprint()) : new QTableWidgetItem("-"));
    table->setItem(row_num, 2, (dino->Level()) ? new QTableWidgetItem(QString::number(*dino->Level())) : new QTableWidgetItem("-"));
    table->setItem(row_num, 3, (dino->Neutered()) ? (*dino->Neutered()) ? new QTableWidgetItem("True") : new QTableWidgetItem("False") : new QTableWidgetItem("-"));
    table->setItem(row_num, 4, edit_item);
    table->setItem(row_num, 5, delete_item);

    QTableWidgetItem* data_item = table->item(row_num, 0);
    data_item->setData(Qt::UserRole, QVariant::fromValue(dino));
}

void EditKitWidget::ItemCellClickedSlot(int row, int col)
{
    if(col != 5 && col != 6)
    {
        return;
    }

    if(col == 5 && row <= ui_->itemsTable->rowCount() - 2)
    {
        QTableWidgetItem* data_item = ui_->itemsTable->item(row, 0);
        current_item_ = data_item->data(Qt::UserRole).value<Shop::Item*>();
        item_widget_->Open(current_item_, false);
    }
    else if(col == 6)
    {
        int rows_count = ui_->itemsTable->rowCount();
        if(row <= rows_count - 2)
        {
            QTableWidgetItem* data_item = ui_->itemsTable->item(row, 0);
            Shop::Item* item = data_item->data(Qt::UserRole).value<Shop::Item*>();
            auto it = std::find_if((*current_kit_->Items()).begin(), (*current_kit_->Items()).end(), [item](const Shop::Item* ptr) { return item == ptr; });
            if(it != (*current_kit_->Items()).end())
            {
                (*current_kit_->Items()).erase(it);
            }
            delete item;
            DrawItemsTable();
        }
        else if(row == rows_count - 1)
        {
            current_item_ = new Shop::Item();
            item_widget_->Open(current_item_, true);
        }
    }
}

void EditKitWidget::ItemCellDoubleClickedSlot(int row, int col)
{
    if(row <= ui_->itemsTable->rowCount() - 2 && (col > 0 && col < 5))
    {
        QTableWidgetItem* data_item = ui_->itemsTable->item(row, 0);
        current_item_ = data_item->data(Qt::UserRole).value<Shop::Item*>();
        item_widget_->Open(current_item_, false);
    }
}

void EditKitWidget::DinoCellClickedSlot(int row, int col)
{
    if(col != 4 && col != 5)
    {
        return;
    }

    if(col == 4 && row <= ui_->dinosTable->rowCount() - 2)
    {
        QTableWidgetItem* data_item = ui_->dinosTable->item(row, 0);
        current_dino_ = data_item->data(Qt::UserRole).value<Shop::Dino*>();
        dino_widget_->Open(current_dino_, false);
    }
    else if(col == 5)
    {
        int rows_count = ui_->dinosTable->rowCount();
        if(row <= rows_count - 2)
        {
            QTableWidgetItem* data_item = ui_->dinosTable->item(row, 0);
            Shop::Dino* dino = data_item->data(Qt::UserRole).value<Shop::Dino*>();
            auto it = std::find_if((*current_kit_->Dinos()).begin(), (*current_kit_->Dinos()).end(), [dino](const Shop::Dino* ptr) { return dino == ptr; });
            if(it != (*current_kit_->Dinos()).end())
            {
                (*current_kit_->Dinos()).erase(it);
            }
            delete dino;
            DrawDinosTable();
        }
        else if(row == rows_count - 1)
        {
            current_dino_ = new Shop::Dino();
            dino_widget_->Open(current_dino_, true);
        }
    }
}

void EditKitWidget::DinoCellDoubleClickedSlot(int row, int col)
{
    if(row <= ui_->dinosTable->rowCount() - 2 && (col > 0 && col < 4))
    {
        QTableWidgetItem* data_item = ui_->dinosTable->item(row, 0);
        current_dino_ = data_item->data(Qt::UserRole).value<Shop::Dino*>();
        dino_widget_->Open(current_dino_, false);
    }
}

void EditKitWidget::OnSubmitItemCreationSlot()
{
    (*current_kit_->Items()).push_back(current_item_);
    current_item_ = nullptr;
    DrawItemsTable();
}

void EditKitWidget::OnCancelItemCreationSlot()
{
    delete current_item_;
    current_item_ = nullptr;
}

void EditKitWidget::OnSubmitItemEditingSlot()
{
    current_item_ = nullptr;
    DrawItemsTable();
}

void EditKitWidget::OnCancelItemEditingSlot()
{
    current_item_ = nullptr;
}

void EditKitWidget::OnSubmitDinoCreationSlot()
{
    (*current_kit_->Dinos()).push_back(current_dino_);
    current_item_ = nullptr;
    DrawDinosTable();
}

void EditKitWidget::OnCancelDinoCreationSlot()
{
    delete current_dino_;
    current_dino_ = nullptr;
}

void EditKitWidget::OnSubmitDinoEditingSlot()
{
    current_dino_ = nullptr;
    DrawDinosTable();
}

void EditKitWidget::OnCancelDinoEditingSlot()
{
    current_dino_ = nullptr;
}
