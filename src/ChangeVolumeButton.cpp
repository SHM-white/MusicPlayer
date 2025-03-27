#include "ChangeVolumeButton.h"

ChangeVolumeButton::ChangeVolumeButton(QWidget *parent)
    : QPushButton(parent)
{
    volumeMenu = std::make_unique<VolumeFlyoutMenu>(dynamic_cast<QWidget*>(this->parent()));
    setFont(QFont(QStringLiteral("Segoe Fluent Icons")));
    _setVolumeIcon();
}

ChangeVolumeButton::~ChangeVolumeButton()
{
}

int ChangeVolumeButton::volume() const
{
    return m_volume;
}

void ChangeVolumeButton::setVolume(int newVolume)
{
    if (m_volume == newVolume)
        return;
    m_volume = newVolume;
    _setVolumeIcon();
    emit volumeChanged();
}

QChar ChangeVolumeButton::getVolumeIcon() const
{
    if (volume() >= 75)
    {
        return QChar(Icons::Volume3);
    }
    else if (volume() >= 50)
    {
        return QChar(Icons::Volume2);
    }
    else if (volume() >= 25)
    {
        return QChar(Icons::Volume1);
    }
    else if (volume() > 0)
    {
        return QChar(Icons::Volume0);
    }
    else {
        return QChar(Icons::Mute);
    }

}

void ChangeVolumeButton::_setVolumeIcon()
{
    this->setText(getVolumeIcon());
}

void ChangeVolumeButton::mousePressEvent(QMouseEvent* event)
{
    if (volumeMenu->isHidden()) {
        // 获取按钮的位置和大小
        QRect buttonRect = this->geometry();
        // 计算 volumeMenu 的新位置，使其显示在按钮的上方并且中心对齐
        int x = buttonRect.x() + (buttonRect.width() - volumeMenu->width()) / 2;
        int y = buttonRect.y() - volumeMenu->height();
        volumeMenu->move(x, y);
        volumeMenu->show();
    }
    else
    {
        volumeMenu->hide();
    }
    QPushButton::mousePressEvent(event); // Call the base class implementation
}

ChangeVolumeWidget::ChangeVolumeWidget(QWidget* parent)
    : BasicWidget(parent)
{
    this->hide(); 
    connect(&_hideTimer, &QTimer::timeout, this, &ChangeVolumeWidget::autoHide);
}

ChangeVolumeWidget::~ChangeVolumeWidget()
{
}

void ChangeVolumeWidget::setIgnoreNextFocusOut()
{
    _ignoreNextFocusOut = true;
}

void ChangeVolumeWidget::focusOutEvent(QFocusEvent* event)
{
    if (_ignoreNextFocusOut) {
        _ignoreNextFocusOut = false; // Reset the flag
        return; // Ignore this focus out event
    }
    this->hide(); // Hide the widget when it loses focus
    BasicWidget::focusOutEvent(event); // Call the base class implementation
}

void ChangeVolumeWidget::mouseMoveEvent(QMouseEvent* event)
{
    QPoint currentPos = event->globalPos();
    if (_autoHide && (_lastMousePos - currentPos).manhattanLength() > _hideDistance) {
        _hideTimer.start(500); // Start the timer to hide the widget after 500ms
    } else {
        _hideTimer.stop(); // Stop the timer if the mouse is inside the widget
    }
    _lastMousePos = currentPos;
    BasicWidget::mouseMoveEvent(event); // Call the base class implementation
}

void ChangeVolumeWidget::autoHide()
{
    _autoHide = false;
    hide();
}
