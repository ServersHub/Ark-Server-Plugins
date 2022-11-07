#ifndef SELLSTOREWIDGET_H
#define SELLSTOREWIDGET_H

#include <QWidget>

namespace Ui {
class SellStoreWidget;
}

namespace Shop {
class SellStore;
class SellStoreItem;
}

class SellStoreItemWidget;

class SellStoreWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SellStoreWidget(QWidget *parent = nullptr);
    ~SellStoreWidget() override;

    void SetSellStore(Shop::SellStore* store);
    void closeEvent(QCloseEvent *event) override;
    void Open();

private:
    void DrawTable();
    void DrawTableRow(Shop::SellStoreItem* item, int row_num);

    Ui::SellStoreWidget *ui_;
    SellStoreItemWidget* item_widget_;
    Shop::SellStore* store_;
    Shop::SellStoreItem* current_item_;

private slots:
    void CellClickedSlot(int row, int col);
    void CellDoubleClickedSlot(int row, int col);
    void OnSubmitCreationSlot();
    void OnCancelCreationSlot();
    void OnSubmitEditingSlot();
    void OnCancelEditingSlot();

signals:
    void OnCloseSignal();
};

#endif // SELLSTOREWIDGET_H
