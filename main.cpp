#include "jellyfin.h"
#include "qmllinker.h"

#include <QFile>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <VLCQtCore/Common.h>
#include <VLCQtQml/Qml.h>

void startInitialization(){

    qmlRegisterSingletonInstance<Jellyfin>("Jellyfin", 1, 0, "Jellyfin", Jellyfin::getInstance());
    qmlRegisterSingletonInstance<QmlLinker>("QmlState", 1, 0, "QmlState", QmlLinker::getInstance());

    qmlRegisterUncreatableType<Node>("Node", 1, 0, "Node", "Nodes cannot be created");
}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    // Initialize QmlLinker singleton and load saved state
    QmlLinker::getInstance(&engine);
    QmlLinker::getInstance()->load();

    if (!VlcCommon::setPluginPath("C:/Program Files/VideoLAN/VLC/plugins"))
        VlcCommon::setPluginPath(qApp->applicationDirPath() + "/plugins");

    VlcQml::registerTypes();

    startInitialization();

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
