#ifndef ITEM_WIDGET_H
#define ITEM_WIDGET_H

#include <QWidget>

namespace Ui {
class ItemWidget;
}

namespace LootBox
{
    class Item;
}

class ItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ItemWidget(QWidget *parent = nullptr);
    ~ItemWidget() override;

    void Open(LootBox::Item* item, bool is_creation);

private:
    void Submit();
    void Cancel(bool hidden);
    void ResetUI();
    void closeEvent(QCloseEvent* event) override;

    Ui::ItemWidget *ui_;
    LootBox::Item* item_;
    bool is_creation_mode_;

signals:
    void SubmitCreationSignal();
    void CancelCreationSignal();
    void SubmitEditionSignal();
    void CancelEditionSignal();

};

#endif // ITEM_WIDGET_H
