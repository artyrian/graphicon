#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <QVector3D>

struct Material
{
	QVector3D ambient;
	QVector3D diffuse;
	QVector3D specular;

	double shininess;

	double reflection;
	double refraction;

	QVector3D color;
};

class Vec: public QVector3D
{
	Vec();
public:
	double static multCoords(const QVector3D &v1, const QVector3D &v2)
	{
		QVector3D a = v1.normalized();
		QVector3D b = v2.normalized();

		double mult = a.x() * b.x() + a.y() * b.y() + a.z()*b.z();
		mult = (mult < 0) ? 0 : mult;

		return mult;
	}
};

struct Ray
{
	QVector3D start;
	QVector3D direction;
	Ray(const QVector3D &a, const QVector3D &b);
};

struct Hit
{
	double distance;
	Material mat;
	QVector3D normal;
	Hit(double d = -0.001, const Material &m = Material(), const QVector3D &norm = QVector3D(0.0, 0.0, 0.0))
		: distance(d), mat(m), normal(norm)
	{}
};

#endif // GEOMETRY_H
