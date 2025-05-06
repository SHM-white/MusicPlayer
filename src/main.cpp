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
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString& locale : uiLanguages) {
        const QString baseName = "MusicPlayer_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
			qDebug() << "Loaded translation file for locale:" << locale;
            break;
        }
        else {
			qDebug() << "Failed to load translation file for locale:" << locale;
        }
    }

    MainWidget w;
    w.show();
#ifdef DEBUG
    qDebug() << "QApplication File Path : " << QApplication::applicationFilePath();
    qDebug() << "QApplication Directory Path : " << QApplication::applicationDirPath();
#endif // DEBUG

    return a.exec();
}
