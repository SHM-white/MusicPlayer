#include "widget.h"
#include "./ui_widget.h"

#include "MusicItem.h"
#include "Settings.h"
#include <QKeyEvent> // Add this include for QKeyEvent

MainWidget::MainWidget(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    setFocusPolicy(Qt::StrongFocus); // Ensure MainWidget can capture keyboard events
    setAttribute(Qt::WA_DontCreateNativeAncestors);
    setAttribute(Qt::WA_TranslucentBackground);
    installWindowAgent();
    setWindowTitle(tr("Music Player"));
    loadStyleSheet(Dark);
    setObjectName(QStringLiteral("main-window"));
    windowAgent->setWindowAttribute(QStringLiteral("dwm-blur"), true);

	// 学校: 南京航空航天大学

	// Setup icons  
    ui->pushButton_Previous         ->setFont(Icons::Font);
    ui->pushButton_Next             ->setFont(Icons::Font);
    ui->pushButton_ShowPlayList     ->setFont(Icons::Font);
    ui->pushButton_Maximize         ->setFont(Icons::Font);
    ui->pushButton_PlaySpeed        ->setFont(Icons::Font);
    ui->pushButton_showFileDetails  ->setFont(Icons::Font);
    ui->pushButton_Previous         ->setText(Icons::Get(Icons::Previous));
    ui->pushButton_Next             ->setText(Icons::Get(Icons::Next));
    ui->pushButton_ShowPlayList     ->setText(Icons::Get(Icons::BulletedList));
    ui->pushButton_Maximize         ->setText(Icons::Get(Icons::FullScreen));
    ui->pushButton_PlaySpeed        ->setText(Icons::Get(Icons::Equalizer));
    ui->pushButton_showFileDetails  ->setText(Icons::Get(Icons::More));

    m_mediaPlayer = std::make_shared<QMediaPlayer>(this);
    m_playTimer = std::make_unique<QTimer>(this);
    m_playbackTimer = std::make_unique<QTimer>(this);
    connect(ui->listWidget_PlayList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(changeMusic(QListWidgetItem*)));
    connect(m_mediaPlayer.get(), SIGNAL(positionChanged(qint64)), this, SLOT(on_positionChanged(qint64)));
    connect(ui->pushButton_Volume, SIGNAL(volumeChanged(int)), this, SLOT(on_volumeChanged(int)));
    connect(ui->pushButton_LoopMode, SIGNAL(playModeSwitched(LoopModeSwitcher::Mode)), this, SLOT(on_loopModeSwitched(LoopModeSwitcher::Mode)));
    connect(m_mediaPlayer.get(), SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(on_mediaStatusChanged(QMediaPlayer::MediaStatus)));
    connect(ui->widget_MusicDetail->lyricsView, &QListView::clicked, [this]() {
        this->setFocus();
        });
    connect(ui->listWidget_PlayList, &QListWidget::itemClicked, [this]() {
		auto timer = new QTimer(this);
		timer->singleShot(100, [this, timer]() {
			this->setFocus();
			timer->deleteLater();
			});
        });
    m_mediaPlayer->setAudioOutput(new QAudioOutput(this));
	m_autoFocusTimer = std::make_unique<QTimer>(this);
	connect(m_autoFocusTimer.get(), &QTimer::timeout, [this]() {
        if (this->isActiveWindow()) {
			this->setFocus();
        }
		});
    m_autoFocusTimer->setInterval(1000);
    m_autoFocusTimer->start();
    setAcceptDrops(true);

    // Enable context menu for listWidget_PlayList
    ui->listWidget_PlayList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget_PlayList, &QListWidget::customContextMenuRequested, this, &MainWidget::showContextMenu);
	connect(ui->pushButton_PlaySpeed, &ChangePlaybackSpeedButton::currentSpeedChanged, [this](double speed) {
		m_mediaPlayer->setPlaybackRate(speed);
        this->setFocus();
		});
	connect(ui->widget_MusicDetail, &DisplayWidget::requestJumpToTimestamp, [this](qint64 timestamp) {
		m_mediaPlayer->setPosition(timestamp);
        this->setFocus();
		});
    // Load settings and music list
    GlobalConfigs::APPLICATION_SETTINGS = ConfigManager::LoadSettings(GlobalConfigs::CONFIG_FILE_PATH()).result();
	ui->pushButton_Volume->setVolume(GlobalConfigs::APPLICATION_SETTINGS.value("volume", 100).toInt());
    ConfigManager::LoadMusicList(GlobalConfigs::LOCAL_PLAY_LIST())
        .then([&](const QStringList& r) {
        updateMusicList(r);
            });
    if (QApplication::arguments().size() > 1) {
        int musicCount = m_musicList.size();
		QStringList fileNames;
		for (int i = 1; i < QApplication::arguments().size(); ++i) {
			auto fileName = QApplication::arguments().at(i);
			fileName.replace("\\", "/");
			fileName.replace("\"", "");
			fileName.replace("'", "");
			fileNames.append(fileName);
		}
		updateMusicList(fileNames);
		changeMusic(ui->listWidget_PlayList->item(musicCount));
    }
}

