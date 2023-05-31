#include "titlebarbutton.h"

#include <QFile>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QSvgRenderer>

TitleBarButton::TitleBarButton(QWidget *parent) : QAbstractButton(parent)
{
    setCursor(Qt::ArrowCursor);
    setFixedSize(46, 32);

    m_state = TitleBarButtonState::kNormal;
    m_normalColor = QColor(0, 0, 0);
    m_hoverColor = QColor(0, 0, 0);
    m_pressedColor = QColor(0, 0, 0);
    m_normalBgColor = QColor(0, 0, 0, 0);
    m_hoverBgColor = QColor(0, 0, 0, 26);
    m_pressedBgColor = QColor(0, 0, 0, 51);
}

void TitleBarButton::setState(TitleBarButtonState state)
{
    if (m_state == state)
        return;
    
    m_state = state;
    update();
}

bool TitleBarButton::isPressed() const
{
    return m_state == TitleBarButtonState::kPressed;
}

void TitleBarButton::setNormalColor(const QColor &color)
{
    if (m_normalColor == color)
        return;
    
    m_normalColor = color;
    update();
}

QColor TitleBarButton::getNormalColor() const
{
    return m_normalColor;
}

void TitleBarButton::setHoverColor(const QColor &color)
{
    if (m_hoverColor == color)
        return;
    
    m_hoverColor = color;
    update();
}

QColor TitleBarButton::getHoverColor() const
{
    return m_hoverColor;
}

void TitleBarButton::setPressedColor(const QColor &color)
{
    if (m_pressedColor == color)
        return;
    
    m_pressedColor = color;
    update();
}

QColor TitleBarButton::getPressedColor() const
{
    return m_pressedColor;
}

void TitleBarButton::setNormalBgColor(const QColor &color)
{
    if (m_normalBgColor == color)
        return;
    
    m_normalBgColor = color;
    update();
}

QColor TitleBarButton::getNormalBgColor() const
{
    return m_normalBgColor;
}

void TitleBarButton::setHoverBgColor(const QColor &color)
{
    if (m_hoverBgColor == color)
        return;
    
    m_hoverBgColor = color;
    update();
}

QColor TitleBarButton::getHoverBgColor() const
{
    return m_hoverBgColor;
}

void TitleBarButton::setPressedBgColor(const QColor &color)
{
    if (m_pressedBgColor == color)
        return;
    
    m_pressedBgColor = color;
    update();
}

QColor TitleBarButton::getPressedBgColor() const
{
    return m_pressedBgColor;
}

void TitleBarButton::getCurColors(QColor &color, QColor bgColor) const
{
    switch (m_state)
    {
    case TitleBarButtonState::kNormal:
        color = m_normalColor;
        bgColor = m_normalBgColor;
        break;
    case TitleBarButtonState::kHover:
        color = m_hoverColor;
        bgColor = m_hoverBgColor;
        break;
    case TitleBarButtonState::kPressed:
        color = m_pressedColor;
        bgColor = m_pressedBgColor;
        break;
    }
}

void TitleBarButton::enterEvent(QEvent *event)
{
    setState(TitleBarButtonState::kHover);
    QAbstractButton::enterEvent(event);
}

void TitleBarButton::leaveEvent(QEvent *event)
{
    setState(TitleBarButtonState::kNormal);
    QAbstractButton::leaveEvent(event);
}

void TitleBarButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    setState(TitleBarButtonState::kPressed);
    QAbstractButton::mousePressEvent(event);
}

SvgTitleBarButton::SvgTitleBarButton(const QString &iconPath, QWidget *parent)
    : TitleBarButton(parent)
{
    setIcon(iconPath);
}

void SvgTitleBarButton::setIcon(const QString &iconPath)
{
    QFile f(iconPath);
    f.open(QFile::ReadOnly);
    m_svgDom.setContent(f.readAll());
    f.close();
}

void SvgTitleBarButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | 
                           QPainter::SmoothPixmapTransform);
    QColor color, bgColor;
    getCurColors(color, bgColor);

    // draw background
    painter.setBrush(bgColor);
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());

    // draw icon
    QString colorName = color.name();
    QDomNodeList pathNodes = m_svgDom.elementsByTagName("path");
    for (int i = 0; i < pathNodes.count(); ++i)
    {
        QDomElement element = pathNodes.at(i).toElement();
        element.setAttribute("stroke", colorName);
    }

    QSvgRenderer render(m_svgDom.toByteArray());
    render.render(&painter, QRectF(rect()));
}

MinimizeButton::MinimizeButton(QWidget *parent) : TitleBarButton(parent)
{

}

void MinimizeButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QColor color, bgColor;
    getCurColors(color, bgColor);

    // draw background
    painter.setBrush(bgColor);
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());

    // draw icon
    painter.setBrush(Qt::NoBrush);
    QPen pen(color, 1);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.drawLine(18, 16, 28, 16);
}

MaximizeButton::MaximizeButton(QWidget *parent)
    : TitleBarButton(parent), m_isMax(false)
{
}

void MaximizeButton::setMaxState(bool isMax)
{
    if (m_isMax == isMax)
        return;
    
    m_isMax = isMax;
    setState(TitleBarButtonState::kNormal);
}

void MaximizeButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QColor color, bgColor;
    getCurColors(color, bgColor);

    // draw background
    painter.setBrush(bgColor);
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());

    // draw icon
    painter.setBrush(Qt::NoBrush);
    QPen pen(color, 1);
    pen.setCosmetic(true);
    painter.setPen(pen);
    
    qreal r = devicePixelRatioF();
    painter.scale(1 / r, 1 / r);
    if (m_isMax)
    {
        painter.drawRect(18 * r, 11 * r, 10 * r, 10 * r);
    } 
    else
    {
        painter.drawRect(18 * r, 13 * r, 8 * r, 8 * r);
        int x0 = static_cast<int>(18 * r) + static_cast<int>(2 * r);
        int y0 = 13 * r;
        int dw = 2 * r;
        QPainterPath path = QPainterPath(QPointF(x0, y0));
        path.lineTo(x0, y0 - dw);
        path.lineTo(x0 + 8 * r, y0 - dw);
        path.lineTo(x0 + 8 * r, y0 - dw + 8 * r);
        path.lineTo(x0 + 8 * r - dw, y0 - dw + 8 * r);
        painter.drawPath(path);
    }
}

CloseButton::CloseButton(const QString &iconPath, QWidget *parent)
    : SvgTitleBarButton(iconPath, parent)
{
    setHoverColor(Qt::white);
    setPressedColor(Qt::white);
    setHoverBgColor(QColor(232, 17, 35));
    setPressedBgColor(QColor(241, 112, 122));
}
