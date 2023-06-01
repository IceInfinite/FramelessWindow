#include "framelesswidget.h"

#include <cmath>
#include <vector>

#ifdef Q_OS_WIN
#include <Windows.h>
#include <dwmapi.h>
#include <windowsx.h>
#include <wingdi.h>
#endif

#include <QDebug>
#include <QGuiApplication>
#include <QOperatingSystemVersion>
#include <QScreen>
#include <QWindow>

#ifdef Q_OS_WIN
constexpr int kBorderWidth = 5;
constexpr int kTaskbarAutoHideThickness = 2;

enum class TaskbarPostion
{
    kLeft = 0,
    kTop = 1,
    kRight = 2,
    kBottom = 3,
    kNoPos = 4
};

bool isFullScreenWin(HWND hWnd)
{
    RECT winRect;
    ::GetWindowRect(hWnd, &winRect);

    HMONITOR monitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO monitorInfo;
    if (::GetMonitorInfo(monitor, &monitorInfo) == FALSE)
        return false;

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

int getDpiForWindow(HWND hWnd, bool horizontal)
{
#if (WINVER >= 0x0605)
    return ::GetDpiForWindow(hWnd);
#endif
    HDC hdc = ::GetDC(hWnd);
    int dpiX = ::GetDeviceCaps(hdc, LOGPIXELSX);
    int dpiY = ::GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(hWnd, hdc);
    if (dpiX > 0 && horizontal)
        return dpiX;
    else if (dpiY > 0 && !horizontal)
        return dpiY;

    return 96;
}

int getSystemMetrics(HWND hWnd, int index, bool horizontal)
{
#if (WINVER >= 0x0605)
    int dpi = getDpiForWindow(hWnd, horizontal);
    return ::GetSystemMetricsForDpi(index, dpi);
#endif
    return ::GetSystemMetrics(index);
}

bool isCompositionEnabled()
{
    BOOL result = FALSE;
    bool success = (::DwmIsCompositionEnabled(&result) == S_OK);
    return (result == TRUE) && success;
}

int getResizeBorderThickness(HWND hWnd, bool horizontal)
{
    QWindow *window = findWindow(hWnd);
    if (!window)
        return 0;

    int frame = SM_CYSIZEFRAME;
    if (horizontal)
        frame = SM_CXSIZEFRAME;

    int result = getSystemMetrics(hWnd, frame, horizontal) +
                 getSystemMetrics(hWnd, 92, horizontal);

    if (result > 0)
        return result;

    int thickness = 8;
    if (!isCompositionEnabled())
        thickness = 4;

    return std::round(thickness * window->devicePixelRatio());
}

void addWindowAnimation(HWND hWnd)
{
    LONG style = ::GetWindowLong(hWnd, GWL_STYLE);
    ::SetWindowLong(
        hWnd, GWL_STYLE,
        style | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CAPTION | CS_DBLCLKS |
            WS_THICKFRAME);
}

void addShadowEffect(HWND hWnd)
{
    if (!isCompositionEnabled())
        return;

    MARGINS margins;
    margins.cxLeftWidth = -1;
    margins.cyTopHeight = -1;
    margins.cxRightWidth = -1;
    margins.cyBottomHeight = -1;
    ::DwmExtendFrameIntoClientArea(hWnd, &margins);
}

bool isTaskbarAutoHide()
{
    APPBARDATA appbarData;
    memset(&appbarData, 0, sizeof(APPBARDATA));
    appbarData.cbSize = sizeof(APPBARDATA);
    UINT_PTR taskbarState = ::SHAppBarMessage(ABM_GETSTATE, &appbarData);
    return (taskbarState == ABS_AUTOHIDE);
}

bool isGreaterEqualWin8_1()
{
    return QOperatingSystemVersion::current() >=
           QOperatingSystemVersion::Windows8_1;
}

bool isGreaterWin7()
{
    return QOperatingSystemVersion::current() >
           QOperatingSystemVersion::Windows7;
}

TaskbarPostion getTaskbarPosition(HWND hWnd)
{
    APPBARDATA appbarData;
    memset(&appbarData, 0, sizeof(APPBARDATA));
    appbarData.cbSize = sizeof(APPBARDATA);
    if (isGreaterEqualWin8_1())
    {
        HMONITOR monitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo;
        if (::GetMonitorInfo(monitor, &monitorInfo) == FALSE)
            return TaskbarPostion::kNoPos;
        std::vector<TaskbarPostion> postitons = {
            TaskbarPostion::kLeft, TaskbarPostion::kTop, TaskbarPostion::kRight,
            TaskbarPostion::kBottom};
        appbarData.rc = monitorInfo.rcMonitor;
        for (auto pos : postitons)
        {
            appbarData.uEdge = static_cast<UINT>(pos);
            if (::SHAppBarMessage(ABM_GETAUTOHIDEBAREX, &appbarData) != NULL)
                return pos;
        }

        return TaskbarPostion::kNoPos;
    }

    appbarData.hWnd = ::FindWindow(TEXT("Shell_TrayWnd"), NULL);

    if (appbarData.hWnd)
    {
        HMONITOR windowMonitor =
            ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
        HMONITOR taskbarMonitor =
            ::MonitorFromWindow(appbarData.hWnd, MONITOR_DEFAULTTOPRIMARY);
        if (windowMonitor && taskbarMonitor && windowMonitor == taskbarMonitor)
        {
            ::SHAppBarMessage(ABM_GETTASKBARPOS, &appbarData);
            return static_cast<TaskbarPostion>(appbarData.uEdge);
        }
    }

    return TaskbarPostion::kNoPos;
}

#endif

FramelessWidget::FramelessWidget(QWidget *parent)
    : QWidget(parent), m_titleBar(new TitleBar(this)), m_isResizeEnable(true)
{
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_DontCreateNativeAncestors);

    if (isGreaterWin7())
        setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    else if (parent)
        setWindowFlags(parent->windowFlags() | Qt::FramelessWindowHint);
    else
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMaximizeButtonHint);

    resize(500, 500);
