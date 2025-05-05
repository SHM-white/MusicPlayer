#pragma once

#include "includeFiles.h"

class DisplayWidget : public QWidget
{
    Q_OBJECT

public:
    DisplayWidget(QWidget *parent = nullptr);
    ~DisplayWidget();

    void updateMetaData(const QString &musicFilePath, const QPixmap &albumCover);

private:
    void loadLyrics(const QString &lyricsFilePath);
    void startVinylRotation();
    void stopVinylRotation();

    void parseLrcFile(const QString &lyricsFilePath); // New method for parsing LRC files
    void updateHighlightedLyric(qint64 currentTime); // New method for updating highlighted lyric

    QLabel *vinylLabel;
    QLabel *albumCoverLabel;
    QTextBrowser *lyricsBrowser;
    QTimer *rotationTimer;
    QTimer *lyricUpdateTimer; // New timer for updating lyrics
    int rotationAngle;

    QVector<QPair<qint64, QString>> lyricsData; // Stores parsed LRC data
    int currentLyricIndex; // Tracks the currently highlighted lyric
};
