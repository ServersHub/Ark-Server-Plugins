#ifndef SELLSTOREITEMWIDGET_H
#define SELLSTOREITEMWIDGET_H

#include <QWidget>

namespace Ui {
class SellStoreItemWidget;
}

namespace Shop
{
class SellStoreItem;
}

class SellStoreItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SellStoreItemWidget(QWidget *parent = nullptr);
    ~SellStoreItemWidget() override;

    void Open(Shop::SellStoreItem* item_ptr, bool is_creation_mode);
    void closeEvent(QCloseEvent *event) override;

private:
    void Submit();

    Ui::SellStoreItemWidget *ui_;
    Shop::SellStoreItem* item_;
    bool creation_mode_;

signals:
    void SubmitCreationSignal();
    void CancelCreationSignal();
    void SubmitEditingSignal();
    void CancelEditingSignal();
};

#endif // SELLSTOREITEMWIDGET_H
