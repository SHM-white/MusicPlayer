#include "MusicItem.h"

MusicItem::MusicItem(QListWidget* parent, const QUrl& url)
	: QListWidgetItem(parent), url{ url }
{
	updateText();
}

MusicItem::~MusicItem()
{
}

QMediaMetaData MusicItem::load(QMediaPlayer* player)
{
	player->setSource(url);
	
	m_mediaMetaData = player->metaData();
	updateText();
	return mediaMetaData();
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
		setText(url.fileName() + QStringLiteral("\t") + m_mediaMetaData.stringValue(QMediaMetaData::Key::Duration));
	}
}
