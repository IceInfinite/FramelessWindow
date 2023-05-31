#include "framelesswidget.h"
#include "ui_framelesswidget.h"

#ifdef Q_OS_WIN
#include <Windows.h>
#include <windowsx.h>
#endif

#include <QWindow>

#ifdef Q_OS_WIN
constexpr int kBorderWidth = 5;

bool isFullScreenWin(HWND hWnd)
{
    RECT winRect;
    ::GetWindowRect(hWnd, &winRect);

    HMONITOR monitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO monitorInfo;
    ::GetMonitorInfo(monitor, &monitorInfo);

    RECT monitorRect = monitorInfo.rcMonitor;

    if (monitorRect.top == winRect.top &&
        monitorRect.bottom == winRect.bottom &&
        monitorRect.left == winRect.left && monitorRect.right == winRect.right)
        return true;

    return false;
}

QWindow *findWindow(HWND hWnd)
{
    if (!hWnd)
        return nullptr;
    
    QWindowList windows = QGuiApplication::topLevelWindows();

    if (windows.count() == 0)
        return 0;

    for (auto window : windows)
    {
        if (window && reinterpret_cast<HWND>(window->winId()) == hWnd)
            return window;
    }
}

int getResizeBorderThickness(HWND hWnd, bool horizontal = true)
{
    QWindow * window = findWindow(hWnd);
    if (!window)
        return 0;

    return 0;
}

#endif

FramelessWidget::FramelessWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::FramelessWidget),
    m_titleBar(new TitleBar(this)),
    m_isResizeEnable(true)
{
    m_ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint |
                   Qt::WindowMinMaxButtonsHint);
    
    connect(windowHandle(), &QWindow::screenChanged, this, &FramelessWidget::onScreenChanged);
    m_titleBar->raise();
}

FramelessWidget::~FramelessWidget()
{
    delete m_ui;
}

void FramelessWidget::setTitleBar(TitleBar *titleBar)
{
    if (!titleBar || m_titleBar == titleBar)
        return;

    m_titleBar->deleteLater();
    m_titleBar = titleBar;
    m_titleBar->setParent(this);
    m_titleBar->raise();
}

void FramelessWidget::setResizeEnabled(bool enable)
{
    m_isResizeEnable = enable;
}

void FramelessWidget::resizeEvent(QResizeEvent *event)
{
    m_titleBar->resize(width(), m_titleBar->height());
}

bool FramelessWidget::nativeEvent(const QByteArray &eventType, void *message,
                                  long *result)
{
#ifdef Q_OS_WIN
    MSG* msg = reinterpret_cast<MSG*>(message);
    if (!msg->hwnd)
        return QWidget::nativeEvent(eventType, message, result);

    switch (msg->message)
    {
        case WM_NCHITTEST:
        {
            if (!m_isResizeEnable)
                break;
            *result = HTBORDER;

            int cursorX = GET_X_LPARAM(msg->lParam);
            int cursorY = GET_Y_LPARAM(msg->lParam);

            bool left = (cursorX - x()) < kBorderWidth;
            bool right = (cursorX - x()) > (width() + kBorderWidth);
            bool top = (cursorY - y()) < kBorderWidth;
            bool bottom = (cursorY - y()) > (width() + kBorderWidth);

            if (left && top)
                *result = HTTOPLEFT;
            else if (left && bottom)
                *result = HTBOTTOMLEFT;
            else if (right && top)
                *result = HTTOPRIGHT;
            else if (right && bottom)
                *result = HTBOTTOMRIGHT;
            else if (top)
                *result = HTTOP;
            else if (bottom)
                *result = HTBOTTOM;
            else if (left)
                *result = HTLEFT;
            else if (right)
                *result = HTRIGHT;

            if (left || right || top || bottom)
                return true;
        }
        case WM_NCCALCSIZE:
        {
            RECT rect;
            if (msg->wParam == TRUE)
                rect =
                    reinterpret_cast<LPNCCALCSIZE_PARAMS>(msg->lParam)->rgrc[0];
            else 
                rect = *reinterpret_cast<LPRECT>(msg->lParam);

            bool max = IsMaximized(msg->hwnd);
            bool fullScreen = isFullScreenWin(msg->hwnd);

            if (max && fullScreen)
            {
                int borderY = getResizeBorderThickness(msg->hwnd, false);
                rect.top += borderY;
                rect.bottom -= borderY;

                int borderX = getResizeBorderThickness(msg->hwnd, true);
                rect.left += borderX;
                rect.right -= borderX;
            }

            //  Windows 下自动隐藏任务栏导致窗口最大化后无法显示下方任务栏的问题
            // if ((max || fullScreen) && TaskBar.isAutoHide())
            // {
            // }
            if (!msg->wParam)
                *result = 0;
            else
                *result = WVR_REDRAW;

            return true;
        }
    }
#endif
    return QWidget::nativeEvent(eventType, message, result);
}

void FramelessWidget::onScreenChanged()
{
#ifdef Q_OS_WIN
    HWND hWnd = reinterpret_cast<HWND>(windowHandle()->winId());
    ::SetWindowPos(hWnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE |
                   SWP_FRAMECHANGED);
#endif
}