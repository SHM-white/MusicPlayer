#include "ConfigManager.h"
#include <QFile>
#include <QTextStream>


// Asynchronous save function implementation
QFuture<bool> ConfigManager::SaveLoadedMusicList(const QStringList& list) {
	return QtConcurrent::run([list](){
		QFile file{ GlobalConfigs::CONFIG_FILE_PATH };
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			return false;
		}

		QTextStream out(&file);
		for (const auto& item : list) {
			out << item << '\n';
		}
		file.close();
		return true;
	});
}

// Asynchronous load function implementation
QFuture<QStringList> ConfigManager::LoadMusicList() {
	return QtConcurrent::run([](){
		QStringList list;
		QFile file{ GlobalConfigs::CONFIG_FILE_PATH };
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			return list; // Return empty list if file cannot be opened
		}

		QTextStream in(&file);
		while (!in.atEnd()) {
			list.append(in.readLine());
		}
		file.close();
		return list;
	});
}
