﻿#pragma once

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
    QListView* lyricsView; // Replace QTextBrowser with QListView

signals:
    void requestJumpToTimestamp(qint64 timestamp); // Signal to request a jump to a specific timestamp

protected:
    void resizeEvent(QResizeEvent *event) override; // Override resizeEvent

public slots:
    void updateHighlightedLyric(qint64 currentTime); // New method for updating highlighted lyric

private slots:
    void handleLyricDoubleClicked(const QModelIndex &index); // Slot to handle double-click events

private:
    void adjustLyricsMaxWidth(); // Adjust maximum width for lyrics

    QVector<QPair<qint64, QString>> lyricsData; // Stores parsed LRC data
    int currentLyricIndex; // Tracks the currently highlighted lyric
    LyricsDelegate *lyricsDelegate; // Custom delegate for styling lyrics
};
