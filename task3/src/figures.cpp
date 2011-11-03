#include "figures.h"

#include <QImage>
#include <iostream>
#include <qmath.h>
#include <resources.h>

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

QVector2D toUnit2D(const QVector3D &a);

/* Normalize all normals in vector to length 1.0 */
void Geometry::finalize()
{
    for (int i = 0; i < normals.count(); ++i) {
	normals[i].normalize();
    }
}

QVector2D toUnit2D(const QVector3D &a)
{
    QVector2D e(a.x(), a.y());
    e.normalize();
    e.setX(e.x()/2.0 + 0.5);
    e.setY(e.y()/2.0 + 0.5);

    return e;
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
        //
        textures.append(toUnit2D(a));
        //
    }

    faces.append(v);
}

void Geometry::appendFaceted(const QVector3D &a, const QVector3D &n)
{
    int v = vertices.count();
    vertices.append(a);
    normals.append(n);
    textures.append(toUnit2D(a));
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
    qSetColor(faceColor, QColor(Qt::lightGray));
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

Winch::Winch(Geometry *g, qreal iRad, qreal depth)
{
    QVector3D x(1, 0, 0);
    QVector3D y(0, 1, 0);
    QVector3D z(0, 0, 1);
    RectSolidCylindre disk(g, iRad, depth + 0.01, numSectors);
    disk.rotate(90, y);
    disk.translate(QVector3D(body_depth / 2.0, 0, 0));

    for (int i = 0; i < num_winch_tri; i++) {
        RectTri tri(g, winch_ledge, winch_ledge / 2.0, depth);
        tri.translate(QVector3D(0, 0, -body_depth/2.0));
        tri.rotate(i * 360 / num_winch_tri, x);
        tri.translate(QVector3D( 0, iRad, 0));
        tri.rotate(90, y);

        parts << tri.parts;
    }

    parts << disk.parts;
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
 //   QVector2D p1(a.x() / (2 * fullLength) + 0.5, a.y() / (2 * fullLength) + 0.5);
    QVector2D p1(a.x(), a.y());
    p1.normalize();
    p1.setX(p1.x()/2.0 + 0.5);
    p1.setY(p1.y()/2.0 + 0.5);

    QVector2D p2(b.x() / (2 * fullLength) + 0.5, b.y() / (2 * fullLength) + 0.5);
    QVector2D p3(c.x() / (2 * fullLength) + 0.5, c.y() / (2 * fullLength) + 0.5);

    geom->textures << p1 << p2 << p3  ;
}

RectSolidCylindre::RectSolidCylindre(Geometry *g, qreal rad, qreal depth, int k)
{
    QVector<QVector3D> outside;
    for (int i = 0; i < k; ++i) {
	qreal angle = (i * 2 * M_PI) / k;
        outside << QVector3D(rad * qSin(angle), rad * qCos(angle), depth / 2.0);
    }

    QVector<QVector3D> out_back = extrude(outside, depth);

    Patch *front = new Patch(g);
    for (int i = 0; i < k; ++i) {
        QVector3D a = outside[i];
        QVector3D b = QVector3D(0, 0, depth / 2.0);
        QVector3D c = outside[(i+1)%k];
        QVector3D norm = QVector3D::normal(a, b, c);
        front->addTri(a, b, c, norm);
    }

    Patch *back = new Patch(g);
    for (int i = 0; i < k; ++i) {
        QVector3D a = QVector3D(0, 0, -depth / 2.0);
        QVector3D b = out_back[i];
        QVector3D c = out_back[(i + 1) % k];
        QVector3D norm = QVector3D::normal(a, b, c);
        back->addTri(a, b, c, norm);
    }

    Patch *os = new Patch(g);
    for (int i = 0; i < k; ++i) {
	os->addQuad(out_back[(i + 1) % k], out_back[i], outside[i], outside[(i + 1) % k]);
    }

    parts << front << back << os;
}

//============================


Benches::Benches(Geometry *g, qreal widht, qreal height, qreal depth, qreal border)
{
    RectPrism benches(g, widht, height, depth);

    qreal border_y = border / 2;

    RectPrism2 lt(g, border_y, border, depth, - prism_delta);
    lt.translate(QVector3D(-widht/2.0 + border / 2.0, height/2.0  + border_y / 2.0, 0));
    lt.rotate(90, QVector3D(0, 0, 1));

    RectPrism2 rt(g, border_y, border, depth, - prism_delta);
    rt.translate(QVector3D(widht/2.0 - border / 2.0, height/2.0 + border_y / 2.0, 0));
    rt.rotate(90, QVector3D(0, 0, 1));

    RectPrism2 lb(g, border_y, border, depth, - prism_delta);
    lb.translate(QVector3D(-widht/2.0 + border / 2.0, -height/2.0 - border_y / 2.0, 0));
    lb.rotate(-90, QVector3D(0, 0, 1));

    RectPrism2 rb(g, border_y, border, depth, - prism_delta);
    rb.translate(QVector3D(widht/2.0 - border / 2.0 , -height/2.0 - border_y / 2.0, 0));
    rb.rotate(-90, QVector3D(0, 0, 1));

    RectTri triLeft(g, tri_width, tri_height, depth);
    triLeft.translate(QVector3D(-benches_widht / 2.0 - tri_height / 2.0, 0, 0));
    triLeft.rotate(90, QVector3D(0, 0, 1));

    RectTri triRight(g, tri_width, tri_height, depth);
    triRight.translate(QVector3D(benches_widht / 2.0 + tri_height / 2.0, 0, 0));
    triRight.rotate(-90, QVector3D(0, 0, 1));

    RectSolidCylindre perchTop(g, perch_rad, widht - 2 * border_y, numSectors);
    perchTop.translate(QVector3D(body_depth / 2.0, height/2.0 + border_y / 2.0, - (body_depth + prism_delta) / 2.0));
    perchTop.rotate(90, QVector3D(0, 1, 0));

    RectSolidCylindre perchBottom(g, perch_rad, widht - 2 * border_y, numSectors);
    perchBottom.translate(QVector3D(body_depth / 2.0, -(height/2.0 + border_y / 2.0), - (body_depth + prism_delta) / 2.0));
    perchBottom.rotate(90, QVector3D(0, 1, 0));



    parts << benches.parts
          << lt.parts
          << lb.parts
          << rt.parts
          << rb.parts
          << triLeft.parts
          << triRight.parts

          << perchTop.parts
          << perchBottom.parts
             ;

}

//============================

RectPrism::RectPrism(Geometry *g, qreal width, qreal height, qreal depth)
{
    enum { bl, br, tr, tl };
    Patch *fb = new Patch(g);
    fb->setSmoothing(false);

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
    sides->setSmoothing(false);
    sides->addQuad(s[bl], s[br], r[br], r[bl]);
    sides->addQuad(s[br], s[tr], r[tr], r[br]);
    sides->addQuad(s[tr], s[tl], r[tl], r[tr]);
    sides->addQuad(s[tl], s[bl], r[bl], r[tl]);

    parts << fb << sides;
}

//========================

RectTri::RectTri(Geometry *g, qreal width, qreal height, qreal depth)
{
    enum { top, left, right };
    Patch *fb = new Patch(g);
    fb->setSmoothing(false);

    QVector3D a(0, height / 2.0, depth / 2.0);
    QVector3D b(-width / 2.0, -height / 2.0, depth / 2.0);
    QVector3D c(width / 2.0, - height / 2.0, depth / 2.0);
    QVector<QVector3D> r;
    r << a << b << c;
    QVector3D norm = QVector3D::normal(a, b, c);
    fb->addTri(r[top], r[left], r[right], norm);

    // back face
    QVector<QVector3D> s = extrude(r, depth);
    norm = QVector3D::normal(s[top], s[right], s[left]);
    fb->addTri(s[top], s[right], s[left], norm);

    // side faces
    Patch *sides = new Patch(g);
    sides->setSmoothing(false);
    sides->addQuad(s[left], s[right], r[right], r[left]);
    sides->addQuad(s[right], s[top], r[top], r[right]);
    sides->addQuad(s[top], s[left], r[left], r[top]);

    parts << fb << sides;
}

//========================


RectPrism2::RectPrism2(Geometry *g, qreal width, qreal height, qreal depth, qreal delta)
{
    enum { bl, br, tr, tl };
    Patch *fb = new Patch(g);
    fb->setSmoothing(false);

    // front face
    QVector<QVector3D> r(4);
    r[br].setX(width);
    r[br].setZ(delta);
    r[tr].setX(width);
    r[tr].setZ(delta);
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
    sides->setSmoothing(false);
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
        RectPrism markHq(g, mark_q_width, mark_q_width, depth);

        qreal angel = - i * 30.0;
        QVector3D place(qCos(M_PI * angel / 180.0) * rad, qSin(M_PI * angel / 180.0) * rad, 0);
	markH.translate(place);
        markH.rotate(90 + angel, z);

        QVector3D placeQ(qCos(M_PI * angel / 180.0) * (rad + width + height ), qSin(M_PI * angel / 180.0) * (rad + width + height ), 0);
        markHq.translate(placeQ);
        markHq.rotate(90 + angel,z);
        parts << markH.parts << markHq.parts;
    }


    for (int i = 0; i < 60; i++) {
        RectPrism markS(g, 0.01, 0.002, depth);
        qreal angel = i * 6.0;

        QVector3D place(qCos(M_PI * angel / 180.0) * (rad + width + height ), qSin(M_PI * angel / 180.0) * (rad + width + height ), 0);
        markS.translate(place);
        markS.rotate(angel, z);

        parts << markS.parts;
    }

}