#ifdef Q_OS_WIN
    connect(
        windowHandle(), &QWindow::screenChanged, this,
        &FramelessWidget::onScreenChanged);
    addWindowAnimation(reinterpret_cast<HWND>(winId()));
    addShadowEffect(reinterpret_cast<HWND>(winId()));
#endif
    m_titleBar->raise();
}

FramelessWidget::~FramelessWidget() {}

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
    QWidget::resizeEvent(event);
    m_titleBar->resize(width(), m_titleBar->height());
}

bool FramelessWidget::nativeEvent(
    const QByteArray &eventType, void *message, long *result)
{
#ifdef Q_OS_WIN
    MSG *msg = reinterpret_cast<MSG *>(message);
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
            bool right = (cursorX - x()) > (width() - kBorderWidth);
            bool top = (cursorY - y()) < kBorderWidth;
            bool bottom = (cursorY - y()) > (height() - kBorderWidth);

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

            break;
        }
        case WM_NCCALCSIZE:
        {
            LPRECT rect;
            if (msg->wParam == TRUE)
            {
                rect = reinterpret_cast<LPRECT>(msg->lParam);
            }
            else if (msg->wParam == FALSE)
            {
                rect = &(reinterpret_cast<LPNCCALCSIZE_PARAMS>(msg->lParam)
                             ->rgrc[0]);
            }
            else
            {
                *result = 0;
                return true;
            }

            bool max = (IsMaximized(msg->hwnd) == TRUE);
            bool fullScreen = isFullScreenWin(msg->hwnd);

            if (max && !fullScreen)
            {
                int borderY = getResizeBorderThickness(msg->hwnd, false);
                rect->top += borderY;
                rect->bottom -= borderY;

                int borderX = getResizeBorderThickness(msg->hwnd, true);
                rect->left += borderX;
                rect->right -= borderX;
            }

            if ((max || fullScreen) && isTaskbarAutoHide())
            {
                TaskbarPostion pos = getTaskbarPosition(msg->hwnd);
                switch (pos)
                {
                    case TaskbarPostion::kLeft:
                        rect->left += kTaskbarAutoHideThickness;
                        break;
                    case TaskbarPostion::kRight:
                        rect->right -= kTaskbarAutoHideThickness;
                        break;
                    case TaskbarPostion::kTop:
                        rect->top += kTaskbarAutoHideThickness;
                        break;
                    case TaskbarPostion::kBottom:
                        rect->bottom -= kTaskbarAutoHideThickness;
                        break;
                }
            }

            if (msg->wParam == FALSE)
                *result = 0;
            else
                *result = WVR_REDRAW;

            return true;
        }
    }
#endif
    return QWidget::nativeEvent(eventType, message, result);
}

void FramelessWidget::onScreenChanged(QScreen *screen)
{
#ifdef Q_OS_WIN
    Q_UNUSED(screen)
    HWND hWnd = reinterpret_cast<HWND>(windowHandle()->winId());
    ::SetWindowPos(
        hWnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
#endif
}
