#include <QApplication>
#include <QClipboard>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QFile f(":qdarkstyle/style.qss");

    if (!f.exists())   {
        printf("Unable to set stylesheet, file not found\n");
    }
    else   {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        app.setStyleSheet(ts.readAll());
    }

    MainWindow window;
    window.setWindowState(Qt::WindowMaximized);
    window.show();

    return app.exec();
}
