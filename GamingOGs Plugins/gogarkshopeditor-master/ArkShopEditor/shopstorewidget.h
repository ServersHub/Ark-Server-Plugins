#ifndef SHOPSTOREWIDGET_H
#define SHOPSTOREWIDGET_H

#include <QWidget>

namespace Ui {
class ShopStoreWidget;
}

namespace Shop {
class ShopStore;
class ShopNode;
}

class CreateShopItemWidget;
class EditShopBeaconWidget;
class EditShopCommandWidget;
class EditShopDinoWidget;
class EditShopEngramWidget;
class EditShopExperienceWidget;
class EditShopItemWidget;

class ShopStoreWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ShopStoreWidget(QWidget *parent = nullptr);
    ~ShopStoreWidget();

    void SetShopStore(Shop::ShopStore* store);
    void Open();

private:
    void closeEvent(QCloseEvent* event) override;
    void DrawTable();
    void DrawTableRow(Shop::ShopNode* item, int row_num);

    Ui::ShopStoreWidget *ui_;
    Shop::ShopStore* store_;
    Shop::ShopNode* current_node_;
    CreateShopItemWidget* create_item_widget_;
    EditShopBeaconWidget* beacon_widget_;
    EditShopCommandWidget* command_widget_;
    EditShopDinoWidget* dino_widget_;
    EditShopEngramWidget* engram_widget_;
    EditShopExperienceWidget* exp_widget_;
    EditShopItemWidget* item_widget_;

private slots:
    void CellClickedSlot(int row, int col);
    void CellDoubleClickedSlot(int row, int col);
    void OnSubmitNewItemSlot();
    void OnCancelNewItemSlot();
    void OnSubmitEditItemSlot();
    void OnCancelEditItemSlot();

signals:
    void OnCloseSignal();
};

#endif // SHOPSTOREWIDGET_H
