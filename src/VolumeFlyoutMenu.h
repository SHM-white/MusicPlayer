#pragma once

#include <QWidget>
#include <QTimer>

class VolumeFlyoutMenu : public QWidget {
    Q_OBJECT

public:
    VolumeFlyoutMenu(QWidget* parent = nullptr);
    ~VolumeFlyoutMenu();

protected:
    void leaveEvent(QEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private slots:
    void autoHide();

private:
    QTimer _hideTimer;
    QPoint _lastMousePos;
    int _hideDistance{ 100 }; // Distance in pixels to trigger auto-hide
};

