#ifndef MYWATCH_H
#define MYWATCH_H

#include <QObject>
#include <QColor>

#include "figures.h"

class MyWatch : public QObject {
    GLuint textureID[1];
public:
    MyWatch(QObject *parent);
    ~MyWatch();


    void draw(bool isAnimation);
    void drawGlass();
    void dynamicDraw(bool isAnimation);
    void drawWithTextures();
    void drawWithoutTextures();
private:
    void createTextureArray();
    void setColor(QColor c, QList <Patch *> &parts);

    void TextureInit();
    void buildGeometryWithoutTextures(Geometry *g, QList<Patch *> &parts);
    void buildGeometry(Geometry *g, QList<Patch *> &parts);
    void dynamicGeometry(bool isAnimation);
    void buildGlass(Geometry *g, QList <Patch*> &parts);
    QList<Patch *> partsWithTextures;
    QList<Patch *> dynamicParts;
    QList<Patch *> partsWithoutTextures;
    QList<Patch *> partsGlass;
    Geometry *geometryWithTextures;
    Geometry *dynGeom;
    Geometry *geometryWithoutTextures;
    Geometry *geometryGlass;

};

#endif // MYWATCH_H
