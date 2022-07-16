QT += gui
QT += xml
QT += concurrent

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    camera/RayCamera.cpp \
    main.cpp \
    raytracer/RayTraceScene.cpp \
    raytracer/RayTracer.cpp \
    raytracer/intersectcheck/KDTree.cpp \
    raytracer/intersectcheck/NaiveIntersect.cpp \
    raytracer/lights/DirectionalLight.cpp \
    raytracer/lights/PointLight.cpp \
    raytracer/lights/SpotLight.cpp \
    raytracer/rendertask/RenderTask.cpp \
    raytracer/sampler/AdaptiveSuperSampler.cpp \
    raytracer/sampler/NaiveSampler.cpp \
    raytracer/shapes/BaseShape.cpp \
    raytracer/shapes/ConeShape.cpp \
    raytracer/shapes/CubeShape.cpp \
    raytracer/shapes/CylinderShape.cpp \
    raytracer/shapes/SphereShape.cpp \
    raytracer/utils/TextureManager.cpp \
    utils/RGBA.cpp \
    utils/SceneParser.cpp \
    utils/ScenefileReader.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    camera/Camera.h \
    camera/RayCamera.h \
    raytracer/RayTraceScene.h \
    raytracer/RayTracer.h \
    raytracer/interface/Intersect.h \
    raytracer/interface/Light.h \
    raytracer/intersectcheck/KDTree.h \
    raytracer/intersectcheck/NaiveIntersect.h \
    raytracer/lights/DirectionalLight.h \
    raytracer/lights/PointLight.h \
    raytracer/lights/SpotLight.h \
    raytracer/rendertask/RenderTask.h \
    raytracer/sampler/AdaptiveSuperSampler.h \
    raytracer/sampler/NaiveSampler.h \
    raytracer/sampler/PixelSampler.h \
    raytracer/shapes/BaseShape.h \
    raytracer/shapes/ConeShape.h \
    raytracer/shapes/CubeShape.h \
    raytracer/shapes/CylinderShape.h \
    raytracer/shapes/SphereShape.h \
    raytracer/utils/BoundUtils.h \
    raytracer/utils/TextureManager.h \
    utils/RGBA.h \
    utils/SceneData.h \
    utils/SceneParser.h \
    utils/ScenefileReader.h


INCLUDEPATH += glm utils
DEPENDPATH += glm utils
DEFINES += _USE_MATH_DEFINES
DEFINES += TIXML_USE_STL
DEFINES += GLM_SWIZZLE GLM_FORCE_RADIANS

# Don't add the -pg flag unless you know what you are doing. It makes QThreadPool freeze on Mac OS X
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_CXXFLAGS_WARN_ON -= -Wall
QMAKE_CXXFLAGS_WARN_ON += -Waddress -Warray-bounds -Wc++0x-compat -Wchar-subscripts -Wformat\
                          -Wmain -Wmissing-braces -Wparentheses -Wreorder -Wreturn-type \
                          -Wsequence-point -Wsign-compare -Wstrict-overflow=1 -Wswitch \
                          -Wtrigraphs -Wuninitialized -Wunused-label -Wunused-variable \
                          -Wvolatile-register-var -Wno-extra

QMAKE_CXXFLAGS += -g

macx {
    QMAKE_CXXFLAGS_WARN_ON -= -Warray-bounds -Wc++0x-compat
}

DISTFILES += \
    configs/main.ini

RESOURCES += \
    setting.qrc
