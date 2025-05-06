#include "ChangePlaybackSpeedButton.h"

ChangePlaybackSpeedWidget::ChangePlaybackSpeedWidget(QWidget* parent)
    : BasicWidget(parent) {
    this->hide();
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    this->setFixedSize(200, 300);

    speedList = new QListWidget(this);
    layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(speedList);

    //connect(speedList, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
    //    emit static_cast<ChangePlaybackSpeedButton*>(parent())->speedChanged(item->data(Qt::UserRole).toDouble());
    //    this->hide();
    //});
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
    painter.fillPath(path, QBrush(Qt::white));
    painter.end();
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
}
