#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QIcon>
#include <QQmlContext>

#include "ocvimageprovider.h"
#include "utility.h"
#include "statusmonitor.h"


int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    Utility *utility = new Utility();
    OCVImageProvider *ocvImageProvider = new OCVImageProvider;
    StatusMonitor *statusMonitor = new StatusMonitor();

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.rootContext()->setContextProperty("controller", controller);
    engine.rootContext()->setContextProperty("statusMonitor", statusMonitor);
    engine.rootContext()->setContextProperty("liveImageProvider",ocvImageProvider);
    engine.rootContext()->setContextProperty("utility", utility);
    engine.addImageProvider(QStringLiteral("live"), ocvImageProvider);


    engine.load(url);

    return app.exec();
}
