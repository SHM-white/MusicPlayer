#include "Settings.h"
#include <QWKWidgets/widgetwindowagent.h>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "widget.h" // For MainWidget


Settings::Settings(QWidget *parent)
    : QWidget(parent), ui{new Ui::Settings}
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window);
}

Settings::~Settings()
{
}
