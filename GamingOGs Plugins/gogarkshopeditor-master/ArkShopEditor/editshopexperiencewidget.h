#ifndef EDITSHOPEXPERIENCEWIDGET_H
#define EDITSHOPEXPERIENCEWIDGET_H

#include <QWidget>

namespace Ui {
class EditShopExperienceWidget;
}

namespace Shop
{
class ShopExperience;
}

class EditShopExperienceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EditShopExperienceWidget(QWidget *parent = nullptr);
    ~EditShopExperienceWidget();

    void Open(Shop::ShopExperience* node);

private:
    void closeEvent(QCloseEvent* event) override;
    void Submit();

    Ui::EditShopExperienceWidget *ui_;
    Shop::ShopExperience* item_;

signals:
    void OnSubmit();
    void OnCancel();
};

#endif // EDITSHOPEXPERIENCEWIDGET_H
