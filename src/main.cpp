#include <QApplication>
#include <QQmlApplicationEngine>
#include <QIcon>
#include <QQmlContext>
#include <QWidget>

#include "imageprocess.h"
#include "ocvimageprovider.h"
#include "threadcontroller.h"
#include "utility.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication app(argc, argv);

    // Initialize utility for some additional pasering, then transfet to ThreadController
    Utility *utility = new Utility();
   
    // Initialize the ImageProvider, and transfer to the ThreadController
    OCVImageProvider *ocvImageProvider = new OCVImageProvider();
   
    // Initialize statusMoinitor for monitoring usage of some hardwares, also pass to ThreadController
    StatusMonitor *statusMonitor = new StatusMonitor();
   
    // Initialize the thread to do our work
    ThreadController *controller = new ThreadController(nullptr, ocvImageProvider, statusMonitor, utility);


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
