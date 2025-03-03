#pragma once

#include <QPushButton>

class PlayPauseButton  : public QPushButton
{
	Q_OBJECT

public:
	PlayPauseButton(QWidget *parent);
	~PlayPauseButton();
};
