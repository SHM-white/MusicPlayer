#include "DisplayWidget.h"

DisplayWidget::DisplayWidget(QWidget *parent)
    : QWidget(parent), rotationAngle(0)
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this); // Change to horizontal layout for side-by-side display

    QVBoxLayout *leftLayout = new QVBoxLayout(); // Left layout for rotating album cover
    QVBoxLayout *rightLayout = new QVBoxLayout(); // Right layout for lyrics

    vinylLabel = new QLabel(this);
    albumCoverLabel = new QLabel(this);
    lyricsBrowser = new QTextBrowser(this);

    vinylLabel->setAlignment(Qt::AlignCenter);
    albumCoverLabel->setAlignment(Qt::AlignCenter);
    lyricsBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    leftLayout->addWidget(vinylLabel);
    leftLayout->addWidget(albumCoverLabel);

    rightLayout->addWidget(lyricsBrowser);

    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);

    rotationTimer = new QTimer(this);
    connect(rotationTimer, &QTimer::timeout, this, [this]() {
        rotationAngle = (rotationAngle + 5) % 360;
        if (!vinylLabel->pixmap().isNull()) {
            QPixmap pixmap = vinylLabel->pixmap().transformed(QTransform().rotate(rotationAngle));
            vinylLabel->setPixmap(pixmap);
        }
    });
}

DisplayWidget::~DisplayWidget()
{
}

void DisplayWidget::updateMetaData(const QString &musicFilePath, const QPixmap &albumCover)
{
    // Set album cover
    albumCoverLabel->setPixmap(albumCover.isNull() ? Utils::loadSvgAsPixmap(QStringLiteral(":/app/DefaultMusicIcon.svg"), QSize(256, 256)) : albumCover.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // Load lyrics
    QString lyricsFilePath = musicFilePath;
    lyricsFilePath.replace(QRegularExpression("\\.\\w+$"), ".lrc"); // Use QRegularExpression instead of QRegExp
    loadLyrics(lyricsFilePath);

    // Start vinyl rotation
    startVinylRotation();
}

void DisplayWidget::loadLyrics(const QString &lyricsFilePath)
{
    QFile file(lyricsFilePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        lyricsBrowser->setText(in.readAll());
        file.close();
    } else {
        lyricsBrowser->setText("Lyrics not found.");
    }
}

void DisplayWidget::startVinylRotation()
{
    rotationTimer->start(100); // Rotate every 100ms
}

void DisplayWidget::stopVinylRotation()
{
    rotationTimer->stop();
}