//========================

RectSecondHand::RectSecondHand(Geometry *g, qreal edge, qreal x_base, qreal y_base, qreal depth, bool isAnimation)
{
    time = QTime::currentTime();
    z = QVector3D(0.0, 0.0, 1.0);

    RectPrism hand(g, edge, x_base, depth);
    RectPrism pend(g, x_base, y_base, depth);
    RectSolidCylindre disk(g, 1.25 * x_base, depth, numSectors);

    qreal angel;
    if ( isAnimation ) {
        angel = 90 - 360 * time.msec() / 1000;
    } else {
        angel = 90 - 6.0 * time.second();
    }

    hand.rotate(angel, z);
    placeHand = QVector3D(0.8 * edge / 2.0, 0, 0);
    hand.translate(placeHand);

    pend.rotate(angel, z);
    placePend = QVector3D(-x_base/2.0 - 0.2 * edge / 2.0, 0, 0);
    pend.translate(placePend);

    parts << hand.parts << pend.parts << disk.parts ;
}

//================
RectMinuteHand::RectMinuteHand(Geometry *g, qreal x, qreal y, qreal depth, bool isAnimation)
{
    time = QTime::currentTime();
    qreal angel;

    if (isAnimation) {
        angel = 90 - 6.0 * time.second() - 6 *  time.msec() / 1000;
        angel *= 10;
    } else {
        angel = 90 - 6.0 * time.minute()  - time.second() / 60.0;
    }
    z = QVector3D(0.0, 0.0, 1.0);

    RectPrism hand(g, x, y, depth);
    RectSolidCylindre disk(g, 1.1 * y, depth, numSectors);
    RectTri tri(g, y, y, depth);

    hand.rotate(angel, z);
    tri.rotate(angel, z);

    QVector3D placeHand(x / 2.0, 0, 0);
    hand.translate(placeHand);
    tri.translate(QVector3D(x + y / 2.0, 0, 0));
    tri.rotate(-90, z);

    parts << hand.parts << disk.parts << tri.parts;
}

