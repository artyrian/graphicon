#include <QGLWidget>
#include <QMatrix4x4>

#include <iostream>

#include "MyWatch.h"
#include "figures.h"
#include "resources.h"

MyWatch::MyWatch(QObject *parent)
    : QObject(parent), geometryWithTextures(new Geometry), dynGeom(new Geometry()), geometryWithoutTextures(new Geometry()), geometryGlass(new Geometry())
{
    buildGeometryWithoutTextures(geometryWithoutTextures, partsWithoutTextures);
    buildGeometry(geometryWithTextures, partsWithTextures);
    buildGlass(geometryGlass, partsGlass);
}

MyWatch::~MyWatch()
{
}

void MyWatch::setColor(QColor c, QList<Patch *> &parts)
{
    for (int i = 0; i < parts.count(); ++i) {
	qSetColor(parts[i]->faceColor, c);
    }
}

void MyWatch::TextureInit()
{
    QImage img;
    img.load("1.bmp");

    if (img.isNull()) {
        std::cout << "Error load image." << std::endl;
        return;
    }
    img = QGLWidget::convertToGLFormat(img);

    glGenTextures(1, textureID);
    glBindTexture(GL_TEXTURE_2D, textureID[0]);
       // связываем текстурный объект с изображением

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // задаём линейную фильтрацию вдали:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // задаём: при фильтрации игнорируются тексели, выходящие за границу текстуры для s координаты
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    // задаём: при фильтрации игнорируются тексели, выходящие за границу текстуры для t координаты
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    // задаём: цвет текселя полностью замещает цвет фрагмента фигуры
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, (GLsizei)img.width(), (GLsizei)img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
}

void MyWatch::buildGeometry(Geometry *g, QList <Patch*> &parts)
{
    TextureInit();

    RectSolidCylindre body(g, body_rad, body_depth, numSectors);
    body.translate(QVector3D(0, 0, -body_depth / 2.0));

    RectSolidCylindre axis(g, axis_rad, axis_depth, numSectors);
    axis.translate(QVector3D(0, 0, axis_depth));

    parts << body.parts
          << axis.parts
             ;

    g->finalize();
}

void MyWatch::buildGeometryWithoutTextures(Geometry *g, QList <Patch*> &parts)
{
    RectTorus outside_body(g, body_rad - 0.001, body_rad + 0.001, body_depth, numSectors);
    outside_body.translate(QVector3D(0, 0, -body_depth / 2.0));


    RectMarks marks(g, marks_rad, marks_width, marks_height, marks_depth);

    RectTriangleTorus rim (g, rim_irad, rim_orad, rim_depth, numSectors);
    rim.translate(QVector3D(0, 0, rim_depth/2.0 + rim2_depth));

    RectTriangleTorus rim2 (g, rim_orad, body_rad, rim2_depth, numSectors);
    rim2.translate(QVector3D(0, 0, rim2_depth/2.0));

    Benches benches(g, benches_widht, benches_height, benches_depth, benches_border);
    benches.translate(QVector3D(0, 0, - body_depth/2.0));

    RectTriangleTorus back_rim(g, rim_orad, body_rad, back_rim_depth, numSectors);
    back_rim.translate(QVector3D(0, 0, - body_depth - 0.5 * back_rim_depth));
    back_rim.rotate(180, QVector3D(0, 1, 0));

    RectSolidCylindre cover(g, rim_orad, back_rim_depth, numSectors);
    cover.translate(QVector3D(0, 0, - body_depth  - 0.5 * back_rim_depth - 0.003));
    cover.rotate(180, QVector3D(0, 1, 0));

    RectSolidCylindre winchS(g, winch_rad / 2.0, winch_depth, numSectors);
    winchS.rotate(90, QVector3D(0, 1, 0));
    winchS.translate(QVector3D(body_depth/2.0 , 0, body_rad +  winch_depth / 2.0 - 0.01));

    BottomBelt b_belt(g);
    TopBelt t_belt(g);

    parts
          << outside_body.parts
          << marks.parts
          << rim.parts << rim2.parts
          << benches.parts
          << back_rim.parts
          << cover.parts
          << winchS.parts
          << b_belt.parts
          << t_belt.parts
            ;

    g->finalize();
}

void MyWatch::dynamicGeometry(bool isAnimation)
{
    dynamicParts.clear();
    delete dynGeom;
    dynGeom = new Geometry();

    QVector3D z(0.0, 0.0, 1.0);

    Winch winch(dynGeom, winch_rad, winch_depth);
    if (isAnimation) {
        winch.translate(QVector3D(0 , 0, body_rad +  1.3 * winch_depth ));
    } else {
        winch.translate(QVector3D(0 , 0, body_rad +  winch_depth / 2.0));
    }

    RectSecondHand second (dynGeom, second_edge, second_x_base, second_y_base, second_depth, isAnimation);
    second.translate(QVector3D(0,0, axis_depth + second_depth - 0.001));

    RectMinuteHand minute (dynGeom, minute_x_base, minute_y_base, minute_depth, isAnimation);
    minute.translate(QVector3D(0,0, 2 * axis_depth / 3.0 + minute_depth / 2.0 ));

    RectHourHand hour (dynGeom, hour_x_base, hour_y_base, hour_depth, isAnimation);
    hour.translate(QVector3D(0,0, axis_depth / 3.0));

    dynamicParts << second.parts << minute.parts << hour.parts << winch.parts;


    for (int i = 0; i < dynamicParts.count(); ++i) {
	qSetColor(dynamicParts[i]->faceColor, QColor(0, 255,0));
    }

    dynGeom->finalize();
}

void MyWatch::createTextureArray()
{
    bool changed = false;
    int from = 0;

    for (int i = 0; i < partsWithTextures.count(); ++i) {
        if (changed) {
            // find next

            from = i;
            changed = false;
        } else {
            // find in
        }
    }
}

void MyWatch::draw(bool anim)
{
    drawWithTextures();
    drawWithoutTextures();
    dynamicDraw(anim);
    drawGlass();
}

void MyWatch::drawWithTextures()
{
    geometryWithTextures->loadArrays();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnable(GL_TEXTURE_2D);

    for (int i = 0; i < partsWithTextures.count(); ++i) {
        partsWithTextures[i]->draw();
    }

    glDisable(GL_TEXTURE_2D);

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

void MyWatch::drawWithoutTextures()
{
    geometryWithoutTextures->loadArrays();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);


    for (int i = 0; i < partsWithoutTextures.count(); ++i) {
        partsWithoutTextures[i]->draw();
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

}

void MyWatch::dynamicDraw(bool isAnimation)
{
    dynamicGeometry(isAnimation);

    dynGeom->loadArrays();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    for (int i = 0; i < dynamicParts.count(); ++i) {
	dynamicParts[i]->draw();
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

void MyWatch::buildGlass(Geometry *g, QList <Patch*> &parts)
{
    RectSolidCylindre glass(g, rim_irad, glass_depth, numSectors);
    glass.translate(QVector3D(0, 0, rim_depth / 2.0 + rim2_depth));

    parts << glass.parts;
}

void MyWatch::drawGlass()
{
    /* alfa-mode */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);

    setColor(QColor(250, 237, 82, 50), partsGlass);

    geometryGlass->loadArrays();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    for (int i = 0; i < partsGlass.count(); ++i) {
        partsGlass[i]->draw();
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    glDisable(GL_BLEND);
}
