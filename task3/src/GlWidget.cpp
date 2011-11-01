#include <QtGui>
#include <QtOpenGL>

#include "GlWidget.h"


#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    xRot = 0;
    yRot = 0;
    zRot = 0;

    qtRed = QColor::fromRgb(255, 0, 0);
    qtBlue = QColor::fromRgb(0, 0, 255);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);

}

GLWidget::~GLWidget()
{
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0) {
        angle += 360;
    }

    while (angle > 360) {
        angle -= 360;
    }
}

void GLWidget::setRotation(int &coord, int angle)
{
    qNormalizeAngle(angle);

    if (angle != coord) {
	coord = angle;
	updateGL();
    }
}


void GLWidget::initializeGL()
{
    qglClearColor(qtBlue.dark());

    watch = new MyWatch(this);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_MULTISAMPLE);

    static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}


void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -10.0);
    glRotatef(xRot, 1.0, 0.0, 0.0);
    glRotatef(yRot, 0.0, 1.0, 0.0);
    glRotatef(zRot, 0.0, 0.0, 1.0);

    watch->draw();
}

/* Need to solve what is more useful:
   glOrtho or gluPerspective()
*/
void GLWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);	    // mb gluPerspective() ???
    glMatrixMode(GL_MODELVIEW);
}


void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setRotation(xRot, xRot + dy);
        setRotation(yRot, yRot + dx);
    } else if (event->buttons() & Qt::RightButton) {
        setRotation(xRot, xRot + dy);
        setRotation(zRot, zRot + dx);
    }

    lastPos = event->pos();
}
