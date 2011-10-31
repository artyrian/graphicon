#ifndef MYWATCH_H
#define MYWATCH_H

#include <QObject>
#include <QColor>

#include "figures.h"

class MyWatch : public QObject {
    GLuint textureID[1];
public:
    MyWatch(QObject *parent, float scale = 1.0);
    ~MyWatch();

    void setColor(QColor c);

    void draw();
    void dynamicDraw();
private:
    void TextureInit();
    void buildGeometry();
    void dynamicGeometry();
    QList<Patch *> parts;
    QList<Patch *> dynamicParts;
    Geometry *geom;
    Geometry *dynGeom;
};

#endif // MYWATCH_H
