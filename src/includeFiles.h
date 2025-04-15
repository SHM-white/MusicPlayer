#pragma once

//DEBUG
#ifdef _DEBUG
//#undef _WIN32
#define DEBUG
#endif // 


#ifdef _WIN32
#include <windows.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")	
#define NOMINMAX
#endif	// _WIN32

//CPP standard headers
#include <memory>
#include <atomic>

//Qt headers
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtGui/QPainter>
#include <QtGui/QWindow>
#include <QtWidgets/qslider.h>
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyle>
#include <QtWidgets/QPushButton>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qtooltip.h>
#include <QFuture>
#include <QtConcurrent/qtconcurrentrun.h>
#include <QtWidgets/qlistwidget.h>
#include <QListWidgetItem>
#include <QLabel>
#include <QPropertyAnimation>
#include <QMimeData>
#include <QFileInfo>

#include <qpainterpath.h>
#include <qwidget.h>
#include <qpainter.h>
#include <qmainwindow.h>
#include <qlabel.h>
#include <qmenubar.h>
#include <qmenu.h>
#include <qtoolbar.h>
#include <qaction.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qtimer.h>
#include <qevent.h>
#include <QtMultimedia/qmediaplayer.h>
#include <qthread.h>
#include <qurl.h>
#include <qmediametadata.h>
#include <qaudiooutput.h>
#include <qsettings.h>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#  include <QtGui/QActionGroup>
#else
#  include <QtWidgets/QActionGroup>
#endif

//QWindowKit headers
#include <QWKWidgets/widgetwindowagent.h>
#include "../libs/QWindowKit/src/widgets/widgetwindowagent.h"
#include <widgetframe/windowbar.h>
#include <widgetframe/windowbutton.h>
#include <QWKWidgets/qwkwidgetsglobal.h>
#include <QWKCore/qwkglobal.h>
#include <QWKCore/windowagentbase.h>

enum Theme {
	Dark,
	Light,
};

class Utils {
public:
	static inline QString QTimeToQString(const QTime& time) {
		return (time < QTime(1, 0, 0, 0)) ? time.toString("mm:ss") : time.toString("hh:mm:ss");
	}
};

class GlobalConfigs {
public:
	static inline const QString CONFIG_FILE_PATH = QCoreApplication::applicationDirPath() + QStringLiteral("/config.cfg");
	static inline const QString LOCAL_PLAY_LIST = QCoreApplication::applicationDirPath() + QStringLiteral("/current.playlist");
};

//Icons in Segoe Fluent Icons font
//Needs set font to "Segoe Fluent Icons"
namespace Icons {
	const auto Font = QFont(QStringLiteral("Segoe Fluent Icons"));
	enum Icons {
		Play = 0xe768,
		Pause = 0xe769,
		Cancel = 0xe711,
		Refresh = 0xe72c,
		EndPoint = 0xe81b,
		MapLayers = 0xe81e,
		Recent = 0xe823,
		ChromeBack = 0xe830,
		Previous = 0xe892,
		Next = 0xe893,
		Clear = 0xe894,
		Sync = 0xe895,
		Download = 0xe896,
		Help = 0xe897,
		Upload = 0xe898,
		OpenInNewWindow = 0xe8a7,
		Switch = 0xe8ab,
		Shuffle = 0xe8b1,
		Import = 0xe8b5,
		Folder = 0xe8b7,
		ChromeClose = 0xe8bb,
		Sort = 0xe8cb,
		Audio = 0xe8d6,
		Unfavorite = 0xe8d9,
		OpenFile = 0xe8e5,
		RepeatOne = 0xe8ed,
		RepeatAll = 0xe8ee,
		FavoriteStar = 0xe734,
		FavoriteStarFill = 0xe735,
		FavoriteList = 0xe728,
		Add = 0xe710,
		Volume0 = 0xe992,
		Volume1 = 0xe993,
		Volume2 = 0xe994,
		Volume3 = 0xe995,
		Mute = 0xe74f,
		BulletedList = 0xe8fd,
		More = 0xe712,
		FullScreen = 0xe740,
		BackToWindow = 0xe73f,
	};
	QChar inline Get(Icons icon) {
		return QChar(icon);
	}
	void inline SetFont(QWidget* widget) {
		widget->setFont(Font);
	}
};