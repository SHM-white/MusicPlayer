#pragma once

#include "includeFiles.h"

class ConfigManager
{
private:

public:
	// Asynchronous save function
	static QFuture<bool> SaveLoadedMusicList(const QStringList& list);

	// Asynchronous load function
	static QFuture<QStringList> LoadMusicList();

};
