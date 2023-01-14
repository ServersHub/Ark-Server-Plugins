#ifndef CREATE_BLOCK_NODE_WIDGET_H
#define CREATE_BLOCK_NODE_WIDGET_H

#include <QWidget>

namespace Ui {
class CreateBlockNodeWidget;
}

namespace LootBox
{
    class Box;
    class BlockNode;
}

class CreateBlockNodeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CreateBlockNodeWidget(QWidget *parent = nullptr);
    ~CreateBlockNodeWidget() override;

    void closeEvent(QCloseEvent* event) override;

    void Open(LootBox::Box* parent_box, LootBox::BlockNode** node);
    void Submit();
    void Cancel();
    void ResetUI();

private:
    Ui::CreateBlockNodeWidget *ui_;
    LootBox::Box* parent_box_;
    LootBox::BlockNode** new_node_ptr_;

signals:
    void SubmitSignal();
    void CancelSignal();
};

#endif // CREATE_BLOCK_NODE_WIDGET_H
