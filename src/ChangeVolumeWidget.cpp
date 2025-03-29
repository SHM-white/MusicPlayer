ChangeVolumeWidget::ChangeVolumeWidget(QWidget* parent)
    : BasicWidget(parent)
{
    this->hide(); 
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint );
    this->setMinimumHeight(30); // Set minimum height
    this->setMaximumHeight(30); // Set maximum height to ensure fixed height
    slider = new QSlider(this);
    slider->setOrientation(Qt::Horizontal);
    slider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // Adjust size policy
    slider->setObjectName(QStringLiteral("VolumeSlider"));
    slider->setMaximum(100);
    verticallayout = new QVBoxLayout(this);
    verticallayout->setContentsMargins(0, 0, 0, 0); // Remove margins
    verticallayout->addWidget(slider);
    setLightMode(); // Set initial mode to light mode
}
