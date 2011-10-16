#include <QApplication>
#include "imageprocessing.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ImageProcessing window;
    window.show();

    return app.exec();
}
