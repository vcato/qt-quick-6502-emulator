#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "computer.hpp"
#include "rambusdeviceview.hpp"
#include "rambusdevicetablemodel.hpp"
#include "rambusdevicedisassemblymodel.hpp"

int main(int argc, char *argv[])
{
    // This is not available with Qt 5.5.  Maybe there is another way
    // to do it?
    // QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    // This is what allows QML to have access to our type.
    Computer::RegisterType();
    RamBusDeviceView::RegisterType();
    RamBusDeviceTableModel::RegisterType();
    RamBusDeviceDisassemblyModel::RegisterType();

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl)
                             QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
