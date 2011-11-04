#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include "MyWatch.h"

class GLWidget : public QGLWidget {
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

    bool showMirror;
    bool lolMode;

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    void rotation();

public slots:
    void setRotation(int &coordRot, int angle);
    void animation();
    void presentation();

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
    bool isAnimation;
    int xRot;
    int yRot;
    int zRot;
    QPoint lastPos;
    QColor backgroundColor;
    QTimer *timer;
    QTimer *timerRotate;
    double zoom;


    void Mirroring();
};

#endif // GLWIDGET_H
