#ifndef ITEMWIDGET_H
#define ITEMWIDGET_H

#include <QWidget>

namespace Ui {
class ItemWidget;
}

namespace Shop
{
class Item;
}

class ItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ItemWidget(QWidget *parent = nullptr);
    ~ItemWidget() override;

    void Open(Shop::Item* item_ptr, bool is_creation_mode);

private:
    void Submit();
    void closeEvent(QCloseEvent* event) override;

    Ui::ItemWidget* ui_;
    Shop::Item* item_;
    bool creation_mode_;

signals:
    void SubmitCreationSignal();
    void CancelCreationSignal();
    void SubmitEditingSignal();
    void CancelEditingSignal();
};

#endif // ITEMWIDGET_H
