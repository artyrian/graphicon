#include <QGLWidget>
#include <QMatrix4x4>

#include <iostream>

#include "MyWatch.h"
#include "figures.h"

QColor silverColor(110,110,110);
QColor handColor(162, 162, 162);

QColor glassColor(0.1 * 255, 0.3 * 255, 0.2 * 255, 0.1 * 255);

MyWatch::MyWatch(QObject *parent)
    : QObject(parent),
      geometryWithTextures1(new Geometry),
      geometryWithTextures2(new Geometry),
      geometryWithTextures3(new Geometry),
      geometryWithTextures4(new Geometry),
      dynGeom(new Geometry()),
      geometryWithoutTextures(new Geometry()),
      geometryGlass(new Geometry())
{
    glGenTextures(count_textures, textureID);

    TextureInit("../textures/face.jpg", 0);
    TextureInit("../textures/back.jpg", 1);
    TextureInit("../textures/belt.jpg", 2);
    TextureInit("../textures/stell.jpg", 3);
    TextureInit("../textures/1.bmp", 4);

    buildGeometryWithoutTextures(geometryWithoutTextures, partsWithoutTextures);
    buildGeometry();
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

void initParameters()
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // задаём линейную фильтрацию вдали:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // задаём: при фильтрации игнорируются тексели, выходящие за границу текстуры для s координаты
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    // задаём: при фильтрации игнорируются тексели, выходящие за границу текстуры для t координаты
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    // задаём: цвет текселя полностью замещает цвет фрагмента фигуры
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void MyWatch::TextureInit(const char *path, int i)
{
    QImage img1(path);
    if (img1.isNull()) {
        std::cout << "Error load image." << std::endl;
        return;
    }

    img1 = QGLWidget::convertToGLFormat(img1);

    glBindTexture(GL_TEXTURE_2D, textureID[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, (GLsizei)img1.width(), (GLsizei)img1.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img1.bits());

    initParameters();
}

void MyWatch::reggi_time()
{
    TextureInit("1.bmp", 0);
}

void MyWatch::buildGeometry()
{

    RectSolidCylindre body(geometryWithTextures1, body_rad, body_depth, numSectors);
    body.translate(QVector3D(0, 0, -body_depth / 2.0));
    partsWithTextures1 << body.parts ;
    geometryWithTextures1->finalize();

    //=====

    RectSolidCylindre cover(geometryWithTextures2, rim_orad, back_rim_depth, numSectors);
    cover.translate(QVector3D(0, 0, - body_depth  - 0.5 * back_rim_depth - 0.003));
    cover.rotate(180, QVector3D(0, 1, 0));
    partsWithTextures2 << cover.parts;
    geometryWithTextures2->finalize();

    //======

    BottomBelt b_belt(geometryWithTextures3);
    TopBelt t_belt(geometryWithTextures3);
    partsWithTextures3<< b_belt.parts << t_belt.parts;
    geometryWithTextures3->finalize();

    //======


    RectTorus outside_body(geometryWithTextures4, body_rad - 0.001, body_rad + 0.001, body_depth, numSectors);
    outside_body.translate(QVector3D(0, 0, -body_depth / 2.0));

    RectTriangleTorus rim(geometryWithTextures4, rim_irad, rim_orad, rim_depth, numSectors);
    rim.translate(QVector3D(0, 0, rim_depth/2.0 + rim2_depth));

    RectTriangleTorus rim2 (geometryWithTextures4, rim_orad, body_rad, rim2_depth, numSectors);
    rim2.translate(QVector3D(0, 0, rim2_depth/2.0));

    Benches benches(geometryWithTextures4, benches_widht, benches_height, benches_depth, benches_border);
    benches.translate(QVector3D(0, 0, - body_depth/2.0));

    RectTriangleTorus back_rim(geometryWithTextures4, rim_orad, body_rad, back_rim_depth, numSectors);
    back_rim.translate(QVector3D(0, 0, - body_depth - 0.5 * back_rim_depth));
    back_rim.rotate(180, QVector3D(0, 1, 0));

    partsWithTextures4<< outside_body.parts << rim.parts << rim2.parts << benches.parts << back_rim.parts;
    geometryWithTextures4->finalize();
}

void MyWatch::buildGeometryWithoutTextures(Geometry *g, QList <Patch*> &parts)
{
    RectSolidCylindre axis(g, axis_rad, axis_depth, numSectors);
    axis.translate(QVector3D(0, 0, axis_depth));

    RectMarks marks(g, marks_rad, marks_width, marks_height, marks_depth);

    Clip clip(g);

    RectSolidCylindre winchS(g, winch_rad / 2.0, winch_depth, numSectors);
    winchS.rotate(90, QVector3D(0, 1, 0));
    winchS.translate(QVector3D(body_depth/2.0 , 0, body_rad +  winch_depth / 2.0 - 0.01));


    parts << axis.parts << marks.parts
          << winchS.parts
          << clip.parts
            ;

    setColor(silverColor, parts);

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


void MyWatch::draw(bool anim)
{
    drawWithTexturesi(geometryWithTextures1, partsWithTextures1, 0);
    drawWithTexturesi(geometryWithTextures2, partsWithTextures2, 1);
    drawWithTexturesi(geometryWithTextures3, partsWithTextures3, 2);
    drawWithTexturesi(geometryWithTextures4, partsWithTextures4, 3);

    drawWithoutTextures();
    dynamicDraw(anim);
    drawGlass();
}

void MyWatch::drawWithTexturesi(Geometry *g, const QList<Patch *> &parts, int k)
{
    glBindTexture(GL_TEXTURE_2D, textureID[k]);

    g->loadArrays();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnable(GL_TEXTURE_2D);

    for (int i = 0; i < parts.count(); ++i) {
        parts[i]->draw();
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

    setColor(silverColor, dynamicParts);

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

    setColor(glassColor, partsGlass);
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
