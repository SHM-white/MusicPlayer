#pragma once
#include "includeFiles.h"
#include <QPushButton>

class PlayPauseButton : public QPushButton
{
	Q_OBJECT

public:
	PlayPauseButton(QWidget* parent);
	~PlayPauseButton();
	bool isPlaying() const;
	PlayPauseButton& setIsPlaying(bool isPlaying);
	//void paintEvent(QPaintEvent* event) override {
	//	QPainter painter{ this };
	//	painter.end();
	//}

Q_SIGNALS:
	void PlayPauseClicked();

protected:
	//void keyPressEvent(QKeyEvent* e) override {
	//	e->key() == Qt::Key::Key_Left;
	//	this->setText(QChar{ 0xE768 });
	//	this->setFont(QFont(QStringLiteral("Segoe Fluent Icons")));

	//}
	void mousePressEvent(QMouseEvent* event) override {
		if (event->button() == Qt::MouseButton::LeftButton) {
			_isPlaying = !_isPlaying;
			_updateText();
			emit PlayPauseClicked();
		}
	}
private:
	void _updateText();
	bool _isPlaying{ false };
};