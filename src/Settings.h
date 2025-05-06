#ifndef SETTINGS_H
#define SETTINGS_H

#include "ui_Settings.h"
#include "includeFiles.h" // For Theme enum
#include "BasicWidget.h"
QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWidget;
}
QT_END_NAMESPACE

class Settings : public BasicWidget
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();
    
private slots:
    void on_spinBox_fontSize_valueChanged(int arg1);

private:
    Ui::Settings* ui;
protected:
    void closeEvent(QCloseEvent* event) override;
};

#endif // SETTINGS_H
