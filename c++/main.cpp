#include "colourWheel.hpp"
#include "extendedImage.hpp"
#include "imageProjection.hpp"
#include "resultImage.hpp"
#include "colourSlice.hpp"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QSurfaceFormat>
#include <QWindow>

int main(int argc, char *argv[]) {

    QGuiApplication app(argc, argv);
    // QQuickStyle::setStyle("Material");

    qmlRegisterType<ExtendedImage>("splash.fish.harlequin", 1, 0,
                                   "ExtendedImage");
    qmlRegisterType<ResultImage>("splash.fish.harlequin", 1, 0, "ResultImage");
    qmlRegisterType<ColourWheel>("splash.fish.harlequin", 1, 0, "ColourWheel");
    qmlRegisterType<ColourSlice>("splash.fish.harlequin", 1, 0, "ColourSlice");
    qmlRegisterType<ImageProjection>("splash.fish.harlequin", 1, 0,
                                     "ImageProjection");
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
