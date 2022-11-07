#ifndef EDITSHOPDINOWIDGET_H
#define EDITSHOPDINOWIDGET_H

#include <QWidget>

namespace Ui {
class EditShopDinoWidget;
}

namespace Shop
{
class ShopDino;
}

class EditShopDinoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EditShopDinoWidget(QWidget *parent = nullptr);
    ~EditShopDinoWidget();

    void Open(Shop::ShopDino* node);

private:
    void closeEvent(QCloseEvent* event) override;
    void Submit();

    Ui::EditShopDinoWidget *ui_;
    Shop::ShopDino* item_;

signals:
    void OnSubmit();
    void OnCancel();
};

#endif // EDITSHOPDINOWIDGET_H
