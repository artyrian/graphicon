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

public slots:
    void setRotation(int &coordRot, int angle);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    MyWatch *watch;

    int xRot;
    int yRot;
    int zRot;
    QPoint lastPos;
    QColor qtRed;
    QColor qtBlue;
};

#endif // GLWIDGET_H
