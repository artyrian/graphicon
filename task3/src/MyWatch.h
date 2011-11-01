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


    void draw();
    void dynamicDraw();
    void drawWithTextures();
    void drawWithoutTextures();
private:
    void createTextureArray();
    void setColor(QColor c, QList <Patch *> &parts);

    void TextureInit();
    void buildGeometryWithoutTextures(Geometry *g, QList<Patch *> &parts);
    void buildGeometry(Geometry *g, QList<Patch *> &parts);
    void dynamicGeometry();
    QList<Patch *> partsWithTextures;
    QList<Patch *> dynamicParts;
    QList<Patch *> partsWithoutTextures;
    Geometry *geometryWithTextures;
    Geometry *dynGeom;
    Geometry *geometryWithoutTextures;
};

#endif // MYWATCH_H
