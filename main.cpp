#include <QCoreApplication>
#include <QCommandLineParser>
#include <QImage>
#include <QtCore>

#include <iostream>
#include "utils/SceneParser.h"
#include "raytracer/raytracer.h"
#include "raytracer/raytracescene.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QSettings settings( ":/configs/main.ini", QSettings::IniFormat );
    QStringList keys = settings.allKeys();


//    settings->beginGroup("user");
//    QCommandLineParser parser;
//    parser.addHelpOption();

    QString iScenePath = settings.value("IO/scene").toString();
    QString oImagePath = settings.value("IO/output").toString();

    RenderData metaData;
    bool success = SceneParser::parse(iScenePath.toStdString(), metaData);

    if (!success) {
        std::cerr << "error loading scene: " << iScenePath.toStdString() << std::endl;
        a.exit(1);
        return 1;
    }

    int width = settings.value("Canvas/width").toInt();
    int height = settings.value("Canvas/height").toInt();

    RayTraceScene rtScene(width, height, metaData);

    RayTracerConfig rtConfig;

    rtConfig.enableShadow = settings.value("Feature/shadows").toBool();
    rtConfig.enableReflection = settings.value("Feature/reflect").toBool();
    rtConfig.enableRefraction = settings.value("Feature/refract").toBool();
    rtConfig.enableTextureMap = settings.value("Feature/texture").toBool();

    rtConfig.enableParallelism = settings.value("Feature/parallel").toBool();
    rtConfig.enableSuperSample = settings.value("Feature/super-sample").toBool();
    rtConfig.enableAcceleration = settings.value("Feature/acceleration").toBool();

    QImage image = QImage(width, height, QImage::Format_RGBX8888);
    image.fill(Qt::black);

    RGBA *data = reinterpret_cast<RGBA *>(image.bits());

    RayTracer raytracer(rtConfig);

    raytracer.render(data, rtScene);

    success = image.save(oImagePath);
    if (!success) {
        image.save(oImagePath, "PNG");
    }

    if (success) {
        std::cout << "Save rendered image to " << oImagePath.toStdString() << std::endl;
    } else {
        std::cerr << "Error: failed to save image to " << oImagePath.toStdString() << std::endl;
    }
    a.exit();
    return 0;
}
