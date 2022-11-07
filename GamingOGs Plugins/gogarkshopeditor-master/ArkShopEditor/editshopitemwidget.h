#ifndef EDITSHOPITEMWIDGET_H
#define EDITSHOPITEMWIDGET_H

#include <QWidget>

namespace Ui {
class EditShopItemWidget;
}

namespace Shop
{
class ShopItem;
class Item;
}

class ItemWidget;

class EditShopItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EditShopItemWidget(QWidget *parent = nullptr);
    ~EditShopItemWidget();

    void Open(Shop::ShopItem* node);

private:
    void DrawTable();
    void DrawItemsRow(Shop::Item* item, int row_num);
    void closeEvent(QCloseEvent* event) override;
    void Submit();

    Ui::EditShopItemWidget *ui_;
    Shop::ShopItem* item_;
    Shop::Item* current_item_;
    ItemWidget* widget_;

private slots:
    void CellClickedSlot(int row, int col);
    void CellDoubleClickedSlot(int row, int col);
    void OnSubmitItemCreationSlot();
    void OnCancelItemCreationSlot();
    void OnSubmitItemEditingSlot();
    void OnCancelItemEditingSlot();

signals:
    void OnSubmit();
    void OnCancel();
};

#endif // EDITSHOPITEMWIDGET_H
