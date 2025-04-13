#include "PlayPauseButton.h"

PlayPauseButton::PlayPauseButton(QWidget *parent)
	: QPushButton(parent)
{
	this->setFont(QFont(QStringLiteral("Segoe Fluent Icons")));
	_updateText();
}

PlayPauseButton::~PlayPauseButton()
{}

bool PlayPauseButton::isPlaying() const
{
	return _isPlaying;
}

PlayPauseButton& PlayPauseButton::setIsPlaying(bool isPlaying)
{
	_isPlaying = isPlaying;
	emit IsPlayingChanged(_isPlaying);
	return *this;
}

void PlayPauseButton::_updateText()
{
	auto text = _isPlaying ? QChar{ Icons::Pause } : QChar{ Icons::Play };
	setText(text);
}
