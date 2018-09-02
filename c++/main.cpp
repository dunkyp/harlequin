#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QSurfaceFormat>
#include <QWindow>
#include "hald.hpp"
#include "colourWheel.hpp"

int main (int argc, char* argv[])
{

    QGuiApplication app (argc, argv);
    qmlRegisterType<HALD>("splash.fish.harlequin", 1, 0, "HALD");
    qmlRegisterType<ColourWheel>("splash.fish.harlequin", 1, 0, "ColourWheel");
    qRegisterMetaType<ColourWheel::Space>("Space");
    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:/qml/main.qml"));
    auto window = app.topLevelWindows().first();
    QSurfaceFormat format;
    format.setSamples(16);
    format.setMajorVersion(4);
    format.setMinorVersion(1);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(1);
    format.setStencilBufferSize(1);
    window->setFormat(format);
    return app.exec();
}
