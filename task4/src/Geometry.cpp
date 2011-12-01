#include "Geometry.h"


Ray::Ray(const QVector3D &a, const QVector3D &b)
{
    start = a;
    direction = b;
    direction.normalize();
}
