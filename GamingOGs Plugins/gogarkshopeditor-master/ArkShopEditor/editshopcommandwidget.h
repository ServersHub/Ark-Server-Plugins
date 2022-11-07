#ifndef EDITSHOPCOMMANDWIDGET_H
#define EDITSHOPCOMMANDWIDGET_H

#include <QWidget>

namespace Ui {
class EditShopCommandWidget;
}

namespace Shop
{
class ShopCommand;
}

class EditShopCommandWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EditShopCommandWidget(QWidget *parent = nullptr);
    ~EditShopCommandWidget();

    void Open(Shop::ShopCommand* node);

private:
    void closeEvent(QCloseEvent* event) override;
    void Submit();

    Ui::EditShopCommandWidget *ui_;
    Shop::ShopCommand* item_;

signals:
    void OnSubmit();
    void OnCancel();
};

#endif // EDITSHOPCOMMANDWIDGET_H
