#include "widget.h"
#include "./ui_widget.h"

MainWidget::MainWidget(QWidget *parent)
    : BasicWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);
}

MainWidget::~MainWidget()
{
    delete ui;
}
