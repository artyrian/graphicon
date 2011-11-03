#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include "MyWatch.h"

class GLWidget : public QGLWidget {
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();


    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    bool isAnimation;
    void presentation();

public slots:
    void setRotation(int &coordRot, int angle);
    void animation();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *q);
private:
    MyWatch *watch;

    int xRot;
    int yRot;
    int zRot;
    QPoint lastPos;
    QColor qtRed;
    QColor qtBlue;

    double zoom;
};

#endif // GLWIDGET_H
