#pragma once

#include <QPushButton>
#include "includeFiles.h"

class LoopModeSwitcher : public QPushButton {
    Q_OBJECT

public:
    enum Mode {
        ListLoop,
        SingleLoop,
        RandomPlay
    };
    Q_ENUM(Mode)

    explicit LoopModeSwitcher(QWidget* parent = nullptr);

    Mode currentMode() const;

signals:
    void playModeSwitched(Mode mode);

private slots:
    void switchMode();

private:
    Mode mode{ ListLoop };
    void updateIcon();
};
