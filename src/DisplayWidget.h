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

    QLabel *vinylLabel;
    QLabel *albumCoverLabel;
    QTextBrowser *lyricsBrowser;
    QTimer *rotationTimer;
    int rotationAngle;
};
