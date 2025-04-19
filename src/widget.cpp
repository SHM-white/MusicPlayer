#include "widget.h"
#include "./ui_widget.h"
#include "MusicItem.h"
#include <QMenu>
#include <QAction>

MainWidget::MainWidget(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DontCreateNativeAncestors);
    setAttribute(Qt::WA_TranslucentBackground);
    installWindowAgent();
    setWindowTitle(tr("Music Player"));
    loadStyleSheet(Dark);
    setObjectName(QStringLiteral("main-window"));
    windowAgent->setWindowAttribute(QStringLiteral("dwm-blur"), true);
    ui->pushButton_Previous         ->setFont(Icons::Font);
    ui->pushButton_Next             ->setFont(Icons::Font);
    ui->pushButton_ShowPlayList     ->setFont(Icons::Font);
    ui->pushButton_Maximize         ->setFont(Icons::Font);
    ui->pushButton_Like             ->setFont(Icons::Font);
    ui->pushButton_showFileDetails  ->setFont(Icons::Font);
    ui->pushButton_Previous         ->setText(Icons::Get(Icons::Previous));
    ui->pushButton_Next             ->setText(Icons::Get(Icons::Next));
    ui->pushButton_ShowPlayList     ->setText(Icons::Get(Icons::BulletedList));
    ui->pushButton_Maximize         ->setText(Icons::Get(Icons::FullScreen));
    ui->pushButton_Like             ->setText(Icons::Get(Icons::FavoriteStar));
    ui->pushButton_showFileDetails  ->setText(Icons::Get(Icons::More));

    m_mediaPlayer = std::make_shared<QMediaPlayer>(this);
    m_playTimer = std::make_unique<QTimer>(this);
    m_playbackTimer = std::make_unique<QTimer>(this);
    connect(ui->listWidget_PlayList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(changeMusic(QListWidgetItem*)));
    //connect(ui->listWidget_PlayList, SIGNAL(itemEntered(QListWidgetItem*)), this, SLOT(changeMusic(QListWidgetItem*)));
    connect(m_mediaPlayer.get(), SIGNAL(positionChanged(qint64)), this, SLOT(on_positionChanged(qint64)));
    connect(ui->pushButton_Volume, SIGNAL(volumeChanged(int)), this, SLOT(on_volumeChanged(int)));
    connect(ui->pushButton_LoopMode, SIGNAL(playModeSwitched(LoopModeSwitcher::Mode)), this, SLOT(on_loopModeSwitched(LoopModeSwitcher::Mode)));
    connect(m_mediaPlayer.get(), SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(on_mediaStatusChanged(QMediaPlayer::MediaStatus)));
    m_mediaPlayer->setAudioOutput(new QAudioOutput(this));

    setAcceptDrops(true);
    ConfigManager::LoadMusicList()
        .then([&](const QStringList& r) {
        updateMusicList(r);
            });

    // Enable context menu for listWidget_PlayList
    ui->listWidget_PlayList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget_PlayList, &QListWidget::customContextMenuRequested, this, &MainWidget::showContextMenu);
}

MainWidget::~MainWidget()
{
    delete ui;
}

static inline void emulateLeaveEvent(QWidget* widget) {
    Q_ASSERT(widget);
    if (!widget) {
        return;
    }
    QTimer::singleShot(0, widget, [widget]() {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        const QScreen* screen = widget->screen();
#else
        const QScreen* screen = widget->windowHandle()->screen();
#endif
        const QPoint globalPos = QCursor::pos(screen);
        if (!QRect(widget->mapToGlobal(QPoint{ 0, 0 }), widget->size()).contains(globalPos)) {
            QCoreApplication::postEvent(widget, new QEvent(QEvent::Leave));
            if (widget->testAttribute(Qt::WA_Hover)) {
                const QPoint localPos = widget->mapFromGlobal(globalPos);
                const QPoint scenePos = widget->window()->mapFromGlobal(globalPos);
                static constexpr const auto oldPos = QPoint{};
                const Qt::KeyboardModifiers modifiers = QGuiApplication::keyboardModifiers();
#if (QT_VERSION >= QT_VERSION_CHECK(6, 4, 0))
                const auto event =
                    new QHoverEvent(QEvent::HoverLeave, scenePos, globalPos, oldPos, modifiers);
                Q_UNUSED(localPos);
#elif (QT_VERSION >= QT_VERSION_CHECK(6, 3, 0))
                const auto event = new QHoverEvent(QEvent::HoverLeave, localPos, globalPos, oldPos, modifiers);
                Q_UNUSED(scenePos);
#else
                const auto event = new QHoverEvent(QEvent::HoverLeave, localPos, oldPos, modifiers);
                Q_UNUSED(scenePos);
#endif
                QCoreApplication::postEvent(widget, event);
            }
        }
    });
}

void MainWidget::changeMusic(QListWidgetItem* item)
{
    if (item == nullptr) {
        return;
    }
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
        //QMetaObject::invokeMethod(this, [&]() {
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
            QFontMetrics fm{ ui->label_MusicName->font() };
            ui->label_MusicName->setText(fm.elidedText(m_currentMusicInfo, Qt::ElideRight, availableWidth));
        }
        m_playbackTimer->singleShot(6, this, SLOT(on_enableListWidget(void)));
        //}, Qt::QueuedConnection);
        locker.store(false);
        locker.notify_one();

    });
}

void MainWidget::on_volumeChanged(int value)
{
    m_mediaPlayer->audioOutput()->setVolume((float)value / 100.0);
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
        //auto file = new QMenu(tr("File(&F)"), menuBar);
        //file->addAction(new QAction(tr("New(&N)"), menuBar));
        //file->addAction(new QAction(tr("Open(&O)"), menuBar));
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

        //menuBar->addMenu(file);
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

    int availableWidth = std::max(100, ui->horizontalLayout_5->geometry().width() - 20); // Subtract a fixed value (e.g., 20)
    QFontMetrics fm{ ui->label_MusicName->font() };
    ui->label_MusicName->setText(fm.elidedText(m_currentMusicInfo, Qt::ElideRight, availableWidth));
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
    m_musicList.append(list);
    for (const auto& i : list) {
        ui->listWidget_PlayList->addItem(new MusicItem{ui->listWidget_PlayList, i});
    }
    ConfigManager::SaveLoadedMusicList(m_musicList);
}

void MainWidget::on_pushButton_ShowPlayList_clicked()
{
    ui->listWidget_PlayList->setVisible(!ui->listWidget_PlayList->isVisible());
}

void MainWidget::on_positionChanged(qint64 value)
{
    if (!ui->horizontalSlider_Progress->isSliderDown()) {
        ui->horizontalSlider_Progress->setValue(value);
        updateTimeLabel(value, ui->horizontalSlider_Progress->maximum());
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
    }
}

void MainWidget::dropEvent(QDropEvent *event)
{
    const auto urls = event->mimeData()->urls();
    QStringList list;
    for (const QUrl &url : urls) {
        QFileInfo fileInfo(url.toLocalFile());
        if (fileInfo.exists() && fileInfo.isFile() && fileInfo.suffix().toLower().contains("mp3")) {
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
    connect(deleteAction, &QAction::triggered, this, [this, item]() {
        removeSelectedItem(item);
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
        while ((peddingMusicRow = distrib(gen)) == currentMusicRow) {};
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
        while ((peddingMusicRow = distrib(gen)) == currentMusicRow) {};
        ui->listWidget_PlayList->setCurrentRow(peddingMusicRow);
        changeMusic(ui->listWidget_PlayList->item(peddingMusicRow));
    }
        break;
    default:
        break;
    }

}

