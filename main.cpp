#include <QCoreApplication>
#include <QCommandLineParser>
#include <QImage>
#include <QtCore>

#include <iostream>
#include "utils/sceneparser.h"
#include "raytracer/raytracer.h"
#include "raytracer/raytracescene.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

//    QSettings settings("CSCI1230", "Ray");
    QSettings settings( ":/configs/main.ini", QSettings::IniFormat );
    QStringList keys = settings.allKeys();


//    settings->beginGroup("user");
//    QCommandLineParser parser;
//    parser.addHelpOption();

//    parser.addPositionalArgument("scene", "Path of the scene file.");
//    parser.addPositionalArgument("output", "Path of the output image.");


//    QCommandLineOption widthOption("width", "width of the output image.", "width", "800");
//    QCommandLineOption heightOption("height", "height of the output image.", "height", "800");

//    parser.addOption(widthOption);
//    parser.addOption(heightOption);

//    QCommandLineOption enableShadows("shadows", "Enable shadow for ray tracing.");
//    QCommandLineOption enableReflection("reflect", "Enable reflection for ray tracing.");
//    QCommandLineOption enableRefraction("refract", "Enable refraction for ray tracing.");
//    QCommandLineOption enableTextureMap("texture", "Enable texture mapping for ray tracing.");

//    parser.addOption(enableShadows);
//    parser.addOption(enableReflection);
//    parser.addOption(enableRefraction);
//    parser.addOption(enableTextureMap);

//    QCommandLineOption enableParallelism("parallel", "Enable parallelism for ray tracing.");
//    QCommandLineOption enableSuperSample("super-sample", "Enable super sampling for ray tracing.");
//    QCommandLineOption enableAcceleration("acceleration", "Enable acceleration data structure for ray tracing.");

//    parser.addOption(enableParallelism);
//    parser.addOption(enableSuperSample);
//    parser.addOption(enableAcceleration);

//    parser.process(a);

//    auto positionalArgs = parser.positionalArguments();

//    if (positionalArgs.size() != 2) {
//        std::cerr << "not enough arguments for the ray tracer, please run with --help to see the manual" << std::endl;
//        a.exit(1);
//        return 1;
//    }

    QString iScenePath = settings.value("IO/scene").toString();
    QString oImagePath = settings.value("IO/output").toString();

    SceneMetaData metaData;
    bool success = SceneParser::parse(iScenePath.toStdString(), metaData);

//    if (!success) {
//        std::cerr << "error loading scene: " << iScenePath.toStdString() << std::endl;
//        a.exit(1);
//        return 1;
//    }

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

//    rtConfig.enableShadow = parser.isSet(enableShadows);
//    rtConfig.enableReflection = parser.isSet(enableReflection);
//    rtConfig.enableRefraction = parser.isSet(enableRefraction);
//    rtConfig.enableTextureMap = parser.isSet(enableTextureMap);

//    rtConfig.enableParallelism = parser.isSet(enableParallelism);
//    rtConfig.enableSuperSample = parser.isSet(enableSuperSample);
//    rtConfig.enableAcceleration = parser.isSet(enableAcceleration);

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
