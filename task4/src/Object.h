#ifndef OBJECT_H
#define OBJECT_H

#include "Geometry.h"
#include <QVector4D>
#include <QVector2D>
#include <math.h>

#define EPS_NORMAL 0.001

const QVector3D AMBIENT (0.2, 0.2, 0.2);
const QVector3D DIFFUSE (0.8, 0.8, 0.8);
const QVector3D SPECULAR (0.0, 0.0, 0.0);
const double SHININESS = 0.0;
const double REFLECTION = 0.0;
const double REFRACTION = 0.0;

const double RADIUS_STANDART_SPHERE = 100;


struct Light
{
    QVector3D position;
    QVector3D ambient;
    QVector3D diffuse;
    QVector3D specular;

    Material mat;
};

/* The power of inheritance!!! */
class Object
{
	Material mat;

protected:
	QVector3D position;
public:
	virtual double F(const QVector3D &z) = 0;
	QVector3D getNormal(const QVector3D & z);

	void setColor(const QVector3D &z);
	QVector3D getColor();

	Material getMaterial();

	Object(const QVector3D &amb,
	       const QVector3D &dif,
	       const QVector3D &spec,
	       double shin, double refl,
	       double refr);
};

class Sphere : public Object
{
	double radius;
public:
	double F(const QVector3D &z);
	Sphere (double r, const QVector3D &p);
};

class Box : public Object
{
	double x, y, z;

	QVector3D abs(const QVector3D &z);
	QVector3D max(const QVector3D &z, double l);
	double maxcomp(const QVector3D &di);
public:
	double F(const QVector3D &z);
	Box(double px, double py, double pz, QVector3D pp);

};

class Plane : public Object
{
	double a, b, c, d;
	QVector4D n;
public:
	double F(const QVector3D &z);
	Plane(const QVector4D &n);

};

class Cone : public Object
{
	QVector2D c;
public:
	double F(const QVector3D &z);
	Cone(const QVector2D &pc, const QVector3D &pos);
};

class Torus : public Object
{
protected:
	QVector2D t;
public:
	virtual double F(const QVector3D &z);
	Torus (const QVector2D &pt, const QVector3D &pos);

};

class Torus64 : public Torus
{
public:
	double F(const QVector3D &z) {
		QVector3D p = z - position;
		QVector2D p2 = QVector2D(p.x(), p.y());

		QVector2D q = QVector2D(p2.length() - t.x(), p.z());
		return pow(pow(q.x(), 8) + pow(q.y(), 8), 1.0/8) - t.y();
	}
	Torus64(const QVector2D &pt, const QVector3D &pos)
		: Torus(pt, pos)
	{}
};

class DisplacementTorus : public Torus
{
public:
	double F(const QVector3D &z)
	{
		QVector3D p = z - position;
		QVector2D p2 = QVector2D(p.x(), p.z());

		QVector2D q = QVector2D(p2.length() - t.x(), p.y());
		double f1 = q.length() - t.y();

		double f2 =  sin(20*p.x())*sin(20*p.y())*sin(20*p.z());
		return f1 + f2;
	}
	DisplacementTorus(const QVector2D &pt, const QVector3D &pos)
		: Torus(pt, pos)
	{}
};

#endif // OBJECT_H
