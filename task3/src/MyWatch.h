#ifndef MYWATCH_H
#define MYWATCH_H

#include <QObject>
#include <QColor>

#include "figures.h"
#include "resources.h"

class MyWatch : public QObject {
    GLuint textureID[count_textures];
    //enum details {face, back, belt};
public:
    MyWatch(QObject *parent);
    ~MyWatch();
    void reggi_time();

    void draw(bool isAnimation);
    void drawGlass();
    void dynamicDraw(bool isAnimation);
    void drawWithTexturesi(Geometry *g, const QList<Patch *> &parts, int k);
    void drawWithoutTextures();
private:
    void setColor(QColor c, QList <Patch *> &parts);

    void TextureInit(const char *path, int i);
    void buildGeometryWithoutTextures(Geometry *g, QList<Patch *> &parts);
    void buildGeometry();
    void dynamicGeometry(bool isAnimation);
    void buildGlass(Geometry *g, QList <Patch*> &parts);
    QList<Patch *> partsWithTextures1;
    QList<Patch *> partsWithTextures2;
    QList<Patch *> partsWithTextures3;
    QList<Patch *> partsWithTextures4;
    QList<Patch *> dynamicParts;
    QList<Patch *> partsWithoutTextures;
    QList<Patch *> partsGlass;
    Geometry *geometryWithTextures1;
    Geometry *geometryWithTextures2;
    Geometry *geometryWithTextures3;
    Geometry *geometryWithTextures4;
    Geometry *dynGeom;
    Geometry *geometryWithoutTextures;
    Geometry *geometryGlass;

};

#endif // MYWATCH_H
