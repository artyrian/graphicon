#ifndef FIGURES_H
#define FIGURES_H

#include <QtGui>
#include <QVector>
#include <QVector3D>
#include <QColor>
#include <QMatrix4x4>
#include <GL/glut.h>

static inline void qMultMatrix(const QMatrix4x4 &mat)
{
    if (sizeof(qreal) == sizeof(GLfloat)) {
	glMultMatrixf((GLfloat*)mat.constData());
    } else {
	GLfloat fmat[16];
	qreal const *r = mat.constData();
	for (int i = 0; i < 16; ++i)
	    fmat[i] = r[i];
	glMultMatrixf(fmat);
    }
}

static inline void qSetColor(GLfloat colorVec[], QColor c)
{
    colorVec[0] = c.redF();
    colorVec[1] = c.greenF();
    colorVec[2] = c.blueF();
    colorVec[3] = c.alphaF();
}


struct Geometry {
    QVector<GLushort> faces;
    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;
    void loadArrays() const;

//    void appendSmooth(const QVector3D &a, const QVector3D &n, int from);
    void appendFaceted(const QVector3D &a, const QVector3D &n);

    void finalize();
};


class Patch {

public:
//    enum Smoothing { Faceted, Smooth };
    Patch(Geometry *);
    ~Patch();
//    void setSmoothing(Smoothing s) { sm = s; }
    void translate(const QVector3D &t);
    void rotate(qreal deg, QVector3D axis);
    void draw() const;
    void addTri(const QVector3D &a, const QVector3D &b, const QVector3D &c, const QVector3D &n);
    void addQuad(const QVector3D &a, const QVector3D &b,  const QVector3D &c, const QVector3D &d);

    GLushort start;
    GLushort count;
//    GLushort initv;

    GLfloat faceColor[4];
    QMatrix4x4 mat;
//    Smoothing sm;
    Geometry *geom;
};

class CoordAxis {
public:
    CoordAxis(qreal x, qreal y, qreal z);
};

class Rectoid {
public:
    void translate(const QVector3D &t);
    void rotate(qreal deg, QVector3D axis);
    QList<Patch*> parts;
};

class RectTorus : public Rectoid {
public:
    RectTorus(Geometry *g, qreal iRad, qreal oRad, qreal depth, int numSectors);
};

class RectSolidCylindre : public Rectoid {
public:
    RectSolidCylindre(Geometry *g, qreal rad, qreal depth, int numSectors);
};

class RectPrism : public Rectoid {
public:
    RectPrism(Geometry *g, qreal width, qreal height, qreal depth);
};

class RectMarks : public Rectoid {
public:
    RectMarks(Geometry *g, qreal rad, qreal width, qreal height, qreal depth);
};

//======
class RectSecondHand : public Rectoid {
    QTime time;
    QVector3D z;
    QVector3D placeHand;
    QVector3D placePend;
public:
    RectSecondHand(Geometry *g, qreal edge, qreal base, qreal wight_base, qreal depth);
};

class RectMinuteHand : public Rectoid {
    QTime time;
    QVector3D z;
public:
    RectMinuteHand(Geometry *g, qreal x, qreal y, qreal depth);
};

class RectHourHand : public Rectoid {
    QTime time;
    QVector3D z;
public:
    RectHourHand(Geometry *g, qreal x, qreal y, qreal depth);
};
#endif // FIGURES_H
