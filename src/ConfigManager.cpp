#include "ConfigManager.h"
#include <QFile>
#include <QTextStream>


// Asynchronous save function implementation
QFuture<bool> ConfigManager::SaveLoadedMusicList(const QStringList& list) {
	return QtConcurrent::run([&](){
		QFile file{ GlobalConfigs::LOCAL_PLAY_LIST()};
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
QFuture<QStringList> ConfigManager::LoadMusicList(const QString& listPath) {
	return QtConcurrent::run([&, listPath](){
		QStringList list;
		QFile file{ listPath };
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

QFuture<bool> ConfigManager::SaveSettings(const QString& settingsPath, const QMap<QString, QVariant>& settings)
{
	return QtConcurrent::run([&, settingsPath, settings]() {
		QFile file{ settingsPath };
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			return false;
		}
		QTextStream out(&file);
		for (auto it = settings.constBegin(); it != settings.constEnd(); ++it) {
			out << it.key() << '=' << it.value().toString() << '\n';
		}
		file.close();
		return true;
	});
}

QFuture<ApplicationSettings> ConfigManager::LoadSettings(const QString& settingsPath)
{
	return QtConcurrent::run([&, settingsPath]() {
		QMap<QString, QVariant> settings;
		QFile file{ settingsPath };
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			return settings; // Return empty map if file cannot be opened
		}
		QTextStream in(&file);
		while (!in.atEnd()) {
			QString line = in.readLine();
			QStringList parts = line.split('=');
			if (parts.size() == 2) {
				settings.insert(parts[0], parts[1]);
			}
		}
		file.close();
		return settings;
	});
}
