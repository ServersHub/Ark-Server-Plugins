#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QString>
#include <QFileDialog>
#include <QFileDevice>
#include <QDir>
#include <QMessageBox>

#include "box_widget.h"
#include "mysql_widget.h"
#include "internal/box.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui_(new Ui::MainWindow)
{
    ui_->setupUi(this);
    box_widget_ = new BoxWidget(this);
    box_widget_->hide();
    mysql_widget_ = new MySQLWidget(this);
    mysql_widget_->hide();

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
    table->setColumnWidth(9, 20);
    table->setColumnWidth(10, 20);
    hHeader->setStretchLastSection(false);

    table->setHorizontalHeaderItem(0, new QTableWidgetItem(""));
    table->setHorizontalHeaderItem(1, new QTableWidgetItem("Box"));
    table->setHorizontalHeaderItem(2, new QTableWidgetItem("Gamble percent"));
    table->setHorizontalHeaderItem(3, new QTableWidgetItem("Gamble price"));
    table->setHorizontalHeaderItem(4, new QTableWidgetItem("Permission"));
    table->setHorizontalHeaderItem(5, new QTableWidgetItem("Min amount"));
    table->setHorizontalHeaderItem(6, new QTableWidgetItem("Max amount"));
    table->setHorizontalHeaderItem(7, new QTableWidgetItem("Items count"));
    table->setHorizontalHeaderItem(8, new QTableWidgetItem(""));
    table->setHorizontalHeaderItem(9, new QTableWidgetItem(""));


    vHeader->setSectionResizeMode(QHeaderView::Fixed);
    vHeader->setDefaultSectionSize(20);
    vHeader->setVisible(false);

    table->setFocusPolicy(Qt::NoFocus);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    table->setVisible(true);

    ui_->saveAction->setEnabled(false);
    ui_->exportAdminAction->setEnabled(false);
    ui_->exportClientAction->setEnabled(false);
    ui_->editMySQLAction->setEnabled(false);

    connect(ui_->openAction, SIGNAL(triggered()), this, SLOT(OpenActionSlot()));
    connect(ui_->saveAction, SIGNAL(triggered()), this, SLOT(SaveActionSlot()));
    connect(ui_->editMySQLAction, SIGNAL(triggered()), this, SLOT(EditMySQLActionSlot()));
    connect(ui_->exportAdminAction, SIGNAL(triggered()), this, SLOT(HtmlExportAdminActionSlot()));
    connect(ui_->exportClientAction, SIGNAL(triggered()), this, SLOT(HtmlExportClientActionSlot()));
    connect(ui_->tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(CellClickedSlot(int, int)));
    connect(ui_->tableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(CellDoubleClickedSlot(int, int)));

    connect(box_widget_, SIGNAL(SubmitCreationSignal()), this, SLOT(SubmitBoxCreationSlot()));
    connect(box_widget_, SIGNAL(CancelCreationSignal()), this, SLOT(CancelBoxCreationSlot()));
    connect(box_widget_, SIGNAL(SubmitEditionSignal()), this, SLOT(SubmitBoxEditionSlot()));
    connect(box_widget_, SIGNAL(CancelEditionSignal()), this, SLOT(CancelBoxEditionSlot()));
}

MainWindow::~MainWindow()
{
    delete ui_;
}

void MainWindow::SaveFile()
{
    auto it = json_data_.find("Boxes");
    it->clear();
    for(const LootBox::Box* box : boxes_)
    {
        (*it)[box->GetBoxName().toStdString()] = box->ToJSON();
    }

    json_data_["General"]["Mysql"] = mysql_configuration_.Dump();

    QFile f(current_filename_);
    if(!f.open(QFile::WriteOnly | QFile::Text))
    {
        SpawnMessageBox(QString("Can't open the file \"%1\" to write").arg(current_filename_));
        return;
    }

    QTextStream stream(&f);
    stream << QString::fromStdString(json_data_.dump(1));
}

void MainWindow::UpdateBoxesModel()
{
    for(LootBox::Box* box : boxes_)
    {
        box->~Box();
    }
    boxes_.clear();

    if(const auto boxes = json_data_.find("Boxes"); boxes != json_data_.end())
    {
        for(const auto& box : boxes->items())
        {
            LootBox::Box* box_ptr = new LootBox::Box(QString::fromStdString(box.key()), box.value());
            boxes_.push_back(box_ptr);
        }
    }
}

void MainWindow::UpdateTableWidget()
{
    QTableWidget* table = ui_->tableWidget;

    table->clearContents();
    table->setRowCount(static_cast<int>(boxes_.size()) + 1);
    table->setColumnCount(10);

    int row = 0;
    for(LootBox::Box* box : boxes_)
    {
        InsertTableRow(box, row);
        row++;
    }

    QTableWidgetItem* add_item = new QTableWidgetItem();
    add_item->setIcon(QIcon(":/icons/icons/add.png"));
    table->setItem(row, 9, add_item);
}

