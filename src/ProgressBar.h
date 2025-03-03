#pragma once

#include <QSlider>
#include "includeFiles.h"

class ProgressBar  : public QSlider
{
	Q_OBJECT

public:
	ProgressBar(QObject *parent);
	~ProgressBar();
};
