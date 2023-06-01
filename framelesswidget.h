#ifndef FRAMELESSWIDGET_H
#define FRAMELESSWIDGET_H

#include <QScreen>
#include <QWidget>

#include "titlebar.h"

class FramelessWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FramelessWidget(QWidget *parent = nullptr);
    virtual ~FramelessWidget();
    void setTitleBar(TitleBar *titleBar);
    void setResizeEnabled(bool enable);

protected:
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual bool nativeEvent(
        const QByteArray &eventType, void *message, long *result) override;

private slots:
    void onScreenChanged(QScreen *screen);

protected:
    TitleBar *m_titleBar;
    bool m_isResizeEnable;
};

#endif  // FRAMELESSWIDGET_H
