#ifndef CREATESHOPITEMWIDGET_H
#define CREATESHOPITEMWIDGET_H

#include <QWidget>

namespace Ui {
class CreateShopItemWidget;
}

namespace Shop
{
class ShopNode;
}

class CreateShopItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CreateShopItemWidget(QWidget *parent = nullptr);
    ~CreateShopItemWidget();

    void Open(Shop::ShopNode** node);

private:
    void closeEvent(QCloseEvent* event) override;

    Ui::CreateShopItemWidget *ui_;
    Shop::ShopNode** item_;

signals:
    void OnCreatedSignal();
    void OnCancelledSignal();
};

#endif // CREATESHOPITEMWIDGET_H