// 姓名: 周奕轩

MainWidget::~MainWidget()
{
	ConfigManager::SaveSettings(GlobalConfigs::CONFIG_FILE_PATH(), GlobalConfigs::APPLICATION_SETTINGS);
    delete ui;
}

void MainWidget::updateMusicNameLabel(const QString& musicInfo, int availableWidth) {
    QFontMetrics fm{ ui->label_MusicName->font() };
    ui->label_MusicName->setText(fm.elidedText(musicInfo, Qt::ElideRight, availableWidth));
}

void MainWidget::changeMusic(QListWidgetItem* item)
{
    if (item == nullptr) {
        return;
    }
	if (std::find(m_playedMusics.begin(), m_playedMusics.end(), item) == m_playedMusics.end()) {
        while (m_playedMusics.size() > std::min(m_musicList.size() / 4, 100ll)) {
            m_playedMusics.erase(m_playedMusics.begin());
        }
		m_playedMusics.push_back(item);
	}
    this->setFocus();
    auto* i = dynamic_cast<MusicItem*>(item);
    m_playingMusicItem = item;
    ui->playPauseButton->setIsPlaying(false);
    on_playPauseButton_clicked();
    ui->listWidget_PlayList->setDisabled(true);
    ui->pushButton_Next->setDisabled(true);
    ui->pushButton_Previous->setDisabled(true);
    auto metaData = i->load(m_mediaPlayer);
    metaData.then([&](QMediaMetaData metaData) {
        static std::atomic_bool locker{ false };
        locker.wait(true);
        locker.store(true);
        if (!metaData.isEmpty()) {
            m_currentMetaData = metaData;
        }
        ui->listWidget_PlayList->update();

        auto duration = metaData.value(QMediaMetaData::Duration);
        auto title = metaData.value(QMediaMetaData::AlbumTitle);
        auto artist = metaData.value(QMediaMetaData::AlbumArtist);

        ui->horizontalSlider_Progress->setValue(0);
        if (!metaData.isEmpty()) {
            auto a = duration.toInt();
            ui->horizontalSlider_Progress->setMaximum(a);
            ui->playPauseButton->setIsPlaying(true);
            m_playTimer->singleShot(3, this, SLOT(on_playPauseButton_clicked(void)));
            if (title.isNull() && artist.isNull()) {
                m_currentMusicInfo = dynamic_cast<MusicItem*>(m_playingMusicItem)->url.fileName();
            }
            else {
                m_currentMusicInfo = title.toString() + QStringLiteral(" - ") + artist.toString();
            }
            int availableWidth = std::max(100, ui->horizontalLayout_5->geometry().width() - 20);
            updateMusicNameLabel(m_currentMusicInfo, availableWidth);
        }
        m_playbackTimer->singleShot(6, this, SLOT(on_enableListWidget(void)));
        locker.store(false);
        locker.notify_one();
    });
}

void MainWidget::on_volumeChanged(int value)
{
    m_mediaPlayer->audioOutput()->setVolume((float)value / 100.0);
    GlobalConfigs::APPLICATION_SETTINGS["volume"] = value;
}

