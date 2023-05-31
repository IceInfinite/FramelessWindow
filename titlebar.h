#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QWidget>
#include <QLabel>

#include "titlebarbutton.h"

class TitleBar : public QWidget
{
    Q_OBJECT
public:
    explicit TitleBar(QWidget *parent = 0);
    virtual ~TitleBar() = default;

    void setDoubleClickEnabled(bool enable);

public slots:
    void setTitle(const QString &title);
    void setIcon(const QIcon &icon);

protected:
    virtual bool eventFilter(QObject *obj, QEvent *e) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    void toggleMaxState();

private:
    bool isDragRegion(const QPoint &pos);
    bool hasButtonPressed();
    bool canDrag(const QPoint &pos);

private:
    MinimizeButton m_minBtn;
    MaximizeButton m_maxBtn;
    CloseButton m_closeBtn;
    bool m_isDoubleClickedEnabled;
    QLabel *m_iconLabel;
    QLabel *m_titleLabel;
};

#endif // TITLEBAR_H
