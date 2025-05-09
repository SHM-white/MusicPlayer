﻿#include "ChangeVolumeButton.h"

ChangeVolumeButton::ChangeVolumeButton(QWidget *parent)
	: QPushButton(parent)
{
    volumeWidget = std::make_unique<ChangeVolumeWidget>(dynamic_cast<QWidget*>(this));
    setFont(Icons::Font);
    volumeWidget->setGeometry(QRect(0, 0, 100, 10)); 
    connect(volumeWidget->slider, SIGNAL(valueChanged(int)), this, SLOT(sliderMoved(int)));
    connect(volumeWidget->slider, SIGNAL(valueChanged(int)), this, SLOT(updateTooltip(int)));
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
    emit volumeChanged(m_volume);
}
void ChangeVolumeButton::sliderMoved(int value) {
    setVolume(value);
}

void ChangeVolumeButton::updateTooltip(int value) {
    // Calculate the position of the slider handle
    int sliderPosition = volumeWidget->slider->style()->sliderPositionFromValue(volumeWidget->slider->minimum(), volumeWidget->slider->maximum(), value, volumeWidget->slider->width());
    QPoint handleGlobalPos = volumeWidget->slider->mapToGlobal(QPoint(sliderPosition, 0));
    
    // Calculate the center position above the volumeWidget
    QPoint widgetGlobalPos = volumeWidget->mapToGlobal(volumeWidget->rect().center());
    QPoint tooltipPos(widgetGlobalPos.x(), widgetGlobalPos.y() - 40); // Adjust the y-coordinate to display above the volumeWidget
    
    QToolTip::showText(tooltipPos, QString::number(value), volumeWidget->slider);
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
        int y = globalPos.y() - volumeWidget->height() - 10;
        volumeWidget->move(x, y);
        volumeWidget->show();
        volumeWidget->setFocus();
        volumeWidget->slider->setValue(volume());
        _ignoreNextShow = true; // Ignore the next focus out event
    }
    else {
        volumeWidget->hide();
    }
}

ChangeVolumeWidget::ChangeVolumeWidget(QWidget* parent)
    : BasicWidget(parent)
{
    this->hide(); 
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint );
    this->setFixedHeight(30); 
    this->setObjectName(QStringLiteral("volumeWidget"));
    slider = new QSlider(this);
    slider->setOrientation(Qt::Horizontal);
    slider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // Adjust size policy
    slider->setObjectName(QStringLiteral("VolumeSlider"));
    slider->setMaximum(100);
    verticallayout = new QVBoxLayout(this);
    verticallayout->setContentsMargins(0, 0, 0, 0); // Remove margins
    verticallayout->addWidget(slider);
}

ChangeVolumeWidget::~ChangeVolumeWidget()
{
    delete slider;
    delete verticallayout;
}

void ChangeVolumeWidget::focusOutEvent(QFocusEvent* event)
{
    if (event->reason() == Qt::FocusReason::MouseFocusReason && slider->underMouse()) {
        BasicWidget::focusOutEvent(event);
        setFocus();
        return; 
    }
    this->hide(); 
    _hideTimer.setInterval(50);
    _hideTimer.setSingleShot(true);
    _hideTimer.start();
    BasicWidget::focusOutEvent(event); 
}

void ChangeVolumeWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QPainterPath path;
    path.addRoundedRect(rect(), 5, 5);
    painter.fillPath(path, QBrush(m_backgroundColor));
    painter.end();
}

void ChangeVolumeWidget::on_themeChanged(Theme t) {
    currentTheme = t;
    update();
}
//QColor ChangeVolumeWidget::backgroundColor() const
//{
//    return m_backgroundColor;
//}
//
//void ChangeVolumeWidget::setBackgroundColor(const QColor &newBackgroundColor)
//{
//    if (m_backgroundColor == newBackgroundColor)
//        return;
//    m_backgroundColor = newBackgroundColor;
//    emit backgroundColorChanged();
//}
