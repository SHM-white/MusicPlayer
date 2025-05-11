#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "BasicWidget.h"
#include "ConfigManager.h"
#include "LoopModeSwitcher.h"
#include "DisplayWidget.h"
#include "includeFiles.h"

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
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();
    Theme currentTheme{};
    QWK::WidgetWindowAgent *windowAgent;

Q_SIGNALS:
    void themeChanged(Theme t);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void keyPressEvent(QKeyEvent* event) override;

protected Q_SLOTS:
    void updateMusicList(const QStringList& list);
    void changeMusic(QListWidgetItem* item);
    void on_volumeChanged(int value);
    void on_positionChanged(qint64 value);
private slots:
    void on_pushButton_ShowPlayList_clicked();
    void on_playPauseButton_clicked();

    void on_horizontalSlider_Progress_valueChanged(int value);
    void on_horizontalSlider_Progress_sliderReleased(); // New slot for slider release
    void showContextMenu(const QPoint &pos);
    void removeSelectedItem(QListWidgetItem *item);
    void on_loopModeSwitched(LoopModeSwitcher::Mode mode);
    void on_mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void on_enableListWidget();
    void on_pushButton_Previous_clicked();
    void on_pushButton_Next_clicked();

    void on_pushButton_Maximize_clicked();

    void on_pushButton_showFileDetails_clicked();

private:
    void installWindowAgent();
    void loadStyleSheet(Theme theme);
    bool event(QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void updateTimeLabel(qint64 current, qint64 total);
    void updateMusicNameLabel(const QString& musicInfo, int availableWidth);
    Ui::MainWidget *ui;
    bool m_paintTransparentBackground{ true };
    std::shared_ptr<QMediaPlayer> m_mediaPlayer;
    QMediaMetaData m_currentMetaData;
    std::unique_ptr<QTimer> m_playTimer;
    QString m_currentMusicInfo{ QStringLiteral("...") };
    QListWidgetItem* m_playingMusicItem{ nullptr };
    QStringList m_musicList;
    LoopModeSwitcher::Mode m_loopMode = LoopModeSwitcher::Mode::ListLoop;
    std::unique_ptr<QTimer> m_playbackTimer;
    std::unique_ptr<QTimer> m_autoFocusTimer;
    std::vector<QListWidgetItem*> m_playedMusics;
};
#endif // WIDGET_H
