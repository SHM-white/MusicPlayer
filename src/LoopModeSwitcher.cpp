#include "LoopModeSwitcher.h"

LoopModeSwitcher::LoopModeSwitcher(QWidget* parent)
    : QPushButton(parent), mode(ListLoop) {
    Icons::SetFont(this);
    updateIcon();
    connect(this, SIGNAL(clicked()), this, SLOT(switchMode()));
}

LoopModeSwitcher::Mode LoopModeSwitcher::currentMode() const {
    return mode;
}

void LoopModeSwitcher::switchMode() {
    mode = static_cast<Mode>((mode + 1) % 3); // Cycle through the modes
    updateIcon();
    emit playModeSwitched(mode);
}

void LoopModeSwitcher::updateIcon() {
    switch (mode) {
        case SingleLoop:
            setText(Icons::Get(Icons::RepeatOne));
            break;
        case ListLoop:
            setText(Icons::Get(Icons::RepeatAll));
            break;
        case RandomPlay:
            setText(Icons::Get(Icons::Shuffle));
            break;
    }
}
