#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "config.h"
#include "applicationmanager.h"
#include "laboratoriesmodel.h"
#include "computerlab.h"
#include <QtQml>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("applicationConfig", Config::instance());
    engine.rootContext()->setContextProperty("applicationManager", ApplicationManager::instance());
    engine.rootContext()->setContextProperty("labModel", ApplicationManager::instance()->model());
    qmlRegisterType<DiskLayout>("faith.mobile.disklayout", 1,0, "DiskLayout");
    qmlRegisterType<DiskLabel>("faith.mobile.disklabel", 1,0, "DiskLabel");

    ApplicationManager::instance()->readConfig();

    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));
    return app.exec();
}
