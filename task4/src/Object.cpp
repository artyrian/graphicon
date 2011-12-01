#include "Object.h"

Object::Object(const QVector3D &amb,
       const QVector3D &dif,
       const QVector3D &spec,
       double shin, double refl,
       double refr)
{
	mat.ambient = amb;
	mat.diffuse = dif;
	mat.specular = spec;
	mat.shininess = shin;
	mat.reflection = refl;
	mat.refraction = refr;
}

void Object::setColor(const QVector3D &z)
{
	mat.color = z;
}

QVector3D Object::getColor()
{
	return mat.color;
}

Material Object::getMaterial()
{
	return mat;
}

QVector3D Object::getNormal(const QVector3D &z)
{
    QVector3D z1 = z + QVector3D(EPS_NORMAL, 0.0, 0.0);
    QVector3D z2 = z - QVector3D(EPS_NORMAL, 0.0, 0.0);
    QVector3D z3 = z + QVector3D(0.0, EPS_NORMAL, 0.0);
    QVector3D z4 = z - QVector3D(0.0, EPS_NORMAL, 0.0);
    QVector3D z5 = z + QVector3D(0.0, 0.0, EPS_NORMAL);
    QVector3D z6 = z - QVector3D(0.0, 0.0, EPS_NORMAL);

    double dx = F(z1) - F(z2);
    double dy = F(z3) - F(z4);
    double dz = F(z5) - F(z6);

    return (QVector3D(dx, dy, dz) / (2.0 * EPS_NORMAL)).normalized();
}



double Sphere::F(const QVector3D &z)
{
	return (z - position).length() - radius;
}

Sphere::Sphere(double r, const QVector3D &p)
	: Object(AMBIENT, DIFFUSE, SPECULAR, SHININESS,REFLECTION, REFRACTION)
{
	position = p;
	radius = r;
}

Box::Box(double px, double py, double pz, QVector3D pp)
	: Object(AMBIENT, DIFFUSE, SPECULAR, SHININESS,REFLECTION, REFRACTION)
{
	x = px;
	y = py;
	z = pz;
	position = pp;
}

QVector3D Box::abs(const QVector3D &z)
{
	return QVector3D (
				z.x() < 0 ? - z.x() : z.x(),
				z.y() < 0 ? - z.y() : z.y(),
				z.z() < 0 ? - z.z() : z.z()
	);
}

QVector3D Box::max(const QVector3D &z, double m)
{
	return QVector3D (
				z.x() < m ? m : z.x(),
				z.y() < m ? m : z.y(),
				z.z() < m ? m : z.z()
	);
}

double Box::maxcomp(const QVector3D &di)
{
	double mc = di.x();

	if (mc < di.y()) {
		mc = di.y();
	}

	if (mc < di.z()) {
		mc = di.z();
	}

	return mc;
}


double Box::F(const QVector3D &p)
{
    QVector3D  di = abs(p - position) - QVector3D(x / 2, y / 2, z / 2);
    double mc = maxcomp(di);
    return qMin(mc, max(di, 0.0).length());
}


double Plane::F(const QVector3D &p)
{
	QVector3D n3 =n.toVector3D().normalized();
	return QVector3D::dotProduct(n3, p) + n.w() / n3.lengthSquared();
}

Plane::Plane(const QVector4D & abcd)
	: Object(AMBIENT, DIFFUSE, SPECULAR, SHININESS,REFLECTION, REFRACTION)
{
	n = abcd;
}

Cone::Cone(const QVector2D &pc, const QVector3D &pos)
	: Object(AMBIENT, DIFFUSE, SPECULAR, SHININESS,REFLECTION, REFRACTION)
{
	c = pc.normalized();
	position = pos;
}

double Cone::F(const QVector3D &p)
{
	QVector3D s = p - position;
	    float q = QVector2D(s.x(), s.z()).length();
	    return QVector2D::dotProduct(c, QVector2D(q, s.y()));
}

Torus::Torus(const QVector2D &pt, const QVector3D &pos)
	: Object(AMBIENT, DIFFUSE, SPECULAR, SHININESS,REFLECTION, REFRACTION)
{
	position = pos;
	t = pt;
}

double Torus::F(const QVector3D &pp)
{
	QVector3D p = pp - position;
	QVector2D p2 = QVector2D(p.x(), p.z());

	QVector2D q = QVector2D(p2.length() - t.x(), p.y());
	return q.length() - t.y();
}
