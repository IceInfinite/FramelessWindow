#ifndef FRAMELESSWIDGET_H
#define FRAMELESSWIDGET_H

#include <QWidget>
#include <QScreen>

#include "titlebar.h"

namespace Ui {
class FramelessWidget;
}

class FramelessWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FramelessWidget(QWidget *parent = nullptr);
    ~FramelessWidget();

    void setTitleBar(TitleBar *titleBar);
    void setResizeEnabled(bool enable);

protected:
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual bool nativeEvent(const QByteArray &eventType, void *message, 
                             long *result) override;
private slots:
    void onScreenChanged(QScreen *screen);

private:
    Ui::FramelessWidget *m_ui;
    TitleBar *m_titleBar;
    bool m_isResizeEnable;
};

#endif // FRAMELESSWIDGET_H
