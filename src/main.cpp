#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QUrl>
#include <QQuickStyle>
#include <KLocalizedContext>
#include <KLocalizedString>
#include <KIconTheme>

#include "model.h"
#include "Media.h"
#include "MediaManager.h"
#include "Settings.h"

int main(int argc, char *argv[])
{
	qmlRegisterType<Model>("CustomModel", 1, 0, "CustomModel");
    KIconTheme::initTheme();
    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain("playit");
    QApplication::setOrganizationDomain("playit");
    QApplication::setApplicationName(QStringLiteral("playit"));
    QApplication::setDesktopFileName(QStringLiteral("playit"));

    QApplication::setStyle(QStringLiteral("breeze"));
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }

    QQmlApplicationEngine engine;

    Model model;
    Media media;
    Settings settings;

    MediaManager* mm = new MediaManager(&model, &media);

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.rootContext()->setContextProperty("customModel", &model);
    engine.rootContext()->setContextProperty("media", &media);
    engine.rootContext()->setContextProperty("settings", &settings);
    engine.rootContext()->setContextProperty("audioOutputDevices", mm->audioOutputDevices());

    engine.loadFromModule("org.kde.playit", "Main");

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
