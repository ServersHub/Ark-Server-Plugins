#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>

#include "internal/json_utils.h"
#include "internal/mysql_configuration.h"

namespace Ui
{
    class MainWindow;
}

namespace LootBox
{
    class Box;
}

class BoxWidget;
class MySQLWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    void SaveFile();
    void UpdateBoxesModel();
    void UpdateTableWidget();
    void InsertTableRow(LootBox::Box* box, int row);
    void SpawnMessageBox(const QString& msg);
    void closeEvent(QCloseEvent* event) override;

    Ui::MainWindow *ui_;
    LootBox::JSON json_data_;
    QString current_filename_;
    std::vector<LootBox::Box*> boxes_;
    LootBox::Box* new_box_ptr_;
    BoxWidget* box_widget_;
    MySQLWidget* mysql_widget_;
    LootBox::MySQLConfiguration mysql_configuration_;

private slots:
    void OpenActionSlot();
    void SaveActionSlot();
    void CellClickedSlot(int row, int col);
    void CellDoubleClickedSlot(int row, int col);
    void HtmlExportAdminActionSlot();
    void HtmlExportClientActionSlot();
    void EditMySQLActionSlot();

    void SubmitBoxCreationSlot();
    void CancelBoxCreationSlot();
    void SubmitBoxEditionSlot();
    void CancelBoxEditionSlot();
};

#endif // MAINWINDOW_H
