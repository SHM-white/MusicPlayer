#include "ChangePlaybackSpeedButton.h"
#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionViewItem>

ChangePlaybackSpeedWidget::ChangePlaybackSpeedWidget(QWidget* parent)
    : BasicWidget(parent), m_backgroundColor(Qt::white) {
    this->hide();
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    speedList = new QListWidget(this);
    speedList->setSpacing(10); // Increase spacing between items
    layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(speedList);

    connect(speedList, &QListWidget::itemClicked, this, [parent, this](QListWidgetItem* item) {
        emit static_cast<ChangePlaybackSpeedButton*>(parent)->speedChanged(item->data(Qt::UserRole).toDouble());
        this->hide();
    });

    adjustHeightBasedOnItems();
}

ChangePlaybackSpeedWidget::~ChangePlaybackSpeedWidget() {
    delete speedList;
    delete layout;
}

void ChangePlaybackSpeedWidget::focusOutEvent(QFocusEvent* event) {
    this->hide();
    BasicWidget::focusOutEvent(event);
}

void ChangePlaybackSpeedWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    QPainterPath path;
    path.addRoundedRect(rect(), 5, 5);
    painter.fillPath(path, QBrush(m_backgroundColor));
    painter.end();
}

QColor ChangePlaybackSpeedWidget::backgroundColor() const {
    return m_backgroundColor;
}

void ChangePlaybackSpeedWidget::setBackgroundColor(const QColor& color) {
    if (m_backgroundColor != color) {
        m_backgroundColor = color;
        emit backgroundColorChanged();
        update();
    }
}

void ChangePlaybackSpeedWidget::adjustHeightBasedOnItems() {
    int itemCount = speedList->count();
    int itemHeight = 40; // Approximate height of each item including spacing
    int totalHeight = itemCount * itemHeight + 20; // Add some padding
    this->setFixedHeight(totalHeight);
}

ChangePlaybackSpeedButton::ChangePlaybackSpeedButton(QWidget* parent)
    : QPushButton(parent) {
    speedWidget = std::make_unique<ChangePlaybackSpeedWidget>(this);
    setupSpeedOptions();
}

ChangePlaybackSpeedButton::~ChangePlaybackSpeedButton() {}

void ChangePlaybackSpeedButton::mousePressEvent(QMouseEvent* event) {
    if (speedWidget->isHidden()) {
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
    speedWidget->adjustHeightBasedOnItems();
}
