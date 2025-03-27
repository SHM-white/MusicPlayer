#pragma once

#include <QPushButton>
#include "includeFiles.h"
#include "BasicWidget.h"

class ChangeVolumeWidget : public BasicWidget {
	Q_OBJECT

public:
	ChangeVolumeWidget(QWidget* parent);
	~ChangeVolumeWidget();
    void setIgnoreNextFocusOut();
protected:
    void focusOutEvent(QFocusEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private slots:
    void autoHide();

private:
    QTimer _hideTimer;
    bool _autoHide{ true };
    bool _ignoreNextFocusOut{ false };
    QPoint _lastMousePos;
    int _hideDistance{ 100 }; // Distance in pixels to trigger auto-hide
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

private:
	void _setVolumeIcon();
    int m_volume;
	std::unique_ptr<ChangeVolumeWidget> volumeWidget;

protected Q_SLOTS:

	void mousePressEvent(QMouseEvent* event) override;
};