void MainWidget::installWindowAgent()
{
    // 1. Setup window agent
    windowAgent = new QWK::WidgetWindowAgent(this);
    windowAgent->setup(this);

    // 2. Construct your title bar
    auto menuBar = [this]() {
        auto menuBar = new QMenuBar(this);

        //// Virtual menu
        auto file = new QMenu(tr("File(&F)"), menuBar);
        //file->addAction(new QAction(tr("New(&N)"), menuBar));
        auto openAction = new QAction(tr("Open(&O)"), menuBar);
		connect(openAction, &QAction::triggered, this, [this]() {
			auto fileName = QFileDialog::getOpenFileNames(this, tr("Open File"), QString(), tr("Audio Files (*.mp3 *.wav *.flac *.ogg *.wma *.aac)"));
			if (!fileName.isEmpty()) {
				updateMusicList(fileName);
			}
			});
        file->addAction(openAction);
        //file->addSeparator();

        //auto edit = new QMenu(tr("Edit(&E)"), menuBar);
        //edit->addAction(new QAction(tr("Undo(&U)"), menuBar));
        //edit->addAction(new QAction(tr("Redo(&R)"), menuBar));

        // Theme action
        auto darkAction = new QAction(tr("Enable dark theme"), menuBar);
        darkAction->setCheckable(true);
        connect(darkAction, &QAction::triggered, this, [this](bool checked) {
            loadStyleSheet(checked ? Dark : Light); //
            });
        connect(this, &MainWidget::themeChanged, darkAction, [this, darkAction]() {
            darkAction->setChecked(currentTheme == Dark); //
            });

		auto moreSettings = new QAction(tr("More Settings"), menuBar);
        connect(moreSettings, &QAction::triggered, [this]() {
            static Settings *settingsWidget = nullptr;
            if (settingsWidget == nullptr) {
				settingsWidget = new Settings(this);
                
            }
            settingsWidget->show();
            });

#ifdef Q_OS_WIN
        auto noneAction = new QAction(tr("None"), menuBar);
        noneAction->setData(QStringLiteral("none"));
        noneAction->setCheckable(true);
        //noneAction->setChecked(true);

        auto dwmBlurAction = new QAction(tr("Enable DWM blur"), menuBar);
        dwmBlurAction->setData(QStringLiteral("dwm-blur"));
        dwmBlurAction->setCheckable(true);
        dwmBlurAction->setChecked(true);

        auto acrylicAction = new QAction(tr("Enable acrylic material"), menuBar);
        acrylicAction->setData(QStringLiteral("acrylic-material"));
        acrylicAction->setCheckable(true);
        //acrylicAction->setChecked(true);

        auto micaAction = new QAction(tr("Enable mica"), menuBar);
        micaAction->setData(QStringLiteral("mica"));
        micaAction->setCheckable(true);

        auto micaAltAction = new QAction(tr("Enable mica alt"), menuBar);
        micaAltAction->setData(QStringLiteral("mica-alt"));
        micaAltAction->setCheckable(true);

        auto winStyleGroup = new QActionGroup(menuBar);
        winStyleGroup->addAction(noneAction);
        winStyleGroup->addAction(dwmBlurAction);
        winStyleGroup->addAction(acrylicAction);
        winStyleGroup->addAction(micaAction);
        winStyleGroup->addAction(micaAltAction);
        connect(winStyleGroup, &QActionGroup::triggered, this,
            [this, winStyleGroup](QAction* action) {
                // Unset all custom style attributes first, otherwise the style will not display
                // correctly
                for (const QAction* _act : winStyleGroup->actions()) {
                    const QString data = _act->data().toString();
                    if (data.isEmpty() || data == QStringLiteral("none")) {
                        continue;
                    }
                    windowAgent->setWindowAttribute(data, false);
                }
                const QString data = action->data().toString();
                if (data == QStringLiteral("none")) {
                    setProperty("custom-style", false);
                    m_paintTransparentBackground = false;
                }
                else if (!data.isEmpty()) {
                    windowAgent->setWindowAttribute(data, true);
                    setProperty("custom-style", true);
                    m_paintTransparentBackground = true;
                }
                style()->polish(this);
                update();
            });

#elif defined(Q_OS_MAC)
        auto darkBlurAction = new QAction(tr("Dark blur"), menuBar);
        darkBlurAction->setCheckable(true);
        connect(darkBlurAction, &QAction::toggled, this, [this](bool checked) {
            if (!windowAgent->setWindowAttribute(QStringLiteral("blur-effect"), "dark")) {
                return;
            }
            if (checked) {
                setProperty("custom-style", true);
                style()->polish(this);
            }
            });

        auto lightBlurAction = new QAction(tr("Light blur"), menuBar);
        lightBlurAction->setCheckable(true);
        connect(lightBlurAction, &QAction::toggled, this, [this](bool checked) {
            if (!windowAgent->setWindowAttribute(QStringLiteral("blur-effect"), "light")) {
                return;
            }
            if (checked) {
                setProperty("custom-style", true);
                style()->polish(this);
            }
            });

        auto noBlurAction = new QAction(tr("No blur"), menuBar);
        noBlurAction->setCheckable(true);
        connect(noBlurAction, &QAction::toggled, this, [this](bool checked) {
            if (!windowAgent->setWindowAttribute(QStringLiteral("blur-effect"), "none")) {
                return;
            }
            if (checked) {
                setProperty("custom-style", false);
                style()->polish(this);
            }
            });

        auto macStyleGroup = new QActionGroup(menuBar);
        macStyleGroup->addAction(darkBlurAction);
        macStyleGroup->addAction(lightBlurAction);
        macStyleGroup->addAction(noBlurAction);
#endif

        // Real menu
        auto settings = new QMenu(tr("Settings(&S)"), menuBar);
		settings->addAction(moreSettings);
        settings->addSeparator();
        settings->addAction(darkAction);

#ifdef Q_OS_WIN
        settings->addSeparator();
        settings->addAction(noneAction);
        settings->addAction(dwmBlurAction);
        settings->addAction(acrylicAction);
        //settings->addAction(micaAction);
        //settings->addAction(micaAltAction);
#elif defined(Q_OS_MAC)
        settings->addAction(darkBlurAction);
        settings->addAction(lightBlurAction);
        settings->addAction(noBlurAction);
#endif

        menuBar->addMenu(file);
        //menuBar->addMenu(edit);
        menuBar->addMenu(settings);
        return menuBar;
        }();
    menuBar->setObjectName(QStringLiteral("win-menu-bar"));

    auto titleLabel = new QLabel();
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setObjectName(QStringLiteral("win-title-label"));

#ifndef Q_OS_MAC
    auto iconButton = new QWK::WindowButton();
    iconButton->setObjectName(QStringLiteral("icon-button"));
    iconButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    auto pinButton = new QWK::WindowButton();
    pinButton->setCheckable(true);
    pinButton->setObjectName(QStringLiteral("pin-button"));
    pinButton->setProperty("system-button", true);
    pinButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    auto minButton = new QWK::WindowButton();
    minButton->setObjectName(QStringLiteral("min-button"));
    minButton->setProperty("system-button", true);
    minButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    auto maxButton = new QWK::WindowButton();
    maxButton->setCheckable(true);
    maxButton->setObjectName(QStringLiteral("max-button"));
    maxButton->setProperty("system-button", true);
    maxButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    auto closeButton = new QWK::WindowButton();
    closeButton->setObjectName(QStringLiteral("close-button"));
    closeButton->setProperty("system-button", true);
    closeButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
#endif

    auto windowBar = new QWK::WindowBar();
#ifndef Q_OS_MAC
    windowBar->setIconButton(iconButton);
    windowBar->setPinButton(pinButton);
    windowBar->setMinButton(minButton);
    windowBar->setMaxButton(maxButton);
    windowBar->setCloseButton(closeButton);
#endif
    windowBar->setMenuBar(menuBar);
    windowBar->setTitleLabel(titleLabel);
    windowBar->setHostWidget(this);

    windowAgent->setTitleBar(windowBar);
#ifndef Q_OS_MAC
    windowAgent->setHitTestVisible(pinButton, true);
    windowAgent->setSystemButton(QWK::WindowAgentBase::WindowIcon, iconButton);
    windowAgent->setSystemButton(QWK::WindowAgentBase::Minimize, minButton);
    windowAgent->setSystemButton(QWK::WindowAgentBase::Maximize, maxButton);
    windowAgent->setSystemButton(QWK::WindowAgentBase::Close, closeButton);
#endif
    windowAgent->setHitTestVisible(menuBar, true);

#ifdef Q_OS_MAC
    windowAgent->setSystemButtonAreaCallback([](const QSize& size) {
        static constexpr const int width = 75;
        return QRect(QPoint(size.width() - width, 0), QSize(width, size.height())); //
        });
#endif

    setMenuWidget(windowBar);


#ifndef Q_OS_MAC
    connect(windowBar, &QWK::WindowBar::pinRequested, this, [this, pinButton](bool pin) {
        if (isHidden() || isMinimized() || isMaximized() || isFullScreen()) {
            return;
        }
        setWindowFlag(Qt::WindowStaysOnTopHint, pin);
        show();
        pinButton->setChecked(pin);
        });
    connect(windowBar, &QWK::WindowBar::minimizeRequested, this, &QWidget::showMinimized);
    connect(windowBar, &QWK::WindowBar::maximizeRequested, this, [this, maxButton](bool max) {
        if (max) {
            showMaximized();
        }
        else {
            showNormal();
        }

        // It's a Qt issue that if a QAbstractButton::clicked triggers a window's maximization,
        // the button remains to be hovered until the mouse move. As a result, we need to
        // manually send leave events to the button.
        emulateLeaveEvent(maxButton);
        });
    connect(windowBar, &QWK::WindowBar::closeRequested, this, &QWidget::close);
#endif
}

