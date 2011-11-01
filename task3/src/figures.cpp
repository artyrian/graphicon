#include "figures.h"

#include <QImage>
#include <iostream>
#include <qmath.h>

static inline QVector<QVector3D> extrude(const QVector<QVector3D> &verts, qreal depth)
{
    QVector<QVector3D> extr = verts;

    for (int v = 0; v < extr.count(); ++v) {
	extr[v].setZ(extr[v].z() - depth);
    }

    return extr;
}

void Geometry::loadArrays() const
{
    glVertexPointer(3, GL_FLOAT, 0, vertices.constData());
    glNormalPointer(GL_FLOAT, 0, normals.constData());
    glTexCoordPointer(2, GL_FLOAT, 0, textures.constData());
}

/* Normalize all normals in vector to length 1.0 */
void Geometry::finalize()
{
    for (int i = 0; i < normals.count(); ++i) {
	normals[i].normalize();
    }
}

void Geometry::appendSmooth(const QVector3D &a, const QVector3D &n, int from)
{
    int v = vertices.count() - 1;
    for ( ; v >= from; --v) {
        if (qFuzzyCompare(vertices[v], a)) {
            normals[v] += n;
            break;
        }
    }

    if (v < from) {
        v = vertices.count();
        vertices.append(a);
        normals.append(n);
    }

    faces.append(v);
}

void Geometry::appendFaceted(const QVector3D &a, const QVector3D &n)
{
    int v = vertices.count();
    vertices.append(a);
    normals.append(n);
    faces.append(v);
}



//===========================


Patch::Patch(Geometry *g)
   : geom(g)

{
    start = geom->faces.count();
    count = 0;
    initv = g->vertices.count();
    smoothing = true;
    qSetColor(faceColor, QColor(Qt::darkGray));
}

void Patch::setSmoothing(bool s)
{
    smoothing = s;
}

void Patch::rotate(qreal deg, QVector3D axis)
{
    mat.rotate(deg, axis);
}

void Patch::translate(const QVector3D &t)
{
    mat.translate(t);
}

void Patch::draw() const
{
    glPushMatrix();
    qMultMatrix(mat);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, faceColor);

    const GLushort *indices = geom->faces.constData();
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, indices + start);
    glPopMatrix();
}

void Patch::addTri(const QVector3D &a, const QVector3D &b, const QVector3D &c, const QVector3D &n)
{
    QVector3D norm = n.isNull() ? QVector3D::normal(a, b, c) : n;

    if (smoothing)
    {
        geom->appendSmooth(a, norm, initv);
        geom->appendSmooth(b, norm, initv);
        geom->appendSmooth(c, norm, initv);
    }
    else
    {
        geom->appendFaceted(a, norm);
        geom->appendFaceted(b, norm);
        geom->appendFaceted(c, norm);
    }

    count += 3;
}

void Patch::addQuad(const QVector3D &a, const QVector3D &b,  const QVector3D &c, const QVector3D &d)
{
    QVector3D norm = QVector3D::normal(a, b, c);
    if (smoothing) {
        addTri(a, b, c, norm);
        addTri(a, c, d, norm);
    } else {
        addTri(a, b, c, norm);
        int k = geom->vertices.count();
        geom->appendSmooth(a, norm, k);
        geom->appendSmooth(c, norm, k);
        geom->appendFaceted(d, norm);
        count += 3;
    }


}

void Rectoid::translate(const QVector3D &t)
{
    for (int i = 0; i < parts.count(); ++i)
	parts[i]->translate(t);
}

void Rectoid::rotate(qreal deg, QVector3D axis)
{
    for (int i = 0; i < parts.count(); ++i)
	parts[i]->rotate(deg, axis);
}

//=======================================

Winch::Winch(Geometry *g, qreal iRad, qreal oRad, qreal depth, int k)
{
    QVector<QVector3D> inside;
    QVector<QVector3D> outside;
    for (int i = 0; i < k; ++i) {
        qreal angle = (i * 2 * M_PI) / k;
        inside << QVector3D(iRad * qSin(angle), iRad * qCos(angle), depth/2.0);
        outside << QVector3D(oRad * qSin(angle + M_PI/(qreal)k), oRad * qCos(angle + M_PI/(qreal)k), depth/2.0);
    }

    QVector<QVector3D> in_back = extrude(inside, depth);
    QVector<QVector3D> out_back = extrude(outside, depth);

    Patch *front = new Patch(g);
    for (int i = 0; i < k; ++i) {
        front->addQuad(outside[i], inside[i], inside[(i + 1) % k], outside[(i + 1) % k]);
        QVector3D a = outside[i];
        QVector3D b = inside [(i+1) % k];
        QVector3D c = inside[i];
        QVector3D norm = QVector3D::normal(a, b, c);
        front->addTri(a, b, c, norm);
    }

    Patch *back = new Patch(g);
    for (int i = 0; i < k; ++i) {
        back->addQuad(in_back[i], out_back[i], out_back[(i + 1) % k], in_back[(i + 1) % k]);
        QVector3D a = out_back[i];
        QVector3D b = in_back[i];
        QVector3D c = in_back[(i+1) % k];
        QVector3D norm = QVector3D::normal(a, b, c);
        front->addTri(a, b, c, norm);
    }

    Patch *os1 = new Patch(g);
    Patch *os2 = new Patch(g);
    for (int i = 0; i < k; ++i) {
        os1->addQuad(in_back[i], out_back[(i + 1) % k], outside[(i + 1) % k], inside[i]);
        os2->addQuad(out_back[(i + 1) % k], in_back[(i+1) % k], inside[(i+1) % k], outside[(i + 1) % k]);
    }


    parts //<< front << back
          << os1 << os2;
}

