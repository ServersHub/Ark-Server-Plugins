#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>
#include <QFileDevice>
#include <QTextStream>
#include <QDir>

#include "internal/kits.h"
#include "internal/shopstore.h"
#include "internal/sellstore.h"

#include "kitswidget.h"
#include "shopstorewidget.h"
#include "sellstorewidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui_(new Ui::MainWindow)
{
    ui_->setupUi(this);

    kits_widget_ = new KitsWidget(this);
    shop_store_widget_ = new ShopStoreWidget(this);
    sell_store_widget_ = new SellStoreWidget(this);
    kits_widget_->hide();
    shop_store_widget_->hide();
    sell_store_widget_->hide();

    ui_->save_btn->setEnabled(false);
    ui_->kits_btn->setEnabled(false);
    ui_->shop_btn->setEnabled(false);
    ui_->sell_btn->setEnabled(false);

    ui_->file_label->setText("No file");
    ui_->kits_indicator->setText("Status: None");
    ui_->shop_indicator->setText("Status: None");
    ui_->sell_indicator->setText("Status: None");

    connect(ui_->open_btn, SIGNAL(clicked(bool)), this, SLOT(OnOpenButtonClickedSlot(bool)));
    connect(ui_->save_btn, SIGNAL(clicked(bool)), this, SLOT(OnSaveButtonClickedSlot(bool)));
    connect(ui_->kits_btn, SIGNAL(clicked(bool)), this, SLOT(OnKitsButtonClickedSlot(bool)));
    connect(ui_->shop_btn, SIGNAL(clicked(bool)), this, SLOT(OnShopStoreButtonClickedSlot(bool)));
    connect(ui_->sell_btn, SIGNAL(clicked(bool)), this, SLOT(OnSellStoreButtonClickedSlot(bool)));

    connect(kits_widget_, SIGNAL(OnCloseSignal()), this, SLOT(OnKitsWidgetClosedSlot()));
    connect(shop_store_widget_, SIGNAL(OnCloseSignal()), this, SLOT(OnShopStoreWidgetClosedSlot()));
    connect(sell_store_widget_, SIGNAL(OnCloseSignal()), this, SLOT(OnSellStoreWidgetClosedSlot()));
}

MainWindow::~MainWindow()
{
    delete ui_;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(!opened_file_.isEmpty())
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

bool MainWindow::OpenFile()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Select file"), QDir::currentPath(), tr("JSON (*.json)"));
    if(filename.isEmpty())
    {
        return false;
    }

    QFile f(filename);
    if(!f.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox box;
        box.setText("Can't open the file!");
        box.exec();
        return false;
    }

    QTextStream stream(&f);
    try {
        json_ = Shop::JSON::parse(stream.readAll().toStdString());
        kits_ = std::make_unique<Shop::Kits>(json_["Kits"]);
        shop_store_ = std::make_unique<Shop::ShopStore>(json_["ShopItems"]);
        sell_store_ = std::make_unique<Shop::SellStore>(json_["SellItems"]);
    } catch (const Shop::JSON::parse_error& e) {
        QMessageBox box;
        box.setText(QString("Invalid JSON syntax: %1").arg(e.what()));
        box.exec();
        return false;
    }

    opened_file_ = filename;
    return true;
}

void MainWindow::SaveFile()
{
    auto it = json_.find("Kits");
    it->clear();
    (*it) = kits_->Dump();

    it = json_.find("ShopItems");
    it->clear();
    (*it) = shop_store_->Dump();

    it = json_.find("SellItems");
    it->clear();
    (*it) = sell_store_->Dump();

    QFile f(opened_file_);
    if(!f.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox box;
        box.setText(QString("Can't open the file \"%1\" to write").arg(opened_file_));
        box.exec();
        return;
    }

    QTextStream stream(&f);
    stream << QString::fromStdString(json_.dump(1));
}

void MainWindow::UpdateKitsIndicator()
{
    if(kits_->IsValid())
    {
        ui_->kits_indicator->setText("Status: Valid");
    }
    else
    {
        ui_->kits_indicator->setText("Status: Invalid");
    }
}

void MainWindow::UpdateShopStoreIndicator()
{
    if(shop_store_->IsValid())
    {
        ui_->shop_indicator->setText("Status: Valid");
    }
    else
    {
        ui_->shop_indicator->setText("Status: Invalid");
    }
}

void MainWindow::UpdateSellStoreIndicator()
{
    if(sell_store_->IsValid())
    {
        ui_->sell_indicator->setText("Status: Valid");
    }
    else
    {
        ui_->sell_indicator->setText("Status: Invalid");
    }
}

void MainWindow::OnOpenButtonClickedSlot(bool)
{
    if(!opened_file_.isEmpty())
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

    if(OpenFile())
    {
        kits_widget_->SetKits(kits_.get());
        shop_store_widget_->SetShopStore(shop_store_.get());
        sell_store_widget_->SetSellStore(sell_store_.get());

        ui_->save_btn->setEnabled(true);
        ui_->kits_btn->setEnabled(true);
        ui_->shop_btn->setEnabled(true);
        ui_->sell_btn->setEnabled(true);

        ui_->file_label->setText("File: " + opened_file_);

        UpdateKitsIndicator();
        UpdateShopStoreIndicator();
        UpdateSellStoreIndicator();
    }
}

void MainWindow::OnSaveButtonClickedSlot(bool)
{
    SaveFile();
}

void MainWindow::OnKitsButtonClickedSlot(bool)
{
    kits_widget_->Open();
}

void MainWindow::OnShopStoreButtonClickedSlot(bool)
{
    shop_store_widget_->Open();
}

void MainWindow::OnSellStoreButtonClickedSlot(bool)
{
    sell_store_widget_->Open();
}

void MainWindow::OnKitsWidgetClosedSlot()
{
    UpdateKitsIndicator();
}

void MainWindow::OnShopStoreWidgetClosedSlot()
{
    UpdateShopStoreIndicator();
}

void MainWindow::OnSellStoreWidgetClosedSlot()
{
    UpdateSellStoreIndicator();
}
