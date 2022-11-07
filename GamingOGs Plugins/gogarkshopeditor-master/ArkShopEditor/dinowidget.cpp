#include "dinowidget.h"
#include "ui_dinowidget.h"

#include "internal/dino.h"

DinoWidget::DinoWidget(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui_(new Ui::DinoWidget)
{
    ui_->setupUi(this);

    connect(ui_->submitBtn, &QPushButton::clicked, [this]() {
        this->Submit();
    });
    connect(ui_->cancelBtn, &QPushButton::clicked, [this]() {
        if(this->creation_mode_)
        {
            emit DinoWidget::CancelCreationSignal();
        }
        else
        {
            emit DinoWidget::CancelEditingSignal();
        }
        this->hide();
    });
}

DinoWidget::~DinoWidget()
{
    delete ui_;
}

void DinoWidget::Open(Shop::Dino* dino_ptr, bool is_creation_mode)
{
    dino_ = dino_ptr;
    creation_mode_ = is_creation_mode;

    ui_->blueprintEdit->setText(dino_->Blueprint() ? *dino_->Blueprint() : "");
    ui_->levelSpin->setValue(dino_->Level() ? *dino_->Level() : -1);
    ui_->neuteredCombo->setCurrentIndex(dino_->Neutered() ? dino_->Neutered() ? 1 : 2 : 0);
    ui_->errorLabel->setText("");

    show();
}

void DinoWidget::Submit()
{
    const auto bp = ui_->blueprintEdit->text();
    const auto level = ui_->levelSpin->value();
    const auto neutered_index = ui_->neuteredCombo->currentIndex();

    if(bp.isEmpty())
    {
        ui_->errorLabel->setText("Blueprint shouldn't be empty");
        return;
    }
    if(level <= 0)
    {
        ui_->errorLabel->setText("Level should be more than zero");
        return;
    }
    if(neutered_index == 0)
    {
        ui_->errorLabel->setText("Neutered flag should be True or False");
        return;
    }

    dino_->Blueprint() = bp;
    dino_->Level() = level;
    dino_->Neutered() = neutered_index == 1 ? true : false;

    if(creation_mode_)
    {
        emit SubmitCreationSignal();
    }
    else
    {
        emit SubmitEditingSignal();
    }

    hide();
}

void DinoWidget::closeEvent(QCloseEvent* event)
{
    QWidget::closeEvent(event);
    if(creation_mode_)
    {
        emit CancelCreationSignal();
    }
    else
    {
        emit CancelEditingSignal();
    }
}
