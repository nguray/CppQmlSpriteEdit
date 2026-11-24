#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "editarea.h"
#include "palette.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("CppQmlSpriteEdit", "Main");


    QObject *root = engine.rootObjects().first();
    auto *palette = root->findChild<Palette *>("mypalette");
    auto *editarea = root->findChild<EditArea *>("myeditarea");

    if (palette && editarea) {
        QObject::connect(palette, &Palette::foregroundColorChanged,
                         editarea, &EditArea::onForegroundColorChanged);
    }

    return QGuiApplication::exec();
}
