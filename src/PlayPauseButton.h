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

Q_SIGNALS:
	void PlayPauseClicked();
	void IsPlayingChanged(bool);

protected:
	void mousePressEvent(QMouseEvent* event) override {
		if (event->button() == Qt::MouseButton::LeftButton) {
			setIsPlaying(!isPlaying());
			_updateText();
			emit PlayPauseClicked();
		}
		QPushButton::mousePressEvent(event);
	}
private:
	void _updateText();
	bool _isPlaying{ false };
};