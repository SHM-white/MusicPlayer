#include "MusicItem.h"

MusicItem::MusicItem(QListWidget* parent, const QUrl& url)
	: QListWidgetItem(parent), url{ url }
{
	updateText();
}

MusicItem::~MusicItem()
{
}

QFuture<QMediaMetaData> MusicItem::load(std::shared_ptr<QMediaPlayer>& player)
{
    // 使用 QtConcurrent::run 启动后台线程
    return QtConcurrent::run([this, player]() -> QMediaMetaData {
        static std::atomic<bool> locker = false;
        locker.wait(true); // 等待其他线程释放锁
        locker.store(true); // 加锁

        if (url.isEmpty()) {
            locker.store(false); // 解锁
            locker.notify_one();
            return mediaMetaData();
        }

        // 切换到主线程设置媒体源
        QMetaObject::invokeMethod(player.get(), [&]() {
            player->setSource(url);
            }, Qt::BlockingQueuedConnection);

        // 等待媒体加载完成
        while (player->mediaStatus() == QMediaPlayer::MediaStatus::LoadingMedia) {
            QThread::msleep(3); // 避免忙等待
        }

        // 切换到主线程获取元数据
        QMetaObject::invokeMethod(player.get(), [&]() {
            m_mediaMetaData = player->metaData();
            }, Qt::BlockingQueuedConnection);

        // 更新显示文本
        updateText();

        locker.store(false); // 解锁
        locker.notify_one();

        return m_mediaMetaData;
        });
}


QMediaMetaData MusicItem::mediaMetaData() const
{
    return m_mediaMetaData;
}

void MusicItem::updateText()
{
	if (m_mediaMetaData.isEmpty()) {
		setText(url.fileName());
	}
	else
	{
		setText(url.fileName() + QStringLiteral(" - ") + m_mediaMetaData.stringValue(QMediaMetaData::Key::Duration));
	}
}
