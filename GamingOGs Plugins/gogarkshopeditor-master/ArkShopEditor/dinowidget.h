#ifndef DINOWIDGET_H
#define DINOWIDGET_H

#include <QWidget>

namespace Ui {
class DinoWidget;
}

namespace Shop
{
class Dino;
}

class DinoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DinoWidget(QWidget *parent = nullptr);
    ~DinoWidget() override;

    void Open(Shop::Dino* dino_ptr, bool is_creation_mode);

private:
    void Submit();
    void closeEvent(QCloseEvent* event) override;

    Ui::DinoWidget* ui_;
    Shop::Dino* dino_;
    bool creation_mode_;

signals:
    void SubmitCreationSignal();
    void CancelCreationSignal();
    void SubmitEditingSignal();
    void CancelEditingSignal();
};

#endif // DINOWIDGET_H
