#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "BasicWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWidget;
}
QT_END_NAMESPACE

namespace QWK {
    class WidgetWindowAgent;
    class StyleAgent;
}

class MainWidget : public QMainWindow
{
    Q_OBJECT

public:
    enum Theme {
        Dark,
        Light,
    };
    Q_ENUM(Theme)
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();
    Theme currentTheme{};
    QWK::WidgetWindowAgent *windowAgent;

Q_SIGNALS:
    void themeChanged();

protected Q_SLOTS:
    void updateMusicList(); 
    void changeMusic(QListWidgetItem* item);
    void on_volumeChanged(int value);
    void on_positionChanged(qint64 value);
private slots:
    void on_pushButton_ShowPlayList_clicked();
    void on_playPauseButton_clicked();

    void on_horizontalSlider_Progress_valueChanged(int value);
    void on_horizontalSlider_Progress_sliderReleased(); // New slot for slider release

private:
    void installWindowAgent();
    void loadStyleSheet(Theme theme);
    bool event(QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void updateTimeLabel(qint64 current, qint64 total);
    Ui::MainWidget *ui;
    bool m_paintTransparentBackground{ true };
    std::shared_ptr<QMediaPlayer> m_mediaPlayer;
    QMediaMetaData m_currentMetaData;
    std::unique_ptr<QTimer> m_playTimer;
#ifdef DEBUG
    QStringList m_musicList{ "D:/Music/Music_9/Krimsonn - Stranger.MP3",
    "D:/Music/Music_9/Manafest - Edge of My Life.mp3" };
#else
    QStringList m_musicList;
#endif // DEBUG

};
#endif // WIDGET_H
