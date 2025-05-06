#ifndef SETTINGS_H
#define SETTINGS_H

#include "ui_Settings.h"
#include "includeFiles.h" // For Theme enum

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWidget;
}
QT_END_NAMESPACE

class Settings : public QWidget
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

private:
    Ui::Settings* ui;
};

#endif // SETTINGS_H
