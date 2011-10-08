HEADERS       = imageviewer.h \

SOURCES       = imageviewer.cpp \
		main.cpp \
		../src-core/liblinear-1.8/linear.cpp \
		../src-core/liblinear-1.8/tron.cpp \
		../src-core/liblinear-1.8/blas/daxpy.c \
		../src-core/liblinear-1.8/blas/ddot.c \
		../src-core/liblinear-1.8/blas/dscal.c \
		../src-core/liblinear-1.8/blas/dnrm2.c \
		../src-core/Instances.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/imageviewer
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS imageviewer.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/widgets/imageviewer
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)

wince*: {
   DEPLOYMENT_PLUGIN += qjpeg qmng qgif
}
