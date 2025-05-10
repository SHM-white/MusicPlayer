// 学校: 南京航空航天大学
// 学号: 082420122
// 姓名: 周奕轩
// 项目: 2025 C++程序设计课程设计

#include "ConfigManager.h"
#include <QFile>
#include <QTextStream>

// 姓名: 周奕轩
// Asynchronous save function implementation
QFuture<bool> ConfigManager::SaveLoadedMusicList(const QStringList& list) {
	return QtConcurrent::run([&](const QStringList& list){
		static std::atomic_bool locker{ false };
		locker.wait(true);
		QFile file{ GlobalConfigs::LOCAL_PLAY_LIST()};
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			locker.store(false);
			locker.notify_one(); 
			return false;
		}

		QTextStream out(&file);
		for (const auto& item : list) {
			out << item << '\n';
		}
		file.close();
		locker.store(false);
		locker.notify_one();
		return true;
	}, list);
}

// 学校: 南京航空航天大学
// Asynchronous load function implementation
QFuture<QStringList> ConfigManager::LoadMusicList(const QString& listPath) {
	return QtConcurrent::run([&, listPath](){
		static std::atomic_bool locker{ false };
		locker.wait(true);
		QStringList list;
		QFile file{ listPath };
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			locker.store(false);
			locker.notify_one();
			return list; // Return empty list if file cannot be opened
		}

		QTextStream in(&file);
		while (!in.atEnd()) {
			list.append(in.readLine());
		}
		file.close();
		locker.store(false);
		locker.notify_one();
		return list;
	});
}

// 项目: 2025 C++程序设计课程设计
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

// 学号: 082420122
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
