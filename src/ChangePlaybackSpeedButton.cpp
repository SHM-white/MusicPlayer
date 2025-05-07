#include "ChangePlaybackSpeedButton.h"
#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionViewItem>

ChangePlaybackSpeedWidget::ChangePlaybackSpeedWidget(QWidget* parent)
    : BasicWidget(parent) {
    this->hide();
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
	this->setObjectName("playbackSpeedWidget");
    speedList = new QListWidget(this);
    speedList->setSpacing(10); // Increase spacing between items
    layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(speedList);

    adjustHeightBasedOnItems();
}

ChangePlaybackSpeedWidget::~ChangePlaybackSpeedWidget() {
    delete speedList;
    delete layout;
}

void ChangePlaybackSpeedWidget::focusOutEvent(QFocusEvent* event) {
    this->hide();
    _hideTimer.setInterval(50);
    _hideTimer.setSingleShot(true);
    _hideTimer.start();
    BasicWidget::focusOutEvent(event);
}

void ChangePlaybackSpeedWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    QPainterPath path;
    path.addRoundedRect(rect(), 5, 5);
    painter.fillPath(path, QBrush(m_backgroundColor));
    painter.end();
}



void ChangePlaybackSpeedWidget::adjustHeightBasedOnItems() {
    int itemCount = speedList->count();
    int itemHeight = 40; // Approximate height of each item including spacing
    int totalHeight = itemCount * itemHeight + 30; // Add some padding
    this->setFixedHeight(totalHeight);
    this->setFixedWidth(100);
}

ChangePlaybackSpeedButton::ChangePlaybackSpeedButton(QWidget* parent)
    : QPushButton(parent) {
    speedWidget = std::make_unique<ChangePlaybackSpeedWidget>(this);
	connect(speedWidget->speedList, &QListWidget::itemClicked, [this](QListWidgetItem* item) {
		if (item) {
			double speed = item->data(Qt::UserRole).toDouble();
			setCurrentSpeed(speed);
			speedWidget->hide();
		}
		});
    setupSpeedOptions();
}

ChangePlaybackSpeedButton::~ChangePlaybackSpeedButton() {}

void ChangePlaybackSpeedButton::mousePressEvent(QMouseEvent* event) {
    if (speedWidget->isHidden()) {
        if (speedWidget->_hideTimer.remainingTime() > 0 && _ignoreNextShow)
        {
            _ignoreNextShow = false;
            return;
        }
        QPoint globalPos = this->mapToGlobal(QPoint(0, 0));
        QRect buttonRect = this->geometry();
        int x = globalPos.x() + (buttonRect.width() - speedWidget->width()) / 2;
        int y = globalPos.y() - speedWidget->height() - 10;
        speedWidget->move(x, y);
        speedWidget->show();
        speedWidget->setFocus();
        _ignoreNextShow = true;
    } else {
        speedWidget->hide();
    }
}

void ChangePlaybackSpeedButton::setupSpeedOptions() {
    QList<double> speeds = {0.5, 0.75, 1.0, 1.25, 1.5, 2.0};
    for (double speed : speeds) {
        QListWidgetItem* item = new QListWidgetItem(QString::number(speed) + "x", speedWidget->speedList);
        item->setData(Qt::UserRole, speed);
        speedWidget->speedList->addItem(item);
    }
	speedWidget->speedList->setCurrentRow((int)speeds.indexOf(1.0, 0));
    speedWidget->adjustHeightBasedOnItems();
}

double ChangePlaybackSpeedButton::currentSpeed() const
{
    return m_currentSpeed;
}

void ChangePlaybackSpeedButton::setCurrentSpeed(double newCurrentSpeed)
{
    if (qFuzzyCompare(m_currentSpeed, newCurrentSpeed))
        return;
    m_currentSpeed = newCurrentSpeed;
    emit currentSpeedChanged(newCurrentSpeed);
}