void MainWidget::loadStyleSheet(Theme theme)
{
    if (!styleSheet().isEmpty() && theme == currentTheme)
        return;
    currentTheme = theme;

    if (QFile qss(theme == Dark ? QStringLiteral(":/dark-style.qss")
        : QStringLiteral(":/light-style.qss"));
        qss.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setStyleSheet(QString::fromUtf8(qss.readAll()));
        windowAgent->setWindowAttribute(QStringLiteral("none"), false);
        setProperty("custom-style", true);
        style()->polish(this);
        Q_EMIT themeChanged(theme);
        update();
    }
}

bool MainWidget::event(QEvent* event)
{
    switch (event->type()) {
    case QEvent::WindowActivate: {
        auto menu = menuWidget();
        if (menu) {
            menu->setProperty("bar-active", true);
            style()->polish(menu);
        }
        break;
    }

    case QEvent::WindowDeactivate: {
        auto menu = menuWidget();
        if (menu) {
            menu->setProperty("bar-active", false);
            style()->polish(menu);
        }
        break;
    }

    default:
        break;
    }
    return QMainWindow::event(event);
}

void MainWidget::paintEvent(QPaintEvent* event)
{
    QRect newRect = this->rect();
    newRect.setBottomRight(this->rect().bottomRight() + QPoint{ 2, 2 });
    newRect.setTopLeft(QPoint{ -1, -1 });
    QPainter painter(this);
    if(currentTheme == Dark)
    {
        painter.setBrush(QBrush(QColor(0, 0, 0, m_paintTransparentBackground ? 128 : 255)));
        painter.drawRect(newRect);
    }
    else
    {
        painter.setBrush(QColor(240, 240, 240, m_paintTransparentBackground ? 128 : 255));
        painter.drawRect(newRect);
    }
    painter.end();
    QMainWindow::paintEvent(event);
}

