#pragma once

#include "includeFiles.h"
#include "LyricsDelegate.h" // Include the custom delegate header
#include <QVector>
#include <QPair>

class DisplayWidget : public QWidget
{
    Q_OBJECT

public:
    DisplayWidget(QWidget *parent = nullptr);
    ~DisplayWidget();

    void updateMetaData(const QString &musicFilePath, const QPixmap &albumCover);
    void loadLyrics(const QString &lyricsFilePath);

protected:
    void resizeEvent(QResizeEvent *event) override; // Override resizeEvent

public slots:
    void updateHighlightedLyric(qint64 currentTime); // New method for updating highlighted lyric

private:
    void parseLrcFile(const QString &lyricsFilePath); // New method for parsing LRC files
    void adjustLyricsMaxWidth(); // Adjust maximum width for lyrics

    QListView *lyricsView; // Replace QTextBrowser with QListView

    QVector<QPair<qint64, QString>> lyricsData; // Stores parsed LRC data
    int currentLyricIndex; // Tracks the currently highlighted lyric

    LyricsDelegate *lyricsDelegate; // Custom delegate for styling lyrics
};
