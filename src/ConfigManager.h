#pragma once

#include "includeFiles.h"

using ApplicationSettings = QMap<QString, QVariant>;

class ConfigManager
{
private:

public:
	//Asynchronous save and load functions
	static QFuture<bool> SaveLoadedMusicList(const QStringList& list);
	static QFuture<bool> SaveSettings(const QString& settingsPath, const QMap<QString, QVariant>& settings);
	
	static QFuture<QStringList> LoadMusicList(const QString& listPath);
	static QFuture<ApplicationSettings> LoadSettings(const QString& settingsPath);
};
