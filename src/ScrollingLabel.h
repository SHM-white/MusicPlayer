#pragma once

#include <QLabel>
#include <QPropertyAnimation>

class ScrollingLabel : public QLabel {
   Q_OBJECT

public:
   explicit ScrollingLabel(QWidget* parent = nullptr);
   ~ScrollingLabel();

   void setMaximumWidth(int width);
public Q_SLOTS:
   void setText(const QString& text);

private:
   QPropertyAnimation* scrollAnimation;
   int maxWidth;

   void setupAnimation();
   void updateScrolling();
};
