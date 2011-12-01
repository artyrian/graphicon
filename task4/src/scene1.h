#ifndef SCENE1_H
#define SCENE1_H

#include <QVector>
#include <QImage>
#include <QRgb>
//#include "Geometry.h"
#include "Object.h"

#define IMAGE_WIDTH 800
#define IMAGE_HEIGHT 600
#define DEPTH 5 * RADIUS_STANDART_SPHERE
const QRgb COLOR_BACKGROUND = qRgb(0, 50, 50);


#define SCENE_BORDER 800

#define EPS_HIT 0.01

class Scene1
{
	QImage *image;
	QVector<Light> vLight;
	QVector<Object *> vObject;
	QVector3D observer_point;

	double rotateMatrix[3][3];
	int viewX, viewY;
	double rA, rB, rG;

	Hit raySceneIntersection(const Ray &ray);
	QRgb rayTrace(const Ray &ray);
	bool isVisible(const QVector3D &point, const QVector3D &light);
	QVector3D Shade(const QVector3D &point, const Hit &hit, const Light &light);
	double processDistance(const QVector3D &position, uint &indexObject);
	void updateRotateMatrix();
	QVector3D applyMatrixRotate(const QVector3D &a);
public:
	void initScene1();
	void initScene2();
	void set_observer_point(const QVector3D &point);
	QImage *buildImage(int power);
	QImage *buildImage2(int power);
	void setRotation(int x, int y);
	Scene1();
};


#endif // SCENE1_H
