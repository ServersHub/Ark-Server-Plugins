#ifndef EDITKITWIDGET_H
#define EDITKITWIDGET_H

#include <QWidget>

namespace Ui {
class EditKitWidget;
}

namespace Shop
{
    class Kit;
    class Item;
    class Dino;
}

class ItemWidget;
class DinoWidget;

class EditKitWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EditKitWidget(QWidget *parent = nullptr);
    ~EditKitWidget() override;

    void Open(Shop::Kit* kit_ptr);

private:
    void Submit();
    void closeEvent(QCloseEvent* event) override;
    void DrawItemsTable();
    void DrawItemsRow(Shop::Item* item, int row_num);
    void DrawDinosTable();
    void DrawDinosRow(Shop::Dino* dino, int row_num);

    Ui::EditKitWidget* ui_;
    ItemWidget* item_widget_;
    DinoWidget* dino_widget_;
    Shop::Kit* current_kit_;
    Shop::Item* current_item_;
    Shop::Dino* current_dino_;

signals:
    void SubmitEditingSignal();
    void CancelEditingSignal();

private slots:
    void ItemCellClickedSlot(int row, int col);
    void ItemCellDoubleClickedSlot(int row, int col);
    void DinoCellClickedSlot(int row, int col);
    void DinoCellDoubleClickedSlot(int row, int col);
    void OnSubmitItemCreationSlot();
    void OnCancelItemCreationSlot();
    void OnSubmitItemEditingSlot();
    void OnCancelItemEditingSlot();
    void OnSubmitDinoCreationSlot();
    void OnCancelDinoCreationSlot();
    void OnSubmitDinoEditingSlot();
    void OnCancelDinoEditingSlot();
};

#endif // EDITKITWIDGET_H
