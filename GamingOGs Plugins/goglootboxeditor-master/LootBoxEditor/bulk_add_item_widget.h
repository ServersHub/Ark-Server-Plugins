#ifndef BULK_ADD_ITEM_WIDGET_H
#define BULK_ADD_ITEM_WIDGET_H

#include <QWidget>

namespace Ui {
class BulkAddItemWidget;
}

namespace LootBox
{
class Box;
}

class BulkAddItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BulkAddItemWidget(QWidget *parent = nullptr);
    ~BulkAddItemWidget() override;

    void Open(LootBox::Box* box);

private:
    bool CheckSettings();
    bool CheckTemplate();
    void CreateElements();
    void ResetUI();

    Ui::BulkAddItemWidget *ui_;
    LootBox::Box* box_;

private slots:
    void SubmitButtonClickedSlot(bool);
    void CancelButtonClickedSlot(bool);

signals:
    void SubmittedAddingSignal();
};

#endif // BULK_ADD_ITEM_WIDGET_H
