#ifndef KITSWIDGET_H
#define KITSWIDGET_H

#include <QWidget>

namespace Ui {
class KitsWidget;
}

namespace Shop {
class Kits;
class Kit;
}

class AddKitWidget;
class EditKitWidget;

class KitsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KitsWidget(QWidget *parent = nullptr);
    ~KitsWidget() override;

    void SetKits(Shop::Kits* kits);
    void Open();

private:
    void closeEvent(QCloseEvent* event) override;
    void DrawTable();
    void DrawTableRow(Shop::Kit* kit, int row_num);

    Ui::KitsWidget *ui_;
    Shop::Kits* kits_;
    Shop::Kit* current_kit_;
    AddKitWidget* add_kit_widget_;
    EditKitWidget* edit_kit_widget_;

private slots:
    void CellClickedSlot(int row, int col);
    void CellDoubleClickedSlot(int row, int col);
    void OnSubmitCreationSlot();
    void OnCancelCreationSlot();
    void OnSubmitEditingSlot();
    void OnCancelEditingSlot();

signals:
    void OnCloseSignal();
};

#endif // KITSWIDGET_H
