#ifndef TITLEBARBUTTON_H
#define TITLEBARBUTTON_H

#include <QAbstractButton>
#include <QColor>
#include <QDomDocument>
#include <QString>

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

    bool isPressed() const;

    void setNormalColor(const QColor &color);
    QColor getNormalColor() const;

    void setHoverColor(const QColor &color);
    QColor getHoverColor() const;

    void setPressedColor(const QColor &color);
    QColor getPressedColor() const;

    void setNormalBgColor(const QColor &color);
    QColor getNormalBgColor() const;

    void setHoverBgColor(const QColor &color);
    QColor getHoverBgColor() const;

    void setPressedBgColor(const QColor &color);
    QColor getPressedBgColor() const;

    void getCurColors(QColor &color, QColor &bgColor) const;

protected:
    virtual void enterEvent(QEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;

    void setState(TitleBarButtonState state);

private:
    TitleBarButtonState m_state;
    // Icon color
    QColor m_normalColor;
    QColor m_hoverColor;
    QColor m_pressedColor;
    // background color
    QColor m_normalBgColor;
    QColor m_hoverBgColor;
    QColor m_pressedBgColor;
};

class SvgTitleBarButton : public TitleBarButton
{
public:
    SvgTitleBarButton(const QString &iconPath, QWidget *parent = nullptr);
    virtual ~SvgTitleBarButton() = default;

    void setIcon(const QString &iconPath);

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    QDomDocument m_svgDom;
};

class MinimizeButton : public TitleBarButton
{
public:
    MinimizeButton(QWidget *parent = nullptr);
    virtual ~MinimizeButton() = default;

protected:
    virtual void paintEvent(QPaintEvent *event) override;
};

class MaximizeButton : public TitleBarButton
{
public:
    MaximizeButton(QWidget *parent = nullptr);
    virtual ~MaximizeButton() = default;

    void setMaxState(bool isMax);

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    bool m_isMax;
};

class CloseButton : public SvgTitleBarButton
{
public:
    CloseButton(const QString &iconPath, QWidget *parent = nullptr);
    virtual ~CloseButton() = default;
};

#endif  // TITLEBARBUTTON_H