void MainWidget::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);

    int availableWidth = std::max(100, ui->horizontalLayout_5->geometry().width() - 20);
    updateMusicNameLabel(m_currentMusicInfo, availableWidth);
}

void MainWidget::updateTimeLabel(qint64 current, qint64 total)
{
    QTime c(0, 0);
    c = c.addMSecs(current);
    QTime t(0, 0);
    t = t.addMSecs(total);
    ui->label_PlayTime->setText(Utils::QTimeToQString(c) + "/" + Utils::QTimeToQString(t));
}

void MainWidget::updateMusicList(const QStringList& list) {
    // 将操作放入后台线程
    QtConcurrent::run([this, list]() {
		static std::atomic_bool locker{ false };
		locker.wait(true);
        QStringList newMusicList = m_musicList;
        newMusicList.append(list);

        // 更新 UI 必须在主线程中完成
        QMetaObject::invokeMethod(this, [this, list]() {
            for (const auto& i : list) {
                ui->listWidget_PlayList->addItem(new MusicItem{ ui->listWidget_PlayList, i });
            }
            }, Qt::QueuedConnection);

        // 更新成员变量
        QMetaObject::invokeMethod(this, [this, newMusicList]() {
            m_musicList = newMusicList;
            ConfigManager::SaveLoadedMusicList(newMusicList);
            }, Qt::QueuedConnection);
		locker.store(false);
		locker.notify_one();
        });

}

