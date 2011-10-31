#include <QGLWidget>
#include <QMatrix4x4>

#include <iostream>

#include "MyWatch.h"
#include "figures.h"
#include "resources.h"

MyWatch::MyWatch(QObject *parent, float scale)
    : QObject(parent), geom(new Geometry), dynGeom(new Geometry())
{
    buildGeometry();

}

MyWatch::~MyWatch()
{
}

void MyWatch::setColor(QColor c)
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // задаём: при фильтрации игнорируются тексели, выходящие за границу текстуры для t координаты
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // задаём: цвет текселя полностью замещает цвет фрагмента фигуры
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, (GLsizei)img.width(), (GLsizei)img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());

}

void MyWatch::buildGeometry()
{
    TextureInit();

    RectSolidCylindre body (geom, body_rad, body_depth, numSectors);

    RectSolidCylindre axis (geom, axis_rad, axis_depth, numSectors);
    axis.translate(QVector3D(0, 0, axis_depth));

    RectMarks marks (geom, marks_rad, marks_width, marks_height, marks_depth);

    parts << marks.parts << body.parts << axis.parts;

    geom->finalize();
}

void MyWatch::dynamicGeometry()
{
    dynamicParts.clear();
    delete dynGeom;
    dynGeom = new Geometry();

    QVector3D z(0.0, 0.0, 1.0);

    RectSecondHand second (dynGeom, second_edge, second_x_base, second_y_base, second_depth);
    second.translate(QVector3D(0,0, axis_depth));

    RectMinuteHand minute (dynGeom, minute_x_base, minute_y_base, minute_depth);
    minute.translate(QVector3D(0,0, 2 * axis_depth / 3.0));

    RectHourHand hour (dynGeom, hour_x_base, hour_y_base, hour_depth);
    hour.translate(QVector3D(0,0, axis_depth / 3.0));

    dynamicParts << second.parts << minute.parts << hour.parts;


    for (int i = 0; i < dynamicParts.count(); ++i) {
	qSetColor(dynamicParts[i]->faceColor, QColor(0, 255,0));
    }

    dynGeom->finalize();
}

void CoordAxis()
{
    glBegin(GL_LINE);
	glColor3i(0, 255, 0);
	glVertex3d(0,0,0);
	glVertex3d(0.5,0,0);
	glVertex3d(0,0,0);
	glVertex3d(0,1,0);
	glVertex3d(0,0,0);
	glVertex3d(0,0,1.5);
    glEnd();
/*
    glBegin(GL_QUADS);

                                    // Передняя грань
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.1f, -0.1f,  0.1f);	// Низ лево
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.1f, -0.1f,  0.1f);	// Низ право
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.1f,  0.1f,  0.1f);	// Верх право
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.1f,  0.1f,  0.1f);	// Верх лево

                                    // Задняя грань
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.1f, -0.1f, -0.1f);	// Низ право
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.1f,  0.1f, -0.1f);	// Верх право
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 0.1f,  0.1f, -0.1f);	// Верх лево
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.1f, -0.1f, -0.1f);	// Низ лево

                                    // Верхняя грань
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.1f,  0.1f, -0.1f);	// Верх лево
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.1f,  0.1f,  0.1f);	// Низ лево
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.1f,  0.1f,  0.1f);	// Низ право
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.1f,  0.1f, -0.1f);	// Верх право

                                    // Нижняя грань
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.1f, -0.1f, -0.1f);	// Верх право
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 0.1f, -0.1f, -0.1f);	// Верх лево
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.1f, -0.1f,  0.1f);	// Низ лево
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.1f, -0.1f,  0.1f);	// Низ право

                                    // Правая грань
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.1f, -0.1f, -0.1f);	// Низ право
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.1f,  0.1f, -0.1f);	// Верх право
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 0.1f,  0.1f,  0.1f);	// Верх лево
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.1f, -0.1f,  0.1f);	// Низ лево

                                    // Левая грань
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.1f, -0.1f, -0.1f);	// Низ лево
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.1f, -0.1f,  0.1f);	// Низ право
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.1f,  0.1f,  0.1f);	// Верх право
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.1f,  0.1f, -0.1f);	// Верх лево

    glEnd();
*/

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.1f, -0.1f,  0.1f);	// Низ лево
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.2f, -0.2f,  0.1f);	// Низ право
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.3f,  0.3f,  0.1f);	// Верх право
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.4f,  0.4f,  0.1f);	// Верх лево
    glEnd();
}

void MyWatch::draw()
{
    CoordAxis();

    geom->loadArrays();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    for (int i = 0; i < parts.count(); ++i) {
	parts[i]->draw();
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    dynamicDraw();
}


void MyWatch::dynamicDraw()
{
    dynamicGeometry();

    dynGeom->loadArrays();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    for (int i = 0; i < dynamicParts.count(); ++i) {
	dynamicParts[i]->draw();
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}
