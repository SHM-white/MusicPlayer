#include "VolumeFlyoutMenu.h"
#include <QMouseEvent>

VolumeFlyoutMenu::VolumeFlyoutMenu(QWidget* parent)
    : QWidget(parent)
{
    this->hide();
    connect(&_hideTimer, &QTimer::timeout, this, &VolumeFlyoutMenu::autoHide);
}

VolumeFlyoutMenu::~VolumeFlyoutMenu()
{
}

void VolumeFlyoutMenu::leaveEvent(QEvent* event)
{
    _hideTimer.start(500); // Start the timer to hide the widget after 500ms
    QWidget::leaveEvent(event); // Call the base class implementation
}

void VolumeFlyoutMenu::mouseMoveEvent(QMouseEvent* event)
{
    QPoint currentPos = event->globalPos();
    if ((_lastMousePos - currentPos).manhattanLength() > _hideDistance) {
        _hideTimer.start(500); // Start the timer to hide the widget after 500ms
    } else {
        _hideTimer.stop(); // Stop the timer if the mouse is inside the widget
    }
    _lastMousePos = currentPos;
    QWidget::mouseMoveEvent(event); // Call the base class implementation
}

void VolumeFlyoutMenu::autoHide()
{
    hide();
}