void MainWidget::on_pushButton_ShowPlayList_clicked()
{
    updateMusicNameLabel(m_currentMusicInfo, 100);
    ui->listWidget_PlayList->setVisible(!ui->listWidget_PlayList->isVisible());
    int availableWidth = std::max(100, ui->horizontalLayout_5->geometry().width() - 20);
    updateMusicNameLabel(m_currentMusicInfo, availableWidth);
}

void MainWidget::on_positionChanged(qint64 value)
{
    if (!ui->horizontalSlider_Progress->isSliderDown()) {
        ui->horizontalSlider_Progress->setValue(value);
        updateTimeLabel(value, ui->horizontalSlider_Progress->maximum());
        ui->widget_MusicDetail->updateHighlightedLyric(value);
    }
}

void MainWidget::on_playPauseButton_clicked()
{
    if (m_mediaPlayer->isAvailable()) {
        ui->playPauseButton->isPlaying() ? m_mediaPlayer->play() : m_mediaPlayer->pause();
    }
    return;
}

void MainWidget::on_horizontalSlider_Progress_valueChanged(int value)
{
    if (ui->horizontalSlider_Progress->isSliderDown()) {
        m_mediaPlayer->setPosition(value);
        updateTimeLabel(value, ui->horizontalSlider_Progress->maximum());
    }
}

void MainWidget::on_horizontalSlider_Progress_sliderReleased()
{
    int value = ui->horizontalSlider_Progress->value();
    m_mediaPlayer->setPosition(value);
    updateTimeLabel(value, ui->horizontalSlider_Progress->maximum());
}

void MainWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
        // 项目: 2025 C++程序设计课程设计
    }
}

void MainWidget::dropEvent(QDropEvent *event)
{
    const auto urls = event->mimeData()->urls();
    QStringList list;
    auto isAudioFile = [](const QFileInfo& fileInfo) {
        static const QStringList audioFileTypes = { "mp3", "flac", "wav", "ogg", "aac", "wma" };
        return fileInfo.exists() && fileInfo.isFile() && (audioFileTypes.contains(fileInfo.suffix().toLower()));
        };
    for (const QUrl &url : urls) {
        QFileInfo fileInfo(url.toLocalFile());
        if (isAudioFile(fileInfo)) {
            list.append(fileInfo.absoluteFilePath());
        }
    }
    updateMusicList(list);
}

void MainWidget::showContextMenu(const QPoint &pos)
{
    QListWidgetItem *item = ui->listWidget_PlayList->itemAt(pos);
    if (!item) return;

    QMenu contextMenu(this);
    QAction *deleteAction = contextMenu.addAction(tr("Delete"));
    QAction* deleteAllAction = contextMenu.addAction(tr("Delete All"));
    connect(deleteAction, &QAction::triggered, this, [this, item]() {
        removeSelectedItem(item);
    });
	connect(deleteAllAction, &QAction::triggered, this, [this]() {
		m_musicList.clear();
		ui->listWidget_PlayList->clear();
		ConfigManager::SaveLoadedMusicList(m_musicList);
		});
    contextMenu.exec(ui->listWidget_PlayList->mapToGlobal(pos));
}

void MainWidget::removeSelectedItem(QListWidgetItem *item)
{
    int row = ui->listWidget_PlayList->row(item);
    if (row >= 0) {
        m_musicList.removeAt(row);
        delete ui->listWidget_PlayList->takeItem(row);
    }
    ConfigManager::SaveLoadedMusicList(m_musicList);
}

void MainWidget::on_loopModeSwitched(LoopModeSwitcher::Mode mode)
{
    m_loopMode = mode;
    switch (mode)
    {
    case LoopModeSwitcher::ListLoop:
        m_mediaPlayer->setLoops(QMediaPlayer::Once);
        break;
    case LoopModeSwitcher::SingleLoop:
        m_mediaPlayer->setLoops(QMediaPlayer::Infinite);
        break;
    case LoopModeSwitcher::RandomPlay:
        m_mediaPlayer->setLoops(QMediaPlayer::Once);
        break;
    default:
        break;
    }
}

