#ifndef ADDKITWIDGET_H
#define ADDKITWIDGET_H

#include <QWidget>

namespace Ui {
class AddKitWidget;
}

namespace Shop
{
    class Kit;
}

class AddKitWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AddKitWidget(QWidget *parent = nullptr);
    ~AddKitWidget() override;

    void Open(Shop::Kit* kit_ptr);

private:
    void Submit();
    void closeEvent(QCloseEvent* event) override;

    Ui::AddKitWidget *ui_;
    Shop::Kit* current_kit_;

signals:
    void SubmitCreationSignal();
    void CancelCreationSignal();
};

#endif // ADDKITWIDGET_H
