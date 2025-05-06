#pragma once

#include "includeFiles.h"
#include "BasicWidget.h"

class ChangePlaybackSpeedWidget : public BasicWidget {
    Q_OBJECT

    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged FINAL)

public:
    ChangePlaybackSpeedWidget(QWidget* parent);
    ~ChangePlaybackSpeedWidget();

    QListWidget* speedList;
    QTimer _hideTimer;

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor& color);

    void adjustHeightBasedOnItems(); // Moved from private to public

signals:
    void backgroundColorChanged();

protected:
    void focusOutEvent(QFocusEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    QVBoxLayout* layout;
    QColor m_backgroundColor;
};

class ChangePlaybackSpeedButton : public QPushButton {
    Q_OBJECT

public:
    ChangePlaybackSpeedButton(QWidget* parent);
    ~ChangePlaybackSpeedButton();

signals:
    void speedChanged(double speed);

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    std::unique_ptr<ChangePlaybackSpeedWidget> speedWidget;
    bool _ignoreNextShow{ false };

    void setupSpeedOptions();
};