void MainWidget::on_mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia) {
        on_pushButton_Next_clicked();
    }
}

void MainWidget::on_enableListWidget()
{
    ui->listWidget_PlayList->setEnabled(true);
    ui->pushButton_Next->setEnabled(true);
    ui->pushButton_Previous->setEnabled(true);
    QString musicFilePath = dynamic_cast<MusicItem*>(m_playingMusicItem)->url.toString();
    QPixmap albumCover = m_currentMetaData.value(QMediaMetaData::CoverArtImage).value<QPixmap>();
    ui->widget_MusicDetail->updateMetaData(musicFilePath, albumCover);

}


void MainWidget::on_pushButton_Previous_clicked()
{
    int currentMusicRow = -1;
    if (m_playingMusicItem != nullptr) {
        currentMusicRow = ui->listWidget_PlayList->row(m_playingMusicItem);
    }
    switch (m_loopMode)
    {
    case LoopModeSwitcher::SingleLoop:
    case LoopModeSwitcher::ListLoop:
    {
        if (m_playingMusicItem == nullptr) break;
        auto nextMusic = (currentMusicRow - 1) < 0 ? ui->listWidget_PlayList->item(ui->listWidget_PlayList->count() - 1) : ui->listWidget_PlayList->item(currentMusicRow - 1);
        if (nextMusic == nullptr) break;
        ui->listWidget_PlayList->setCurrentItem(nextMusic);
        changeMusic(nextMusic);
    }
        break;
    case LoopModeSwitcher::RandomPlay:
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, ui->listWidget_PlayList->count() - 1);
        int peddingMusicRow;
        while ((peddingMusicRow = distrib(gen)) == currentMusicRow || (std::find(m_playedMusics.begin(), m_playedMusics.end(), ui->listWidget_PlayList->item(peddingMusicRow)) != m_playedMusics.end())) {};
        ui->listWidget_PlayList->setCurrentRow(peddingMusicRow);
        changeMusic(ui->listWidget_PlayList->item(peddingMusicRow));
    }
        break;
    default:
        break;
    }

}


void MainWidget::on_pushButton_Next_clicked()
{
    int currentMusicRow = -1;
    if (m_playingMusicItem != nullptr) {
        currentMusicRow = ui->listWidget_PlayList->row(m_playingMusicItem);
    }
    switch (m_loopMode)
    {
    case LoopModeSwitcher::SingleLoop:
    case LoopModeSwitcher::ListLoop:
    {
        //if (m_playingMusicItem == nullptr) break;
        auto nextMusic = (currentMusicRow + 1) >= ui->listWidget_PlayList->count() ? ui->listWidget_PlayList->item(0) : ui->listWidget_PlayList->item(currentMusicRow + 1);
        if (nextMusic == nullptr) break;
        ui->listWidget_PlayList->setCurrentItem(nextMusic);
        changeMusic(nextMusic);
    }
        break;
    case LoopModeSwitcher::RandomPlay:
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, ui->listWidget_PlayList->count() - 1);
        int peddingMusicRow;
        while ((peddingMusicRow = distrib(gen)) == currentMusicRow || (std::find(m_playedMusics.begin(), m_playedMusics.end(), ui->listWidget_PlayList->item(peddingMusicRow)) != m_playedMusics.end())) {};
        ui->listWidget_PlayList->setCurrentRow(peddingMusicRow);
        changeMusic(ui->listWidget_PlayList->item(peddingMusicRow));
    }
        break;
    default:
        break;
    }

}

void MainWidget::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Space:
        // Toggle play/pause when spacebar is pressed
        ui->playPauseButton->setIsPlaying(!ui->playPauseButton->isPlaying());
        on_playPauseButton_clicked();
        event->accept();
        break;
	case Qt::Key_Left:
        m_mediaPlayer->setPosition(std::max(m_mediaPlayer->position() - 5000, 0ll)); // Rewind 5 seconds
        break;
	case Qt::Key_Right:
		m_mediaPlayer->setPosition(std::min(m_mediaPlayer->position() + 5000, m_mediaPlayer->duration())); // Forward 5 seconds
		break;
    case Qt::Key_Delete:
		// Delete the selected item when Delete key is pressed
		if (ui->listWidget_PlayList->currentItem()) {
			removeSelectedItem(ui->listWidget_PlayList->currentItem());
		}
		event->accept();
		break;
    default:
        QMainWindow::keyPressEvent(event); // Pass unhandled events to the base class
        break;
    }
}


