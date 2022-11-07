#ifndef EDITSHOPBEACONWIDGET_H
#define EDITSHOPBEACONWIDGET_H

#include <QWidget>

namespace Ui {
class EditShopBeaconWidget;
}

namespace Shop
{
class ShopBeacon;
}

class EditShopBeaconWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EditShopBeaconWidget(QWidget *parent = nullptr);
    ~EditShopBeaconWidget();

    void Open(Shop::ShopBeacon* node);

private:
    void closeEvent(QCloseEvent* event) override;
    void Submit();

    Ui::EditShopBeaconWidget *ui_;
    Shop::ShopBeacon* item_;

signals:
    void OnSubmit();
    void OnCancel();
};

#endif // EDITSHOPBEACONWIDGET_H
