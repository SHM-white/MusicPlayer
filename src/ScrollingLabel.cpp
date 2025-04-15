#include "ScrollingLabel.h"

ScrollingLabel::ScrollingLabel(QWidget* parent)
    : QLabel(parent), scrollAnimation(new QPropertyAnimation(this, "pos")), maxWidth(0) {
    setupAnimation();
}

ScrollingLabel::~ScrollingLabel() {
    delete scrollAnimation;
}

void ScrollingLabel::setMaximumWidth(int width) {
    maxWidth = width;
    setFixedWidth(maxWidth);
    updateScrolling();
}

void ScrollingLabel::setText(const QString& text) {
    QLabel::setText(text);
    updateScrolling();
}

void ScrollingLabel::setupAnimation() {
    scrollAnimation->setDuration(5000); // 5 seconds for a full scroll
    scrollAnimation->setLoopCount(-1);  // Infinite loop
}

void ScrollingLabel::updateScrolling() {
    QFontMetrics metrics(font());
    int textWidth = metrics.horizontalAdvance(text());

    if (textWidth > maxWidth) {
        scrollAnimation->stop();
        scrollAnimation->setStartValue(QPoint(0, y()));
        scrollAnimation->setEndValue(QPoint(-textWidth + maxWidth, y()));
        scrollAnimation->start();
    } else {
        scrollAnimation->stop();
        move(0, y());
    }
}
