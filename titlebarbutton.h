#ifndef TITLEBARBUTTON_H
#define TITLEBARBUTTON_H

#include <QAbstractButton>
#include <QColor>

enum TitleBarButtonState
{
    kNormal = 0,
    kHover,
    kPressed
};

class TitleBarButton : public QAbstractButton
{
    Q_OBJECT
public:
    TitleBarButton(QWidget *parent = nullptr);
    virtual ~TitleBarButton() = default;
private:
    TitleBarButtonState m_state;
    // Icon color
    QColor m_normalColor;
    QColor m_hoverColor;
    QColor m_pressColor;
    // background color
    QColor m_normalBgColor;
    QColor m_hoverBgColor;
    QColor m_pressdBgColor;
};

#endif // TITLEBARBUTTON_H