// ======================================

RectTorus::RectTorus(Geometry *g, qreal iRad, qreal oRad, qreal depth, int k)
{
    QVector<QVector3D> inside;
    QVector<QVector3D> outside;
    for (int i = 0; i < k; ++i) {
	qreal angle = (i * 2 * M_PI) / k;
	inside << QVector3D(iRad * qSin(angle), iRad * qCos(angle), depth/2.0);
	outside << QVector3D(oRad * qSin(angle), oRad * qCos(angle), depth/2.0);
    }
 //   inside << QVector3D(0.0, iRad, 0.0);
 //   outside << QVector3D(0.0, oRad, 0.0);

    QVector<QVector3D> in_back = extrude(inside, depth);
    QVector<QVector3D> out_back = extrude(outside, depth);

    Patch *front = new Patch(g);
    for (int i = 0; i < k; ++i)
	front->addQuad(outside[i], inside[i], inside[(i + 1) % k], outside[(i + 1) % k]);

    Patch *back = new Patch(g);
    for (int i = 0; i < k; ++i)
	back->addQuad(in_back[i], out_back[i], out_back[(i + 1) % k], in_back[(i + 1) % k]);

    Patch *is = new Patch(g);
    for (int i = 0; i < k; ++i)
	is->addQuad(in_back[i], in_back[(i + 1) % k], inside[(i + 1) % k], inside[i]);

    Patch *os = new Patch(g);
    for (int i = 0; i < k; ++i)
	os->addQuad(out_back[(i + 1) % k], out_back[i], outside[i], outside[(i + 1) % k]);

    parts << front << back << is << os;
}

//==================================

RectTriangleTorus::RectTriangleTorus(Geometry *g, qreal iRad, qreal oRad, qreal depth, int k)
{
    QVector<QVector3D> inside;
    QVector<QVector3D> outside;
    for (int i = 0; i < k; ++i) {
        qreal angle = (i * 2 * M_PI) / k;
        inside << QVector3D(iRad * qSin(angle), iRad * qCos(angle), depth/2.0);
        outside << QVector3D(oRad * qSin(angle), oRad * qCos(angle), depth/2.0);
    }

    QVector<QVector3D> in_back = extrude(inside, depth);
    QVector<QVector3D> out_back = extrude(outside, depth);

    Patch *front = new Patch(g);
    for (int i = 0; i < k; ++i) {
        front->addQuad(out_back[i], inside[i], inside[(i + 1) % k], out_back[(i + 1) % k]);
    }

    Patch *back = new Patch(g);
    for (int i = 0; i < k; ++i) {
        back->addQuad(in_back[i], out_back[i], out_back[(i + 1) % k], in_back[(i + 1) % k]);
    }

    Patch *is = new Patch(g);
    for (int i = 0; i < k; ++i) {
        is->addQuad(in_back[i], in_back[(i + 1) % k], inside[(i + 1) % k], inside[i]);
    }

    parts << front << back << is;
}

//==================================

void Patch::addTexTri(const QVector3D &a, const QVector3D &b, const QVector3D &c, const qreal &fullLength)
{
    QVector2D p1(a.x() / (2 * fullLength) + 0.5, a.y() / (2 * fullLength) + 0.5);
    QVector2D p2(b.x() / (2 * fullLength) + 0.5, b.y() / (2 * fullLength) + 0.5);
    QVector2D p3(c.x() / (2 * fullLength) + 0.5, c.y() / (2 * fullLength) + 0.5);

    geom->textures << p1 << p2 << p3  ;
}

