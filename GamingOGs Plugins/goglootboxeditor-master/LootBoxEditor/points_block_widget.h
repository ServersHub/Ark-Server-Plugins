#ifndef POINTS_BLOCK_WIDGET_H
#define POINTS_BLOCK_WIDGET_H

#include <QWidget>

namespace Ui {
class PointsBlockWidget;
}

namespace LootBox
{
    class PointsBlock;
}

class PointsBlockWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PointsBlockWidget(QWidget *parent = nullptr);
    ~PointsBlockWidget() override;

    void Open(LootBox::PointsBlock* node);

private:
    void Submit();
    void Cancel(bool hidden);
    void ResetUI();
    void closeEvent(QCloseEvent* event) override;

    Ui::PointsBlockWidget *ui_;
    LootBox::PointsBlock* node_;

signals:
    void SubmitSignal();
    void CancelSignal();
};

#endif // POINTS_BLOCK_WIDGET_H
