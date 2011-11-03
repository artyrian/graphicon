#ifndef FIGURES_H
#define FIGURES_H

#include <QtGui>
#include <QVector>
#include <QVector3D>
#include <QColor>
#include <QMatrix4x4>

#include <QtOpenGL/qgl.h>


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
    QVector<QVector2D> textures;
    void loadArrays() const;

    void appendSmooth(const QVector3D &a, const QVector3D &n, int from);
    void appendFaceted(const QVector3D &a, const QVector3D &n);

    void finalize();
};


class Patch {

public:
    Patch(Geometry *);
    ~Patch();
    void setSmoothing(bool s);
    void translate(const QVector3D &t);
    void rotate(qreal deg, QVector3D axis);
    void draw() const;
    void addTri(const QVector3D &a, const QVector3D &b, const QVector3D &c, const QVector3D &n);
    void addQuad(const QVector3D &a, const QVector3D &b,  const QVector3D &c, const QVector3D &d);
    void addTexTri(const QVector3D &a, const QVector3D &b, const QVector3D &c, const qreal &fullLength);

    GLushort start;
    GLushort count;
    GLushort initv;

    GLfloat faceColor[4];
    QMatrix4x4 mat;
    bool smoothing;
    Geometry *geom;

    int texture_num;
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

class RectEllipseTorus : public Rectoid {
public:
    RectEllipseTorus(Geometry *g, qreal iRad, qreal oRad, qreal depth, int numSectors);
};

class RectBeltTorus : public Rectoid {
public:
    RectBeltTorus(Geometry *g);
};

class RectTriangleTorus : public Rectoid {
public:
    RectTriangleTorus(Geometry *g, qreal iRad, qreal oRad, qreal depth, int numSectors);
};

class RectSolidCylindre : public Rectoid {

public:
    RectSolidCylindre(Geometry *g, qreal rad, qreal depth, int numSectors);

};

class TopBelt : public Rectoid {
public:
    TopBelt(Geometry *g);
};

class BottomBelt : public Rectoid {
public:
    BottomBelt(Geometry *g);
};

class Benches : public Rectoid {

public:
    Benches(Geometry *g, qreal widht, qreal height,  qreal depth, qreal border);
};

class Winch : public Rectoid {
public:
    Winch(Geometry *g, qreal iRad, qreal depth);
};

class RectPrism : public Rectoid {
public:
    RectPrism(Geometry *g, qreal width, qreal height, qreal depth);
};

class RectTri : public Rectoid {
public:
    RectTri(Geometry *g, qreal width, qreal height, qreal depth);
};

class RectPrism2 : public Rectoid {
public:
    RectPrism2(Geometry *g, qreal width, qreal height, qreal depth, qreal delta);
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
    RectSecondHand(Geometry *g, qreal edge, qreal base, qreal wight_base, qreal depth, bool anim);
};

class RectMinuteHand : public Rectoid {
    QTime time;
    QVector3D z;
public:
    RectMinuteHand(Geometry *g, qreal x, qreal y, qreal depth, bool anim);
};

class RectHourHand : public Rectoid {
    QTime time;
    QVector3D z;
public:
    RectHourHand(Geometry *g, qreal x, qreal y, qreal depth, bool anim);
};
#endif // FIGURES_H
