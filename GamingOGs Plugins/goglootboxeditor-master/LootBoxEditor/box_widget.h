#ifndef EDIT_BOX_WIDGET_H
#define EDIT_BOX_WIDGET_H

#include <QWidget>
#include <QKeyEvent>
#include <QTableWidget>

#include "internal/box.h"
#include "internal/node.h"

namespace Ui {
class BoxWidget;
}

class ItemBlockWidget;
class DinoBlockWidget;
class BeaconBlockWidget;
class CommandBlockWidget;
class PointsBlockWidget;
class BulkAddItemWidget;
class BulkAddDinoWidget;

class CreateBlockNodeWidget;

class TableEventFilter : public QObject
{
    Q_OBJECT

public:
    TableEventFilter(QTableWidget* t)
        : table_(t)
    {
    }

protected:
    bool eventFilter(QObject* obj, QEvent* e) override
    {
        auto result = QObject::eventFilter(obj, e);

        if(e->type() == QEvent::KeyPress)
        {
            QKeyEvent* key_event = static_cast<QKeyEvent*>(e);
            if(key_event->key() == Qt::Key_Return)
            {
                const int next_row = table_->currentRow() + 1;
                if(next_row + 1 <= table_->rowCount())
                {
                    auto item = table_->item(next_row, table_->currentColumn());
                    if(item)
                    {
                        table_->setCurrentItem(item, QItemSelectionModel::NoUpdate);
                        table_->edit(table_->currentIndex());
                    }
                }
            }
        }

        return result;
    }

private:
    QTableWidget* table_;
};

class BoxWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BoxWidget(QWidget *parent = nullptr);
    ~BoxWidget() override;

    void Open(LootBox::Box* box, bool is_creation);

private:
    void SetupForCreate();
    void SetupForEdit();
    void ResetUI();
    void UpdateTableWidget();
    void InsertTableNode(LootBox::BlockNode* node, int row);
    void closeEvent(QCloseEvent* event) override;
    void Submit();
    void Cancel();
    void EditNode(LootBox::BlockNode* node);

    Ui::BoxWidget *ui_;
    CreateBlockNodeWidget* create_block_widget_;
    ItemBlockWidget* item_block_widget_;
    DinoBlockWidget* dino_block_widget_;
    BeaconBlockWidget* beacon_block_widget_;
    CommandBlockWidget* command_block_widget_;
    PointsBlockWidget* points_block_widget_;
    BulkAddItemWidget* bulk_add_item_widget_;
    BulkAddDinoWidget* bulk_add_dino_widget_;
    LootBox::Box* box_;
    LootBox::BlockNode* new_node_ptr_;
    bool is_creation_mode_;

private slots:
    void CellClickedSlot(int row, int col);
    void CellChangedSlot(int row, int col);
    void CellDoubleClickedSlot(int row, int col);
    void SubmitNodeCreationSlot();
    void CancelNodeCreationSlot();
    void SubmitNodeEditionSlot();
    void CancelNodeEditionSlot();
    void BulkAddedSlot();

signals:
    void SubmitCreationSignal();
    void CancelCreationSignal();
    void SubmitEditionSignal();
    void CancelEditionSignal();
};

#endif // EDIT_BOX_WIDGET_H
