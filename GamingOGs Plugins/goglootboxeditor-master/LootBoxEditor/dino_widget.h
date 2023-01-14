#ifndef DINO_WIDGET_H
#define DINO_WIDGET_H

#include <QWidget>

namespace Ui {
class DinoWidget;
}

namespace LootBox
{
    class Dino;
}

class DinoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DinoWidget(QWidget *parent = nullptr);
    ~DinoWidget() override;

    void Open(LootBox::Dino* dino, bool is_creation);

private:
    void Submit();
    void Cancel(bool hidden);
    void ResetUI();
    void closeEvent(QCloseEvent* event) override;

    Ui::DinoWidget *ui_;
    LootBox::Dino* dino_;
    bool is_creation_mode_;

signals:
    void SubmitCreationSignal();
    void CancelCreationSignal();
    void SubmitEditionSignal();
    void CancelEditionSignal();
};

#endif // DINO_WIDGET_H
