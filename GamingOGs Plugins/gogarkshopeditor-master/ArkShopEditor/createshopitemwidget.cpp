#include "createshopitemwidget.h"
#include "ui_createshopitemwidget.h"

#include "internal/node.h"
#include "internal/shopstore.h"
#include "internal/shopbeacon.h"
#include "internal/shopcommand.h"
#include "internal/shopdino.h"
#include "internal/shopitem.h"
#include "internal/shopengram.h"
#include "internal/shopexperience.h"
#include "internal/json_utils.h"

CreateShopItemWidget::CreateShopItemWidget(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui_(new Ui::CreateShopItemWidget)
{
    ui_->setupUi(this);

    connect(ui_->submitBtn, &QPushButton::clicked, [this]() {
        int index = this->ui_->comboBox->currentIndex();

        switch(index)
        {
        case 0:
            *item_ = new Shop::ShopItem("");
            break;
        case 1:
            *item_ = new Shop::ShopDino("");
            break;
        case 2:
            *item_ = new Shop::ShopEngram("");
            break;
        case 3:
            *item_ = new Shop::ShopBeacon("");
            break;
        case 4:
            *item_ = new Shop::ShopExperience("");
            break;
        case 5:
            *item_ = new Shop::ShopCommand("");
            break;
        }

        hide();
        emit OnCreatedSignal();
    });
    connect(ui_->cancelBtn, &QPushButton::clicked, [this]() {
        hide();
        emit OnCancelledSignal();
    });
}

CreateShopItemWidget::~CreateShopItemWidget()
{
    delete ui_;
}

void CreateShopItemWidget::Open(Shop::ShopNode **node)
{
    item_ = node;
    show();
}

void CreateShopItemWidget::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    emit OnCancelledSignal();
}
