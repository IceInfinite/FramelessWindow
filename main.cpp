#include <QApplication>
#include <QIcon>

#include "framelesswidget.h"

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication a(argc, argv);
    FramelessWidget w;
    w.setWindowTitle("Frameless Window");
    w.setWindowIcon(QIcon("logo/logo.png"));
    // w.setStyleSheet("background:white");
    w.show();
    return a.exec();
}