void MainWindow::InsertTableRow(LootBox::Box *box, int row)
{
    const bool is_valid = box->CheckValidity();
    QTableWidget* table = ui_->tableWidget;

    QTableWidgetItem* status_item = new QTableWidgetItem();
    status_item->setIcon(is_valid ? QIcon(":/icons/icons/valid.png") : QIcon(":/icons/icons/invalid.png"));
    QTableWidgetItem* edit_item = new QTableWidgetItem();
    edit_item->setIcon(QIcon(":/icons/icons/edit.png"));
    QTableWidgetItem* delete_item = new QTableWidgetItem();
    delete_item->setIcon(QIcon(":/icons/icons/delete.png"));

    table->setItem(row, 0, status_item);
    table->setItem(row, 1, new QTableWidgetItem(box->GetBoxName()));
    table->setItem(row, 2, new QTableWidgetItem(QString::number(box->GetGamblePercent())));
    table->setItem(row, 3, new QTableWidgetItem(QString::number(box->GetGamblePrice())));
    table->setItem(row, 4, new QTableWidgetItem(box->GetPermission()));
    table->setItem(row, 5, new QTableWidgetItem(QString::number(box->GetMinAmount())));
    table->setItem(row, 6, new QTableWidgetItem(QString::number(box->GetMaxAmount())));
    table->setItem(row, 7, new QTableWidgetItem(QString::number(box->GetNodes().size())));
    table->setItem(row, 8, edit_item);
    table->setItem(row, 9, delete_item);

    QTableWidgetItem* data_item = table->item(row, 0);
    data_item->setData(Qt::UserRole, QVariant::fromValue(box));
}

void MainWindow::SpawnMessageBox(const QString& msg)
{
    QMessageBox box;
    box.setText(msg);
    box.exec();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(!current_filename_.isEmpty())
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Close file", "Do you want to save current file?",
                                      QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        if(reply == QMessageBox::Cancel)
        {
            event->ignore();
        }
        if(reply == QMessageBox::Yes)
        {
            SaveFile();
            event->accept();
        }
        if(reply == QMessageBox::No)
        {
            event->accept();
        }
    }
    else
    {
        event->accept();
    }
}

void MainWindow::OpenActionSlot()
{
    if(!current_filename_.isEmpty())
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Close file", "Do you want to save current file?",
                                      QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        if(reply == QMessageBox::Cancel)
        {
            return;
        }
        if(reply == QMessageBox::Yes)
        {
            SaveFile();
        }
    }

    QString filename = QFileDialog::getOpenFileName(this, tr("Select file"), QDir::currentPath(), tr("JSON (*.json)"));
    if(filename.isEmpty())
    {
        return;
    }

    QFile f(filename);
    if(!f.open(QFile::ReadOnly | QFile::Text))
    {
        SpawnMessageBox("Can't open the file!");
        return;
    }

    QTextStream stream(&f);
    try {
        json_data_ = LootBox::JSON::parse(stream.readAll().toStdString());
    } catch (const LootBox::JSON::parse_error& e) {
        SpawnMessageBox(QString("Invalid JSON syntax: %1").arg(e.what()));
        return;
    }

    UpdateBoxesModel();
    UpdateTableWidget();

    if(const auto it = json_data_["General"].find("Mysql"); it != json_data_["General"].end())
    {
        mysql_configuration_.Load(*it);
    }
    else
    {
        mysql_configuration_.InitializeEmpty();
    }

    if(current_filename_.isEmpty())
    {
        ui_->saveAction->setEnabled(true);
        ui_->editMySQLAction->setEnabled(true);
    }

    if(!boxes_.empty())
    {
        ui_->exportAdminAction->setEnabled(true);
        ui_->exportClientAction->setEnabled(true);
    }

    current_filename_ = filename;
}

void MainWindow::SaveActionSlot()
{
    SaveFile();
}

void MainWindow::CellClickedSlot(int row, int col)
{
    if(col != 8 && col != 9)
    {
        return;
    }

    if(col == 8 && row <= ui_->tableWidget->rowCount() - 2)
    {
        QTableWidgetItem* data_item = ui_->tableWidget->item(row, 0);
        LootBox::Box* box = data_item->data(Qt::UserRole).value<LootBox::Box*>();
        box_widget_->Open(box, false);
    }
    else if(col == 9)
    {
        int rows_count = ui_->tableWidget->rowCount();
        if(row <= rows_count - 2)
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Delete box", "Are you sure you want to delete the box?",
                                          QMessageBox::Yes|QMessageBox::No);
            if(reply == QMessageBox::Yes)
            {
                QTableWidgetItem* item = ui_->tableWidget->item(row, 0);
                LootBox::Box* box = item->data(Qt::UserRole).value<LootBox::Box*>();
                const auto it = std::find_if(boxes_.cbegin(), boxes_.cend(),
                                             [box](const LootBox::Box* ptr) { return box == ptr; });
                if(it != boxes_.cend())
                {
                    boxes_.erase(it);
                    delete box;
                }
                UpdateTableWidget();
            }
        }
        else if(row == rows_count - 1)
        {
            new_box_ptr_ = new LootBox::Box("");
            box_widget_->Open(new_box_ptr_, true);
        }
    }
}

void MainWindow::CellDoubleClickedSlot(int row, int col)
{
    if(row <= ui_->tableWidget->rowCount() - 2 && (col > 0 && col < 8))
    {
        QTableWidgetItem* data_item = ui_->tableWidget->item(row, 0);
        LootBox::Box* box = data_item->data(Qt::UserRole).value<LootBox::Box*>();
        box_widget_->Open(box, false);
    }
}

void MainWindow::HtmlExportAdminActionSlot()
{
    qDebug() << "Admin export";
}

void MainWindow::HtmlExportClientActionSlot()
{
    qDebug() << "Client export";
}

void MainWindow::EditMySQLActionSlot()
{
    mysql_widget_->Show(&mysql_configuration_);
}

void MainWindow::SubmitBoxCreationSlot()
{
    boxes_.push_back(new_box_ptr_);
    UpdateTableWidget();
    box_widget_->Open(new_box_ptr_, false);
}

void MainWindow::CancelBoxCreationSlot()
{
    delete new_box_ptr_;
    new_box_ptr_ = nullptr;
}

void MainWindow::SubmitBoxEditionSlot()
{
    UpdateTableWidget();
}

void MainWindow::CancelBoxEditionSlot()
{
}
