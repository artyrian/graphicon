#include <QGLWidget>
#include <QMatrix4x4>


#include <GL/freeglut.h>

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

void MyWatch::buildGeometry()
{
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