void MainWidget::on_pushButton_Maximize_clicked()
{
    if (this->isFullScreen()) {
        this->showNormal();
    } else {
        this->showFullScreen();
    }
}


void MainWidget::on_pushButton_showFileDetails_clicked()
{
	if (m_currentMetaData.isEmpty()) return;
	auto task = QtConcurrent::run([this](const QMediaMetaData data) {
        QString details;
		auto KeyToQString = [](QMediaMetaData::Key key) {
            switch (key)
            {
            // 学号: 082420122
            case QMediaMetaData::Title:
				return QStringLiteral("Title");
                break;
            case QMediaMetaData::Author:
				return QStringLiteral("Author");
                break;
            case QMediaMetaData::Comment:
				return QStringLiteral("Comment");
                break;
            case QMediaMetaData::Description:
				return QStringLiteral("Description");
                break;
            case QMediaMetaData::Genre:
				return QStringLiteral("Genre");
                break;
            case QMediaMetaData::Date:
				return QStringLiteral("Date");
                break;
            case QMediaMetaData::Language:
				return QStringLiteral("Language");
                break;
            case QMediaMetaData::Publisher:
				return QStringLiteral("Publisher");
                break;
            case QMediaMetaData::Copyright:
				return QStringLiteral("Copyright");
                break;
            case QMediaMetaData::Url:
				return QStringLiteral("Url");
                break;
            case QMediaMetaData::Duration:
				return QStringLiteral("Duration");
                break;
            case QMediaMetaData::MediaType:
				return QStringLiteral("MediaType");
                break;
            case QMediaMetaData::FileFormat:
				return QStringLiteral("FileFormat");
                break;
            case QMediaMetaData::AudioBitRate:
				return QStringLiteral("AudioBitRate");
                break;
            case QMediaMetaData::AudioCodec:
				return QStringLiteral("AudioCodec");
                break;
            case QMediaMetaData::VideoBitRate:
				return QStringLiteral("VideoBitRate");
                break;
            case QMediaMetaData::VideoCodec:
				return QStringLiteral("VideoCodec");
                break;
            case QMediaMetaData::VideoFrameRate:
				return QStringLiteral("VideoFrameRate");
                break;
            case QMediaMetaData::AlbumTitle:
				return QStringLiteral("AlbumTitle");
                break;
            case QMediaMetaData::AlbumArtist:
				return QStringLiteral("AlbumArtist");
                break;
            case QMediaMetaData::ContributingArtist:
				return QStringLiteral("ContributingArtist");
                break;
            case QMediaMetaData::TrackNumber:
				return QStringLiteral("TrackNumber");
                break;
            case QMediaMetaData::Composer:
				return QStringLiteral("Composer");
                break;
            case QMediaMetaData::LeadPerformer:
				return QStringLiteral("LeadPerformer");
                break;
            case QMediaMetaData::ThumbnailImage:
				return QStringLiteral("ThumbnailImage");
                break;
            case QMediaMetaData::CoverArtImage:
				return QStringLiteral("CoverArtImage");
                break;
            case QMediaMetaData::Orientation:
				return QStringLiteral("Orientation");
                break;
            case QMediaMetaData::Resolution:
				return QStringLiteral("Resolution");
                break;
            case QMediaMetaData::HasHdrContent:
				return QStringLiteral("HasHdrContent");
                break;
            default:
				return QStringLiteral("Other");
                break;
            }
			};
        for (auto& i : data.keys()) {
            details += KeyToQString(i) + ": " + data.value(i).toString() + "\n";
        }
        QMetaObject::invokeMethod(this, [this, details]() {
            // Show the details in a message box
            QMessageBox::information(this, tr("File Details"), details);
			}, Qt::QueuedConnection);
        }, m_currentMetaData);
}

