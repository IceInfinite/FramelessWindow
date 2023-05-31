#include "titlebar.h"

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#include <QEvent>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QPoint>
#include <QLabel>

TitleBar::TitleBar(QWidget *parent)
    : QWidget(parent), m_closeBtn(this, "/btn/close.svg")
    , m_isDoubleClickedEnabled(false), m_iconLabel(new QLabel(this))
    , m_titleLabel(new QLabel(this))
{
    QHBoxLayout *hBoxLayout = new QHBoxLayout(this);
    resize(200, 32);
    setFixedHeight(32);

    hBoxLayout->setSpacing(0);
    hBoxLayout->setContentsMargins(0, 0, 0, 0);
    hBoxLayout->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    hBoxLayout->addStretch(1);
    hBoxLayout->addWidget(&m_minBtn, 0, Qt::AlignRight);
    hBoxLayout->addWidget(&m_maxBtn, 0, Qt::AlignRight);
    hBoxLayout->addWidget(&m_closeBtn, 0, Qt::AlignRight);

    m_minBtn.setHoverColor(Qt::white);
    m_minBtn.setHoverBgColor(QColor(0, 100, 182));
    m_minBtn.setPressedColor(Qt::white);
    m_minBtn.setPressedBgColor(QColor(54, 57, 65));

    m_maxBtn.setHoverColor(Qt::white);
    m_maxBtn.setHoverBgColor(QColor(0, 100, 182));
    m_maxBtn.setPressedColor(Qt::white);
    m_maxBtn.setPressedBgColor(QColor(54, 57, 65));

    connect(&m_minBtn, &QAbstractButton::clicked, window(), &QWidget::showMinimized);
    connect(&m_maxBtn, &QAbstractButton::clicked, &TitleBar::toggleMaxState);
    connect(&m_closeBtn, &QAbstractButton::clicked, window(), &QWidget::close);

    window()->installEventFilter(this);

    // add window icon
    m_iconLabel->setFixedSize(20, 20);
    hBoxLayout->insertSpacing(0, 10);
    hBoxLayout->insertWidget(1, m_iconLabel, 0, Qt::AlignLeft);
    // add title label
    hBoxLayout->insertWidget(2, m_titleLabel, 0, Qt::AlignLeft);
    // m_titleLabel->setStyleSheet("
    //     QLabel{
    //         background: transparent;
    //         font: 13px 'Segoe UI';
    //         padding: 0 4px
    //     }
    // ");
    connect(window(), &QWidget::windowIconChanged, this, &TitleBar::setIcon);
    connect(window(), &QWidget::windowTitleChanged, this, &TitleBar::setTitle);
}

void TitleBar::setDoubleClickEnabled(bool enable)
{
    m_isDoubleClickedEnabled = enable;
}

void TitleBar::setTitle(const QString &title)
{
    m_titleLabel->setText(title);
    m_titleLabel->adjustSize();
}

void TitleBar::setIcon(const QIcon &icon)
{
    m_iconLabel->setPixmap(icon.pixmap(20, 20));
}

bool TitleBar::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == window())
    {
        if (event->type() == QEvent::WindowStateChange)
        {
            m_maxBtn.setMaxState(window()->isMaximized());
            return false;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || !m_isDoubleClickedEnabled)
        return;

    toggleMaxState();
}

void TitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if (!canDrag(event->pos()))
        return;

#ifdef Q_OS_WIN
    if (::ReleaseCapture())
        ::SendMessage(reinterpret_cast<HWND>(window()->winId()), WM_SYSCOMMAND,
                    SC_MOVE | HTCAPTION, 0);
#endif
}

void TitleBar::mousePressEvent(QMouseEvent *event)
{
    if (!canDrag(event->pos()))
        return;

    // for other system, Mac/Linux
}

void TitleBar::toggleMaxState()
{
    if (window()->isMaximized())
        window()->showNormal();
    else
        window()->showMaximized();
}

bool TitleBar::isDragRegion(const QPoint &pos)
{
    int width = 0;
    QList<TitleBarButton *> btns = findChildren<TitleBarButton *>();
    for (auto btn : btns)
    {
        if (btn->isVisible())
            width += btn->width();
    }

    return 0 < pos.x() < this->width() - width;
}

bool TitleBar::hasButtonPressed()
{    
    QList<TitleBarButton *> btns = findChildren<TitleBarButton *>();
    for (auto btn : btns)
    {
        if (btn->isPressed())
            return true;
    }
    return false;
}

bool TitleBar::canDrag(const QPoint &pos)
{
    return isDragRegion(pos) && !hasButtonPressed();
}
