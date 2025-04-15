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
	return QtConcurrent::run([&]() {
		static std::atomic<bool> locker = false;
		while (locker);
		locker = true;
		if (url.isEmpty()) {
			return mediaMetaData();
		}
		player->setSource(url);
		while (player->mediaStatus() == QMediaPlayer::MediaStatus::LoadingMedia) {};
		m_mediaMetaData = player->metaData();
		updateText();
		locker = false;
		return mediaMetaData();
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
		setText(url.fileName() + QStringLiteral("   ") + m_mediaMetaData.stringValue(QMediaMetaData::Key::Duration));
	}
}
