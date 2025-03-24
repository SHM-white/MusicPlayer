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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#  include <QtGui/QActionGroup>
#else
#  include <QtWidgets/QActionGroup>
#endif

// #include <QtWebEngineWidgets/QWebEngineView>

//#include <QWKWidgets/widgetwindowagent.h>
#include <QWKWidgets/widgetwindowagent.h>
#include "../libs/QWindowKit/src/widgets/widgetwindowagent.h"
//#include <QWKCore/qwindowkit_windows.h>
#include <widgetframe/windowbar.h>
#include <widgetframe/windowbutton.h>
#include <QWKWidgets/qwkwidgetsglobal.h>
#include <QWKCore/qwkglobal.h>
#include <QWKCore/windowagentbase.h>
