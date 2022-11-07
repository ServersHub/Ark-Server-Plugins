#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "internal/json_utils.h"

namespace Ui {
class MainWindow;
}

class KitsWidget;
class ShopStoreWidget;
class SellStoreWidget;

namespace Shop {

class Kits;
class ShopStore;
class SellStore;

}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void closeEvent(QCloseEvent *event) override;

private:
    bool OpenFile();
    void SaveFile();

    void UpdateKitsIndicator();
    void UpdateShopStoreIndicator();
    void UpdateSellStoreIndicator();

    Ui::MainWindow *ui_;
    KitsWidget* kits_widget_;
    ShopStoreWidget* shop_store_widget_;
    SellStoreWidget* sell_store_widget_;
    std::unique_ptr<Shop::Kits> kits_;
    std::unique_ptr<Shop::ShopStore> shop_store_;
    std::unique_ptr<Shop::SellStore> sell_store_;
    Shop::JSON json_;
    QString opened_file_;

private slots:
    void OnOpenButtonClickedSlot(bool);
    void OnSaveButtonClickedSlot(bool);
    void OnKitsButtonClickedSlot(bool);
    void OnShopStoreButtonClickedSlot(bool);
    void OnSellStoreButtonClickedSlot(bool);

    void OnKitsWidgetClosedSlot();
    void OnShopStoreWidgetClosedSlot();
    void OnSellStoreWidgetClosedSlot();
};

#endif // MAINWINDOW_H
