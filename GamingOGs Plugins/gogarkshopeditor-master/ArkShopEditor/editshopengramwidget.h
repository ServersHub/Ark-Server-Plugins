#ifndef EDITSHOPENGRAMWIDGET_H
#define EDITSHOPENGRAMWIDGET_H

#include <QWidget>

namespace Ui {
class EditShopEngramWidget;
}

namespace Shop
{
class ShopEngram;
}

class EditShopEngramWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EditShopEngramWidget(QWidget *parent = nullptr);
    ~EditShopEngramWidget();

    void Open(Shop::ShopEngram* node);

private:
    void closeEvent(QCloseEvent* event) override;
    void Submit();

    Ui::EditShopEngramWidget *ui_;
    Shop::ShopEngram* item_;

signals:
    void OnSubmit();
    void OnCancel();
};

#endif // EDITSHOPENGRAMWIDGET_H
