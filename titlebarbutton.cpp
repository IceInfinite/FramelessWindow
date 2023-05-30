#include "titlebarbutton.h"

TitleBarButton::TitleBarButton(QWidget *parent) : QAbstractButton(parent)
{
    setCursor(Qt::ArrowCursor);
    setFixedSize(46, 32);

}
