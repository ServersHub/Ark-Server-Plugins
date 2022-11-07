#ifndef BEACON_BLOCK_WIDGET_H
#define BEACON_BLOCK_WIDGET_H

#include <QWidget>

namespace Ui {
class BeaconBlockWidget;
}

namespace LootBox
{
    class BeaconBlock;
}

class BeaconBlockWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BeaconBlockWidget(QWidget *parent = nullptr);
    ~BeaconBlockWidget() override;

    void Open(LootBox::BeaconBlock* node);

private:
    void Submit();
    void Cancel(bool hidden);
    void ResetUI();
    void closeEvent(QCloseEvent* event) override;

    Ui::BeaconBlockWidget *ui_;
    LootBox::BeaconBlock* node_;

signals:
    void SubmitSignal();
    void CancelSignal();
};

#endif // BEACON_BLOCK_WIDGET_H
