#include "DisplayWidget.h"
#include "LyricsDelegate.h"

DisplayWidget::DisplayWidget(QWidget *parent)
    : QWidget(parent), currentLyricIndex(-1)
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    //QVBoxLayout *leftLayout = new QVBoxLayout();
    QVBoxLayout *rightLayout = new QVBoxLayout();

    //vinylLabel = new QLabel(this);
    //albumCoverLabel = new QLabel(this);
    lyricsView = new QListView(this); // Initialize QListView

    //vinylLabel->setAlignment(Qt::AlignCenter);
    //albumCoverLabel->setAlignment(Qt::AlignCenter);

    lyricsView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    lyricsView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    lyricsDelegate = new LyricsDelegate(this); // Initialize custom delegate
    lyricsView->setItemDelegate(lyricsDelegate);
	lyricsView->setObjectName("lyricsView");
    //leftLayout->addWidget(vinylLabel);
    //leftLayout->addWidget(albumCoverLabel);

    rightLayout->addWidget(lyricsView);

    //mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);

    //rotationTimer = new QTimer(this);
    //connect(rotationTimer, &QTimer::timeout, [this]() {
    //    rotationAngle = (rotationAngle + 5) % 360;
    //    if (!vinylLabel->pixmap().isNull()) {
    //        QPixmap pixmap = vinylLabel->pixmap().transformed(QTransform().rotate(rotationAngle));
    //        vinylLabel->setPixmap(pixmap);
    //    }
    //});
    QTimer* tmp = new QTimer(this);
	tmp->singleShot(10, [this, tmp]() {
		// Set the maximum width for lyrics
		adjustLyricsMaxWidth();
        tmp->deleteLater();
		});

    connect(lyricsView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(handleLyricDoubleClicked(QModelIndex)));
}

void DisplayWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event); // Call base class implementation
    adjustLyricsMaxWidth(); // Adjust maximum width for lyrics
}

DisplayWidget::~DisplayWidget()
{
}

void DisplayWidget::updateMetaData(const QString &musicFilePath, const QPixmap &albumCover)
{
    // Load lyrics
    QString lyricsFilePath = musicFilePath;
    lyricsFilePath.replace(QRegularExpression("\\.\\w+$"), ".lrc"); // Use QRegularExpression instead of QRegExp
    loadLyrics(lyricsFilePath);
}

void DisplayWidget::loadLyrics(const QString &lyricsFilePath)
{
    // Create a QFutureWatcher to monitor the asynchronous task
    QFutureWatcher<QVector<QPair<qint64, QString>>> *watcher = new QFutureWatcher<QVector<QPair<qint64, QString>>>(this);

    // Connect the finished signal to update the UI
    connect(watcher, &QFutureWatcher<QVector<QPair<qint64, QString>>>::finished, this, [this, watcher]() {
        lyricsData = watcher->result(); // Retrieve the parsed lyrics data

        if (lyricsData.isEmpty()) {
            QStringListModel *model = new QStringListModel(this);
            model->setStringList({"Lyrics not found."});
            lyricsView->setModel(model);
        } else {
            QStringList lyricsList;
            for (const auto &lyric : lyricsData) {
                lyricsList.append(lyric.second);
            }
            QStringListModel *model = new QStringListModel(this);
            model->setStringList(lyricsList);
            lyricsView->setModel(model);
        }

        watcher->deleteLater(); // Clean up the watcher
    });

    // Run the parsing logic asynchronously
    QFuture<QVector<QPair<qint64, QString>>> future = QtConcurrent::run([lyricsFilePath]() {
        QVector<QPair<qint64, QString>> parsedLyrics;
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
                    lyric.replace("  ", "\n");
                    lyric.replace("//", "");
                    parsedLyrics.append(qMakePair(timestamp, lyric));
                }
            }
            file.close();
        }
        return parsedLyrics;
    });

    // Set the future to the watcher
    watcher->setFuture(future);
}

void DisplayWidget::updateHighlightedLyric(qint64 currentTime)
{
    if (lyricsData.isEmpty()) return;

    int newIndex = -1;
    for (int i = 0; i < lyricsData.size(); ++i) {
        if (currentTime < lyricsData[i].first) break;
        newIndex = i;
    }

    if (newIndex != currentLyricIndex) {
        currentLyricIndex = newIndex;
        lyricsDelegate->setHighlightedIndex(currentLyricIndex); // Update delegate
        lyricsView->update();
        // Fix: Provide both row and column arguments to index()
        lyricsView->scrollTo(lyricsView->model()->index(currentLyricIndex, 0), QAbstractItemView::PositionAtCenter);
    }
}

//void DisplayWidget::startVinylRotation()
//{
//    rotationTimer->start(100); // Rotate every 100ms
//}
//
//void DisplayWidget::stopVinylRotation()
//{
//    rotationTimer->stop();
//}

void DisplayWidget::adjustLyricsMaxWidth()
{
    if (!lyricsView) return;

    int maxWidth = lyricsView->viewport()->width(); // Set max width to match lyricsView's width
    lyricsDelegate->setMaxWidth(maxWidth); // Update delegate with new max width
    lyricsView->setModel(lyricsView->model());
    lyricsView->update();
}

void DisplayWidget::handleLyricDoubleClicked(const QModelIndex &index)
{
    if (!lyricsData.isEmpty() && index.isValid() && index.row() < lyricsData.size()) {
        qint64 timestamp = lyricsData[index.row()].first;
        emit requestJumpToTimestamp(timestamp); // Emit the jump signal with the timestamp
    }
}