//===================

RectHourHand::RectHourHand(Geometry *g, qreal x, qreal y, qreal depth, bool isAnimation)
{
    time = QTime::currentTime();

    qreal angel;
    if (isAnimation) {
        angel = 90 - 6.0 * time.minute()  - time.second() / 60.0;
        angel *= 100;
    } else {
        angel = 90 - 30.0 * ((time.hour() + time.minute() / 60.0));
    }

    z = QVector3D(0.0, 0.0, 1.0);

    RectPrism hand(g, x, y, depth);
    RectSolidCylindre disk(g, 0.75 * y, depth, numSectors);
    RectTri tri(g, y, y, depth);

    hand.rotate(angel, z);
    tri.rotate(angel, z);

    QVector3D placeHand(x / 2.0, 0, 0);
    hand.translate(placeHand);
    tri.translate(QVector3D(x + y / 2.0, 0, 0));
    tri.rotate(-90, z);

    parts << hand.parts << disk.parts << tri.parts;
}

//===================

BottomBelt::BottomBelt(Geometry *g)
{
    RectPrism down1(g, belt_width, belt_bottom_height, belt_depth);
    down1.translate(QVector3D(0,  - belt_bottom_height - 0.12,  - belt_depth - benches_depth /2.0 + 0.003));

    RectPrism down2(g, belt_width, belt_after_hole, belt_depth);
    down2.translate(QVector3D(0,  - belt_bottom_height/2.0 - 0.01,  - belt_depth - benches_depth /2.0 + 0.003));
    down2.translate(QVector3D(0, - (belt_bottom_height +count_holes_in_belt * (belt_near_hole_height + 2* belt_hole)) - belt_after_hole/2.0 + 2* belt_hole, 0));

    RectSolidCylindre disk(g, belt_width/2.0, belt_depth, numSectors);
    disk.translate(QVector3D(0,  - belt_bottom_height/2.0 - 0.01,  - belt_depth - benches_depth /2.0 + 0.003));
    disk.translate(QVector3D(0, - (belt_bottom_height +count_holes_in_belt * (belt_near_hole_height + 2* belt_hole)) - belt_after_hole/2.0 + 2*belt_hole - belt_width/2.0, 0));


    for (int i = 0; i < count_holes_in_belt; i++) {
        RectBeltTorus hole(g);
        hole.translate(QVector3D(0,  - belt_bottom_height/2.0 - 0.01,  - belt_depth - benches_depth /2.0 + 0.003));

        hole.translate(QVector3D(0, - (belt_bottom_height +i * (belt_near_hole_height + 2* belt_hole)), 0));
        parts << hole.parts;
    }

    parts << down1.parts << down2.parts << disk.parts;
}

