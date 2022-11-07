#ifndef BULK_ADD_DINO_WIDGET_H
#define BULK_ADD_DINO_WIDGET_H

#include <QWidget>

namespace Ui {
class BulkAddDinoWidget;
}

namespace LootBox
{
class Box;
}

class BulkAddDinoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BulkAddDinoWidget(QWidget *parent = nullptr);
    ~BulkAddDinoWidget() override;

    void Open(LootBox::Box* box);

private:
    bool CheckSettings();
    bool CheckTemplate();
    void CreateElements();
    void ResetUI();

    Ui::BulkAddDinoWidget* ui_;
    LootBox::Box* box_;

private slots:
    void SubmitButtonClickedSlot(bool);
    void CancelButtonClickedSlot(bool);

signals:
    void SubmittedAddingSignal();
};

#endif // BULK_ADD_DINO_WIDGET_H
