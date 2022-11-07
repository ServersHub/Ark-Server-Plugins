#ifndef DINO_BLOCK_WIDGET_H
#define DINO_BLOCK_WIDGET_H

#include <QWidget>

namespace Ui {
class DinoBlockWidget;
}

namespace LootBox
{
    class Dino;
    class DinoBlock;
}

class DinoWidget;

class DinoBlockWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DinoBlockWidget(QWidget *parent = nullptr);
    ~DinoBlockWidget() override;

    void Open(LootBox::DinoBlock* node);

private:
    void Submit();
    void Cancel(bool hidden);
    void ResetUI();
    void closeEvent(QCloseEvent* event) override;
    void UpdateTableWidget();
    void InsertTableRow(LootBox::Dino* dino, int row);

    Ui::DinoBlockWidget *ui_;
    DinoWidget* dino_widget_;
    LootBox::DinoBlock* node_;
    LootBox::Dino* new_dino_ptr_;

private slots:
    void CellClickedSlot(int row, int col);
    void CellDoubleClickedSlot(int row, int col);
    void SubmitDinoCreationSlot();
    void CancelDinoCreationSlot();
    void SubmitDinoEditionSlot();
    void CancelDinoEditionSlot();

signals:
    void SubmitSignal();
    void CancelSignal();
};

#endif // DINO_BLOCK_WIDGET_H
