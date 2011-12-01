#include <QtGui/QApplication>
#include "RayTrace.h"
#include <omp.h>

int main(int argc, char *argv[])
{
//#if defined(_OPENMP)
//	omp_set_num_threads(omp_get_num_procs());
//#endif

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
