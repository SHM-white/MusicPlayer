#include "DisplayWidget.h"

DisplayWidget::DisplayWidget(QWidget *parent)
    : QWidget(parent), rotationAngle(0), currentLyricIndex(-1)
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
    connect(rotationTimer, &QTimer::timeout, [this]() {
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

void DisplayWidget::loadLyrics(const QString& lyricsFilePath)
{
    parseLrcFile(lyricsFilePath);

    if (lyricsData.isEmpty()) {
        lyricsBrowser->setText("Lyrics not found.");
    }
    else {
        QString allLyrics;
        for (const auto& lyric : lyricsData) {
            allLyrics += lyric.second + "\n";
        }
        lyricsBrowser->setText(allLyrics);
    }

    // Start lyric update timer
    lyricUpdateTimer = new QTimer(this);
    //connect(lyricUpdateTimer, &QTimer::timeout, this, [this]() {
    //    // Replace with actual playback time retrieval logic
    //    qint64 currentTime = /* Get current playback time in milliseconds */;
    //    updateHighlightedLyric(currentTime);
    //    });
    lyricUpdateTimer->start(100); // Update every 100ms
}

void DisplayWidget::parseLrcFile(const QString& lyricsFilePath)
{
    lyricsData.clear();
    QFile file(lyricsFilePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QRegularExpression regex(R"(\[(\d+):(\d+)\.(\d+)\](.*))"); // Matches [mm:ss.ms]lyric
        while (!in.atEnd()) {
            QString line = in.readLine();
            QRegularExpressionMatch match = regex.match(line);
            if (match.hasMatch()) {
                qint64 minutes = match.captured(1).toInt();
                qint64 seconds = match.captured(2).toInt();
                qint64 milliseconds = match.captured(3).toInt();
                qint64 timestamp = (minutes * 60 + seconds) * 1000 + milliseconds;
                QString lyric = match.captured(4).trimmed();
                lyricsData.append(qMakePair(timestamp, lyric));
            }
        }
        file.close();
    }
}

void DisplayWidget::updateHighlightedLyric(qint64 currentTime)
{
    if (lyricsData.isEmpty()) return;

    // Find the current lyric index
    int newIndex = -1;
    for (int i = 0; i < lyricsData.size(); ++i) {
        if (currentTime < lyricsData[i].first) break;
        newIndex = i;
    }

    if (newIndex != currentLyricIndex) {
        currentLyricIndex = newIndex;

        // Highlight the current lyric
        QTextCursor cursor = lyricsBrowser->textCursor();
        cursor.movePosition(QTextCursor::Start);
        for (int i = 0; i <= currentLyricIndex; ++i) {
            cursor.movePosition(QTextCursor::Down);
        }
        lyricsBrowser->setTextCursor(cursor);

        // Center the current lyric
        lyricsBrowser->ensureCursorVisible();
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
