#pragma once

#include "includeFiles.h"

class MusicItem : public QListWidgetItem
{
	//Q_OBJECT

public:
	QUrl url;
	MusicItem(QListWidget *parent, const QUrl& url);
	~MusicItem();
	QMediaMetaData load(QMediaPlayer* player);
    QMediaMetaData mediaMetaData() const;
private:
	void updateText();
    QMediaMetaData m_mediaMetaData;
};