RectSolidCylindre::RectSolidCylindre(Geometry *g, qreal rad, qreal depth, int k)
{
    QVector<QVector3D> outside;
    for (int i = 0; i < k; ++i) {
	qreal angle = (i * 2 * M_PI) / k;
	outside << QVector3D(rad * qSin(angle), rad * qCos(angle), 0);
    }

    QVector<QVector3D> out_back = extrude(outside, depth);

    Patch *front = new Patch(g);
    front->setSmoothing(false);
    for (int i = 0; i < k; ++i) {
 //       front->addQuad(outside[i], QVector3D(0, 0, 0), QVector3D(0, 0, 0), outside[(i + 1) % k]);

        QVector3D a = outside[i];
        QVector3D b = QVector3D(0, 0, 0);
        QVector3D c = outside[(i+1)%k];
        QVector3D norm = QVector3D::normal(a, b, c);
        front->addTri(a, b, c, norm);
        front->addTexTri(a, b, c, rad);
   }

    Patch *back = new Patch(g);
    back->setSmoothing(false);
    for (int i = 0; i < k; ++i) {

        QVector3D a = QVector3D(0, 0, -depth);
        QVector3D b = out_back[i];
        QVector3D c = out_back[(i + 1) % k];
        QVector3D norm = QVector3D::normal(a, b, c);
        back->addTri(a, b, c, norm);
        back->addTexTri(a, b, c, rad);

//        back->addQuad(QVector3D(0, 0, -depth), out_back[i], out_back[(i + 1) % k], QVector3D(0, 0, -depth));
    }

    Patch *os = new Patch(g);
    for (int i = 0; i < k; ++i) {
	os->addQuad(out_back[(i + 1) % k], out_back[i], outside[i], outside[(i + 1) % k]);
        g->textures  << QVector2D(1.0f, 0.0f)  << QVector2D(1.0f, 1.0f)  << QVector2D(0.0f, 1.0f) << QVector2D(0.0f, 0.0f)     ;
    }

    parts << front << back << os;
}

//============================

RectPrism::RectPrism(Geometry *g, qreal width, qreal height, qreal depth)
{
    enum { bl, br, tr, tl };
    Patch *fb = new Patch(g);
//    fb->setSmoothing(Patch::Faceted);

    // front face
    QVector<QVector3D> r(4);
    r[br].setX(width);
    r[tr].setX(width);
    r[tr].setY(height);
    r[tl].setY(height);
    QVector3D adjToCenter(-width / 2.0, -height / 2.0, depth / 2.0);
    for (int i = 0; i < 4; ++i)
	r[i] += adjToCenter;
    fb->addQuad(r[bl], r[br], r[tr], r[tl]);

    // back face
    QVector<QVector3D> s = extrude(r, depth);
    fb->addQuad(s[tl], s[tr], s[br], s[bl]);

    // side faces
    Patch *sides = new Patch(g);
    //    sides->setSmoothing(Patch::Faceted);
    sides->addQuad(s[bl], s[br], r[br], r[bl]);
    sides->addQuad(s[br], s[tr], r[tr], r[br]);
    sides->addQuad(s[tr], s[tl], r[tl], r[tr]);
    sides->addQuad(s[tl], s[bl], r[bl], r[tl]);

    parts << fb << sides;
}

//========================

RectMarks::RectMarks(Geometry *g, qreal rad, qreal width, qreal height, qreal depth)
{
    QVector3D z(0, 0, 1);

    for (int i = 0; i < 12; i++) {
        RectPrism markH (g, width, height, depth);
	qreal angel = - i * 30.0;
	QVector3D place(qCos(M_PI * angel / 180.0) * rad, qSin(M_PI * angel / 180.0) * rad, 0);
	markH.translate(place);
	markH.rotate(90 + angel, z);
	parts << markH.parts;
    }

}

//========================

RectSecondHand::RectSecondHand(Geometry *g, qreal edge, qreal x_base, qreal y_base, qreal depth)
{
    time = QTime::currentTime();
    z = QVector3D(0.0, 0.0, 1.0);

    RectPrism hand(g, edge, x_base, depth);
    RectPrism pend(g, x_base, y_base, depth);

    hand.rotate(90-6.0 * time.second(), z);
    placeHand = QVector3D(0.8 * edge / 2.0, 0, 0);
    hand.translate(placeHand);

    pend.rotate(90-6.0 * time.second(), z);
    placePend = QVector3D(-x_base/2.0 - 0.2 * edge / 2.0, 0, 0);
    pend.translate(placePend);

    parts << hand.parts << pend.parts ;
}

//================
RectMinuteHand::RectMinuteHand(Geometry *g, qreal x, qreal y, qreal depth)
{
    time = QTime::currentTime();
    z = QVector3D(0.0, 0.0, 1.0);

    RectPrism hand(g, x, y, depth);

    hand.rotate(90 - 6.0 * (time.minute() + time.second() / 60.0), z);
    QVector3D placeHand(x / 2.0, 0, 0);
    hand.translate(placeHand);

    parts << hand.parts;
}

//===================

RectHourHand::RectHourHand(Geometry *g, qreal x, qreal y, qreal depth)
{
    time = QTime::currentTime();
    z = QVector3D(0.0, 0.0, 1.0);

    RectPrism hand(g, x, y, depth);


    hand.rotate(90 - 30.0 * ((time.hour() + time.minute() / 60.0)), z);
    QVector3D placeHand(x / 2.0, 0, 0);
    hand.translate(placeHand);

    parts << hand.parts;
}

//===================
