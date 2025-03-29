#include "widget.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QDebug>

int main(int argc, char *argv[])
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0) // && QT_VERSION <= QT_VERSION_CHECK(6, 0, 0)
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
    QCoreApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
    QApplication a(argc, argv);

    // 加载翻译文件
    QTranslator translator;
    if (translator.load(QLocale(), "MusicPlayer", "_", ":/i18n")) {
        a.installTranslator(&translator);
        qDebug() << "Translation loaded successfully.";
    } else {
        qDebug() << "Failed to load translation.";
    }

    MainWidget w;
    w.show();
    return a.exec();
}
