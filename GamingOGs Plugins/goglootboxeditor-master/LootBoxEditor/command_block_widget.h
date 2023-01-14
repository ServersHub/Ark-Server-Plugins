#ifndef COMMAND_BLOCK_WIDGET_H
#define COMMAND_BLOCK_WIDGET_H

#include <QWidget>

namespace Ui {
class CommandBlockWidget;
}

namespace LootBox
{
    class CommandBlock;
}

class CommandBlockWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CommandBlockWidget(QWidget *parent = nullptr);
    ~CommandBlockWidget() override;

    void Open(LootBox::CommandBlock* node);

private:
    void Submit();
    void Cancel(bool hidden);
    void ResetUI();
    void closeEvent(QCloseEvent* event) override;

    Ui::CommandBlockWidget *ui_;
    LootBox::CommandBlock* node_;

signals:
    void SubmitSignal();
    void CancelSignal();
};

#endif // COMMAND_BLOCK_WIDGET_H
