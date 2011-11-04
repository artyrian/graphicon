#include <QtGui>
#include <QtOpenGL>

#include "GlWidget.h"

#define COUNT_SHIFT 5

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    xRot = 0;
    yRot = 0;
    zRot = 0;

    zoom = 2.0;

    backgroundColor = QColor(84, 40, 84);

    isAnimation = false;
    showMirror = true;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);

    timerRotate = new QTimer(this);
    connect(timerRotate, SIGNAL(timeout()), this, SLOT(presentation()));
 //   timerRotate->start(100);
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
    qglClearColor(backgroundColor);

    watch = new MyWatch(this);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_MULTISAMPLE);

    static GLfloat lightPosition[4] = { 1.0, 1.0, 1.0, 1.0};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

void reflectionmatrix(GLfloat reflection_matrix[4][4],
                 GLfloat plane_point[3],
                 GLfloat plane_normal[3])
{
  GLfloat* p = (GLfloat*)plane_point;
  GLfloat* v = (GLfloat*)plane_normal;
  float pv = p[0]*v[0]+p[1]*v[1]+p[2]*v[2];

  reflection_matrix[0][0] = 1 - 2 * v[0] * v[0];
  reflection_matrix[1][0] = - 2 * v[0] * v[1];
  reflection_matrix[2][0] = - 2 * v[0] * v[2];
  reflection_matrix[3][0] = 2 * pv * v[0];

  reflection_matrix[0][1] = - 2 * v[0] * v[1];
  reflection_matrix[1][1] = 1- 2 * v[1] * v[1];
  reflection_matrix[2][1] = - 2 * v[1] * v[2];
  reflection_matrix[3][1] = 2 * pv * v[1];

  reflection_matrix[0][2] = - 2 * v[0] * v[2];
  reflection_matrix[1][2] = - 2 * v[1] * v[2];
  reflection_matrix[2][2] = 1 - 2 * v[2] * v[2];
  reflection_matrix[3][2] = 2 * pv * v[2];

  reflection_matrix[0][3] = 0;
  reflection_matrix[1][3] = 0;
  reflection_matrix[2][3] = 0;
  reflection_matrix[3][3] = 1;
}



void ShiftCamera (int i)
{
    gluLookAt(2.0, 0, 0, 0, 0, 0, 0, 1, 0);
}


void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glLoadIdentity();
    glTranslatef(0.0, 0.0, -10.0);
    glRotatef(xRot, 1.0, 0.0, 0.0);
    glRotatef(yRot, 0.0, 1.0, 0.0);
    glRotatef(zRot, 0.0, 0.0, 1.0);

    watch->draw(isAnimation);

    if (showMirror) {
        Mirroring();
    }


}

void draw_mirror()
{
        glEnable(GL_STENCIL);
      //  glEnable(GL_BLEND);

        glBegin(GL_QUADS);

        glColor4d(0.1, 0.2, 0.3, 0.2);

        glBegin(GL_QUADS);
                  glVertex3f( 1.5f, 1.5f, -2.0f);  // Вверх справа
                  glVertex3f(-1.5f, 1.5f, -2.0f);  // Вверх слева
                  glVertex3f(-1.5f,-1.5f, -2.0f);  // Слева снизу
                  glVertex3f( 1.5f,-1.5f, -2.0f);  // Справа снизу
        glEnd();

        glDisable(GL_BLEND);
        glDisable(GL_STENCIL);
}


void GLWidget::Mirroring()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS,1,1);
    glDepthMask(GL_FALSE);

    draw_mirror();

    glDepthMask(GL_TRUE);
    glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
    glStencilFunc(GL_EQUAL,1,1);

    GLfloat reflection_matrix[4][4];
    GLfloat plane_point[3];
    GLfloat plane_normal[3];

    plane_point[0]=0.0f;
    plane_point[1]=0.0f;
    plane_point[2]=-2.0f;

    plane_normal[0]=0.0f;
    plane_normal[1]=0.0f;
    plane_normal[2]=1.0f;

    reflectionmatrix(reflection_matrix, plane_point, plane_normal);

    glPushMatrix();
    glMultMatrixf((float *)reflection_matrix);
    watch->draw(isAnimation);
    glPopMatrix();
    glDisable(GL_STENCIL_TEST);
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
    glOrtho(-zoom, +zoom, -zoom, +zoom, 5.0, 15.0);	    // mb gluPerspective() ???
    glMatrixMode(GL_MODELVIEW);
}


void GLWidget::animation()
{
    isAnimation = !isAnimation;

    timer->stop();
    if (isAnimation) {
        timer->setInterval(75);
    } else {
        timer->setInterval(1000);

    }
    timer->start();

    updateGL();
}

void GLWidget::presentation()
{
    xRot += 1;
    yRot += 2;
    zRot += 3;
    updateGL();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
    timerRotate->stop();
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

    if (event->buttons() & Qt::LeftButton && event->buttons() & Qt::RightButton && isAnimation) {
        watch->reggi_time();
    }

    lastPos = event->pos();
}

void GLWidget::rotation()
{
    if (timerRotate->isActive()) {
        timerRotate->stop();
    } else {
        timerRotate->start(100);
    }
}

void GLWidget::wheelEvent(QWheelEvent * event)
{

    double delta = event->delta() / 1200.0;

    if ( delta + zoom < 3.0 && zoom + delta > 0.5 )
    {
        zoom += delta;
    }

    resizeGL(this->width(), this->height());
    updateGL();
}

void GLWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    animation();
}

