#include "ChangeVolumeButton.h"

ChangeVolumeButton::ChangeVolumeButton(QWidget *parent)
	: QPushButton(parent)
{
    volumeWidget = std::make_unique<ChangeVolumeWidget>(nullptr);
    setFont(QFont(QStringLiteral("Segoe Fluent Icons")));
    //volumeWidget->setMouseTracking(true);
    volumeWidget->setGeometry(QRect(0, 0, 120, 40));
    volumeWidget->setAttribute(Qt::WA_DeleteOnClose);
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

void ChangeVolumeButton::mousePressEvent(QMouseEvent* event) {
    if (volumeWidget->isHidden()) {
        if (volumeWidget->_hideTimer.remainingTime() > 0 && _ignoreNextShow)
        {
            _ignoreNextShow = false;
            return;
        }
        // 获取按钮在屏幕上的全局位置
        QPoint globalPos = this->mapToGlobal(QPoint(0, 0));
        // 获取按钮的位置和大小
        QRect buttonRect = this->geometry();
        // 计算 volumeWidget 的新位置，使其显示在按钮的上方并且中心对齐
        int x = globalPos.x() + (buttonRect.width() - volumeWidget->width()) / 2;
        int y = globalPos.y() - volumeWidget->height();
        volumeWidget->move(x, y);
        volumeWidget->show();
        volumeWidget->setFocus();
        _ignoreNextShow = true; // Ignore the next focus out event
    }
    else
    {
        volumeWidget->hide();
    }
}

ChangeVolumeWidget::ChangeVolumeWidget(QWidget* parent)
    : BasicWidget(parent)
{
    this->hide(); 
    slider = new QSlider(this);
    slider->setOrientation(Qt::Horizontal);
    slider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

ChangeVolumeWidget::~ChangeVolumeWidget()
{
    delete slider;
}

void ChangeVolumeWidget::focusOutEvent(QFocusEvent* event)
{
    if (event->reason() == Qt::FocusReason::MouseFocusReason && slider->underMouse()) {
        BasicWidget::focusOutEvent(event); // Call the base class implementation
        setFocus();
        return; // Do not hide if the focus is moving to the slider
    }
    this->hide(); // Hide the widget when it loses focus
    _hideTimer.setInterval(50);
    _hideTimer.setSingleShot(true);
    _hideTimer.start();
    BasicWidget::focusOutEvent(event); // Call the base class implementation
}

