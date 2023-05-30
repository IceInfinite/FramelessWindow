#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;

private:
    void enableAnimation(bool enable);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