TopBelt::TopBelt(Geometry *g)
{
    RectPrism top(g, belt_width, belt_top_height, belt_depth);
    top.translate(QVector3D(0, belt_top_height + 0.01,  - belt_depth - benches_depth /2.0 + 0.003));
    RectSolidCylindre end(g,belt_depth/ 2.0 + 0.01, belt_width, numSectors);
    end.translate(QVector3D(0, 3 * belt_top_height / 2.0 + 0.01 - belt_depth/2.0 ,  - belt_depth - benches_depth /2.0 + 0.003));
    end.rotate(90, QVector3D(0, 1, 0));

    RectPrism ls(g,belt_depth, belt_width/2.0, 2 * belt_depth);
    RectPrism rs(g,belt_depth, belt_width/2.0, 2 * belt_depth);
    RectPrism ts(g,belt_width +  belt_depth, 3 * belt_depth, 2 * belt_depth);
    ls.translate(QVector3D(- belt_width / 2.0, 3 * belt_top_height / 2.0 + 0.01 - belt_depth/2.0 + belt_width/4.0 ,  - belt_depth - benches_depth /2.0 + 0.003));
    rs.translate(QVector3D(belt_width / 2.0, 3 * belt_top_height / 2.0 + 0.01 - belt_depth/2.0 + belt_width/4.0 ,  - belt_depth - benches_depth /2.0 + 0.003));
    ts.translate(QVector3D(0, 3 * belt_top_height / 2.0 + 0.01 - belt_depth/2.0 + belt_width/2.0   ,  - belt_depth - benches_depth /2.0 + 0.003));

    RectSolidCylindre s(g, belt_hole/2.0, belt_width/2.0 + belt_depth, numSectors);
    s.translate(QVector3D(0, 3 * belt_top_height / 2.0 + 0.01 - belt_depth/2.0 + belt_width/2.0  - (belt_width/2.0 + belt_depth) / 2.0 ,  - belt_depth - benches_depth /2.0 +  + 0.01));
    s.rotate(-80, QVector3D(1, 0, 0));

    RectEllipseTorus ell(g, belt_width, belt_width + belt_depth, ell_depth, numSectors);
    ell.translate(QVector3D(0, belt_top_height, - body_depth / 2.0 - belt_depth ));
    ell.rotate(90, QVector3D(1, 0, 0));

    RectEllipseTorus ell2(g, belt_width, belt_width + belt_depth, ell_depth, numSectors);
    ell2.translate(QVector3D(0, 1.5 * belt_top_height - 0.02, - body_depth / 2.0 + belt_depth / 2.0 ));
    ell2.rotate(90, QVector3D(1, 0, 0));


    parts << top.parts << end.parts << ls.parts << rs.parts << ts.parts << s.parts
             << ell.parts << ell2.parts
             ;

}

RectBeltTorus::RectBeltTorus(Geometry *g)
{
    RectPrism full(g, belt_width, belt_near_hole_height, belt_depth);
    RectPrism lnh(g, belt_width / 2.0 - belt_hole, 4 * belt_hole, belt_depth);
    RectPrism rnh(g, belt_width / 2.0 - belt_hole, 4 * belt_hole, belt_depth);
    RectTorus hole(g, belt_hole, 2 * belt_hole, belt_depth, numSectors);
    lnh.translate(QVector3D(0, -(belt_near_hole_height/2.0 + belt_hole) , 0));
    lnh.translate(QVector3D(belt_width / 4.0 + belt_hole / 2.0, 0, 0));

    rnh.translate(QVector3D(0, -(belt_near_hole_height/2.0 + belt_hole), 0));
    rnh.translate(QVector3D(-(belt_width / 4.0 + belt_hole / 2.0), 0, 0));

    hole.translate(QVector3D(0, -(belt_near_hole_height/2.0 + belt_hole), 0));

    parts << full.parts
          << lnh.parts
          << rnh.parts
          << hole.parts;
}

RectEllipseTorus::RectEllipseTorus(Geometry *g, qreal iRad, qreal oRad, qreal depth, int k)
{
    QVector<QVector3D> inside;
    QVector<QVector3D> outside;
    for (int i = 0; i < k; ++i) {
        qreal angle = (i * 2 * M_PI) / k;
        inside << QVector3D(iRad * ellipse_a * qSin(angle), iRad * ellipse_b * qCos(angle), depth/2.0);
        outside << QVector3D(oRad * ellipse_a * qSin(angle), oRad * ellipse_b *qCos(angle), depth/2.0);
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
