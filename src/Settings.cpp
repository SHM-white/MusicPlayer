#include "Settings.h"
#include "ConfigManager.h"

Settings::Settings(QWidget *parent)
    : BasicWidget(parent), ui{new Ui::Settings}
{
    ui->setupUi(this);
	this->setObjectName("Settings");
    this->setWindowFlags(Qt::Window);
    ui->spinBox_fontSize->setValue(GlobalConfigs::APPLICATION_SETTINGS.value("lyricsFontSize", 14).toInt());
    //ui->scrollAreaWidgetContents->setAttribute(Qt::WA_TranslucentBackground);
    //ui->scrollAreaWidgetContents->setAutoFillBackground(false);
    ui->scrollArea->setAttribute(Qt::WA_TranslucentBackground);
}

Settings::~Settings()
{
    delete ui;
}
void Settings::closeEvent(QCloseEvent* event)
{
	ConfigManager::SaveSettings(GlobalConfigs::CONFIG_FILE_PATH(), GlobalConfigs::APPLICATION_SETTINGS);
    QWidget::closeEvent(event);
}

void Settings::on_spinBox_fontSize_valueChanged(int arg1)
{
    GlobalConfigs::APPLICATION_SETTINGS[QStringLiteral("lyricsFontSize")] = arg1;
}