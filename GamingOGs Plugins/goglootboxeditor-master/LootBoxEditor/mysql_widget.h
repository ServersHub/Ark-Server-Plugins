#ifndef MYSQL_WIDGET_H
#define MYSQL_WIDGET_H

#include <QWidget>

namespace Ui {
class MySQLWidget;
}

namespace LootBox
{
class MySQLConfiguration;
}

class MySQLWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MySQLWidget(QWidget *parent = nullptr);
    ~MySQLWidget();

    void Show(LootBox::MySQLConfiguration* data_ptr);

private:
    Ui::MySQLWidget *ui_;
    LootBox::MySQLConfiguration* data_;
};

#endif // MYSQL_WIDGET_H
