#pragma once

#include <QPushButton>
#include "includeFiles.h"
#include "BasicWidget.h"

class ChangeVolumeWidget : public BasicWidget {
	Q_OBJECT

public:
	ChangeVolumeWidget(QWidget* parent);
	~ChangeVolumeWidget();
	QTimer _hideTimer;
	QSlider* slider;
protected:
    void focusOutEvent(QFocusEvent* event) override;
	void paintEvent(QPaintEvent* event) override;
private:
	QVBoxLayout* verticallayout;
};

class ChangeVolumeButton : public QPushButton
{
	Q_OBJECT

public:
	ChangeVolumeButton(QWidget *parent);
	~ChangeVolumeButton();

    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged FINAL);

    int volume() const;
    void setVolume(int newVolume);
	QChar getVolumeIcon() const;
signals:
    void volumeChanged();
protected slots:
	void sliderMoved(int value);

private:
	void _setVolumeIcon();
	int m_volume{ 100 };
	std::unique_ptr<ChangeVolumeWidget> volumeWidget;
    bool _ignoreNextShow{ false };

protected Q_SLOTS:

	void mousePressEvent(QMouseEvent* event) override;
};

