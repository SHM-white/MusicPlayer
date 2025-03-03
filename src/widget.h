#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "BasicWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWidget;
}
QT_END_NAMESPACE

namespace QWK {
    class WidgetWindowAgent;
    class StyleAgent;
}

class MainWidget : public QMainWindow
{
    Q_OBJECT

public:
    enum Theme {
        Dark,
        Light,
    };
    Q_ENUM(Theme)
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();
    Theme currentTheme{};
    QWK::WidgetWindowAgent *windowAgent;

Q_SIGNALS:
    void themeChanged();

private:
    void installWindowAgent();
    void loadStyleSheet(Theme theme);
    bool event(QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    Ui::MainWidget *ui;
    bool paintTransparentBackground{ true };
};
#endif // WIDGET_H
