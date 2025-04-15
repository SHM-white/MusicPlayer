#pragma once

#include <QWidget>
#include "ui_DisplayWidget.h"

class DisplayWidget : public QWidget
{
	Q_OBJECT

public:
	DisplayWidget(QWidget *parent = nullptr);
	~DisplayWidget();

private:
	Ui::DisplayWidget ui;
};
