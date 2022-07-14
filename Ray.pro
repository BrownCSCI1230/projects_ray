QT += gui
QT += xml
QT += concurrent

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    camera/camtranscamera.cpp \
    camera/fixedcamera.cpp \
    camera/quaternioncamera.cpp \
    main.cpp \
    raytracer/intersectcheck/kdtree.cpp \
    raytracer/intersectcheck/naiveintersect.cpp \
    raytracer/lights/directionallight.cpp \
    raytracer/lights/pointlight.cpp \
    raytracer/lights/spotlight.cpp \
    raytracer/shapes/baseshape.cpp \
    raytracer/shapes/coneshape.cpp \
    raytracer/shapes/cubeshape.cpp \
    raytracer/shapes/cylindershape.cpp \
    raytracer/shapes/sphereshape.cpp \
    raytracer/raytracer.cpp \
    raytracer/rendertask/rendertask.cpp \
    raytracer/sampler/adaptivesupersampler.cpp \
    raytracer/sampler/naivesampler.cpp \
    raytracer/utils/texturemanager.cpp \
    raytracer/raytracescene.cpp \
    utils/scenefilereader.cpp \
    utils/sceneparser.cpp \
    utils/RGBA.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    camera/camera.h \
    camera/camtranscamera.h \
    camera/fixedcamera.h \
    camera/quaternioncamera.h \
    raytracer/interface/light.h \
    raytracer/intersectcheck/kdtree.h \
    raytracer/intersectcheck/naiveintersect.h \
    raytracer/interface/intersect.h \
    raytracer/lights/directionallight.h \
    raytracer/lights/pointlight.h \
    raytracer/lights/spotlight.h \
    raytracer/shapes/baseshape.h \
    raytracer/shapes/coneshape.h \
    raytracer/shapes/cubeshape.h \
    raytracer/shapes/cylindershape.h \
    raytracer/shapes/sphereshape.h \
    raytracer/raytracer.h \
    raytracer/rendertask/rendertask.h \
    raytracer/sampler/adaptivesupersampler.h \
    raytracer/sampler/naivesampler.h \
    raytracer/sampler/pixelsampler.h \
    raytracer/utils/boundutils.h \
    raytracer/utils/texturemanager.h \
    raytracer/raytracescene.h \
    utils/scenedefinition.h \
    utils/scenefilereader.h \
    utils/sceneparser.h \
    utils/RGBA.h

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
