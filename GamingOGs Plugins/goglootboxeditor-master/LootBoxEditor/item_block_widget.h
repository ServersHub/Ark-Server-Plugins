#ifndef ITEM_BLOCK_WIDGET_H
#define ITEM_BLOCK_WIDGET_H

#include <QWidget>

namespace Ui {
class ItemBlockWidget;
}

namespace LootBox
{
    class Item;
    class ItemBlock;
}

class ItemWidget;

class ItemBlockWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ItemBlockWidget(QWidget *parent = nullptr);
    ~ItemBlockWidget() override;

    void Open(LootBox::ItemBlock* node);

private:
    void Submit();
    void Cancel(bool hidden);
    void ResetUI();
    void closeEvent(QCloseEvent* event) override;
    void UpdateTableWidget();
    void InsertTableRow(LootBox::Item* item, int row);

    Ui::ItemBlockWidget* ui_;
    ItemWidget* item_widget_;
    LootBox::ItemBlock* node_;
    LootBox::Item* new_item_ptr_;

private slots:
    void CellClickedSlot(int row, int col);
    void CellDoubleClickedSlot(int row, int col);
    void SubmitItemCreationSlot();
    void CancelItemCreationSlot();
    void SubmitItemEditionSlot();
    void CancelItemEditionSlot();

signals:
    void SubmitSignal();
    void CancelSignal();
};

#endif // ITEM_BLOCK_WIDGET_H
