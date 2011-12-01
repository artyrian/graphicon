#include "scene1.h"
#include <iostream>
#include <math.h>
#include <omp.h>

#define EPS_PIXEL 5

/* 'cause QVector3D is very simple in use for such things :)
  R - x coord,
  G - y coord,
  B - z coord,
*/
static void normalizeColorRGB(QVector3D &v)
{
	v.setX((v.x() > 1.0) ? 1.0 : v.x());
	v.setY((v.y() > 1.0) ? 1.0 : v.y());
	v.setZ((v.z() > 1.0) ? 1.0 : v.z());
	v.setX((v.x() < 0.0) ? 0.0 : v.x());
	v.setY((v.y() < 0.0) ? 0.0 : v.y());
	v.setZ((v.z() < 0.0) ? 0.0 : v.z());
}

static QRgb convertFromVec3(const QVector3D &v)
{
	return qRgb(255 * v.x(), 255 * v.y(), 255 * v.z());
}

Scene1::Scene1()
{
	image = new QImage(IMAGE_WIDTH, IMAGE_HEIGHT, QImage::Format_RGB888);

	viewX = viewY = 0;
	rA = rB = rG = 0;
}

void Scene1::initScene1()
{
	Light light;
	light.ambient = QVector3D(0.0, 0.0, 0.0);
	light.diffuse = QVector3D(0.8, 0.8, 0.8);
	light.specular = QVector3D(1.0, 1.0, 1.0);

	Light light1 = light;
	light1.position = QVector3D(500, 500, 500);
	Light light2 = light;
	light2.position = QVector3D(-300, 300, 300);

	vLight.clear();
	vLight.push_back(light1);
	vLight.push_back(light2);

	observer_point = QVector3D(0, 0, 1200);
	updateRotateMatrix();

	Object *sphere1 = new Sphere(RADIUS_STANDART_SPHERE, QVector3D(0.0, RADIUS_STANDART_SPHERE, 0.0));
	Object *sphere2 = new Sphere(RADIUS_STANDART_SPHERE, QVector3D(- 200, -RADIUS_STANDART_SPHERE, 0.0));
	Object *sphere3 = new Sphere(RADIUS_STANDART_SPHERE, QVector3D(+200, 0.0, 0.0));
	Object *box1 = new Box(RADIUS_STANDART_SPHERE, 4 * RADIUS_STANDART_SPHERE,  RADIUS_STANDART_SPHERE, QVector3D(0.0, - 2 * RADIUS_STANDART_SPHERE, 0.0));
	Object *box2 = new Box(RADIUS_STANDART_SPHERE, 2 * RADIUS_STANDART_SPHERE,  3 * RADIUS_STANDART_SPHERE, QVector3D(+200, - 2 * RADIUS_STANDART_SPHERE, 0.0));
	Object *box3 = new Box(2 * RADIUS_STANDART_SPHERE, 2 * RADIUS_STANDART_SPHERE, 2 * RADIUS_STANDART_SPHERE, QVector3D(-200, - 3 * RADIUS_STANDART_SPHERE, 0.0));
	Object *plane1 = new Plane(QVector4D(0, 1, 0, 300));
	Object *cone1 = new Cone(QVector2D(50, 20), QVector3D(500, 0, 200));
	Object *torus1 = new Torus(QVector2D(60, 30), QVector3D(500, -100, 200));
	Object *torusRect1 = new Torus64(QVector2D(270, 30), QVector3D(330, 0, - 400));
	Object *torusRect2 = new Torus64(QVector2D(190, 30), QVector3D(0, 400, - 400));
	Object *torusRect3 = new Torus64(QVector2D(270, 30), QVector3D(- 330, 0, - 400));

	sphere1->setColor(QVector3D(0, 0.5, 0));
	sphere2->setColor(QVector3D(0.5, 0, 0));
	sphere3->setColor(QVector3D(0, 0, 0.5));

	box1->setColor(QVector3D(0.8, 0.5, 0.2));
	box2->setColor(QVector3D(0.8, 0.5, 0.2));
	box3->setColor(QVector3D(0.8, 0.5, 0.2));

	cone1->setColor(QVector3D(1, 1, 0));
	torus1->setColor(QVector3D(0.5, 0, 0.5));

	torusRect1->setColor(QVector3D(0.5, 0.5, 1));
	torusRect2->setColor(QVector3D(0.5, 0.5, 1));
	torusRect3->setColor(QVector3D(0.5, 0.5, 1));


	vObject.clear();
	vObject.push_back(sphere1);
	vObject.push_back(sphere2);
	vObject.push_back(sphere3);
	vObject.push_back(box1);
	vObject.push_back(box2);
	vObject.push_back(box3);
	vObject.push_back(cone1);
	vObject.push_back(torus1);
	vObject.push_back(torusRect1);
	vObject.push_back(torusRect2);
	vObject.push_back(torusRect3);

	vObject.push_back(plane1);
}

void Scene1::initScene2()
{
	image = new QImage(IMAGE_WIDTH, IMAGE_HEIGHT, QImage::Format_RGB888);

	viewX = viewY = 0;
	rA = rB = rG = 0;

	Light light;
	light.ambient = QVector3D(0.0, 0.0, 0.0);
	light.diffuse = QVector3D(1.0, 1.0, 1.0);
	light.specular = QVector3D(1.0, 1.0, 1.0);

	Light light1 = light;
	light1.position = QVector3D(500, 500, 0);
	Light light2 = light;
	light2.position = QVector3D(0, 500, 500);

	vLight.clear();
	vLight.push_back(light1);
	vLight.push_back(light2);

	observer_point = QVector3D(0, 0, 800);
	updateRotateMatrix();

	Object *plane1 = new Plane(QVector4D(0.0, 1.0, 0.0, 300));

	Object *sphere1 = new Sphere(0.75 * RADIUS_STANDART_SPHERE, QVector3D(0.0, 0.75 * 2 *RADIUS_STANDART_SPHERE, 0.0));
	Object *sphere2 = new Sphere(RADIUS_STANDART_SPHERE, QVector3D(0, 0, 0.0));
	Object *sphere3 = new Sphere(1.5 *RADIUS_STANDART_SPHERE, QVector3D(0, -2 *RADIUS_STANDART_SPHERE, 0.0));

	Object *sphere4 = new Sphere(0.75 * RADIUS_STANDART_SPHERE, QVector3D(-400.0, 0.75 * 2 *RADIUS_STANDART_SPHERE, 100.0));
	Object *sphere5 = new Sphere(RADIUS_STANDART_SPHERE, QVector3D(-400, 0, 100.0));
	Object *sphere6 = new Sphere(1.5 *RADIUS_STANDART_SPHERE, QVector3D(-400, -2 *RADIUS_STANDART_SPHERE, 100.0));

	Object *sphere7 = new Sphere(0.75 * RADIUS_STANDART_SPHERE, QVector3D(300.0, 0.75 * 2 *RADIUS_STANDART_SPHERE, 150.0));
	Object *sphere8 = new Sphere(RADIUS_STANDART_SPHERE, QVector3D(300, 0, 150.0));
	Object *sphere9 = new Sphere(1.5 *RADIUS_STANDART_SPHERE, QVector3D(300, -2 *RADIUS_STANDART_SPHERE, 150.0));

	sphere1->setColor(QVector3D(0, 0, 0.1));
	sphere2->setColor(QVector3D(0, 0, 0.1));
	sphere3->setColor(QVector3D(0, 0, 0.1));
	sphere4->setColor(QVector3D(0, 0, 0.1));
	sphere5->setColor(QVector3D(0, 0, 0.1));
	sphere6->setColor(QVector3D(0, 0, 0.1));
	sphere7->setColor(QVector3D(0, 0, 0.1));
	sphere8->setColor(QVector3D(0, 0, 0.1));
	sphere9->setColor(QVector3D(0, 0, 0.1));

	vObject.clear();
	vObject.push_back(plane1);

	vObject.push_back(sphere1);
	vObject.push_back(sphere2);
	vObject.push_back(sphere3);

	vObject.push_back(sphere4);
	vObject.push_back(sphere5);
	vObject.push_back(sphere6);

	vObject.push_back(sphere7);
	vObject.push_back(sphere8);
	vObject.push_back(sphere9);


}

/*
Forgot linear algebra :( So, I watch this matrix from wiki.
http://en.wikipedia.org/wiki/Rotation_matrix
*/
void Scene1::updateRotateMatrix()
{
	rotateMatrix[0][0] = cos(rA) * cos(rB);
	rotateMatrix[0][1] = cos(rA) * sin(rB) * sin(rG) - sin(rA) * cos(rG);
	rotateMatrix[0][2] = cos(rA) * sin(rB) * cos(rG) + sin(rA) * sin(rG);

	rotateMatrix[1][0] = sin(rA) * cos(rB);
	rotateMatrix[1][1] = sin(rA) * sin(rB) * sin(rG) + cos(rA) * cos(rG);
	rotateMatrix[1][2] = sin(rA) * sin(rB) * cos(rG) - cos(rA) * sin(rG);

	rotateMatrix[2][0] = -sin(rB);
	rotateMatrix[2][1] = cos(rB) * sin(rG);
	rotateMatrix[2][2] = cos(rB) * cos(rG);
}


/* Main loop for build scene
*/
QImage *Scene1::buildImage(int power)
{
	QVector3D current_observer_point = applyMatrixRotate(observer_point);

	for (int y = 0; y < image->height(); y++) {
#pragma omp parallel for
	     for (int x = 0; x < image->width(); x++) {
		if (y % power == 0) {
		    QVector3D positionImage(IMAGE_WIDTH / 2 - x, IMAGE_HEIGHT / 2 - y, DEPTH);
		    QVector3D position = applyMatrixRotate(positionImage);
		    Ray ray(position, position - current_observer_point);
		    QRgb pix = rayTrace(ray);
		    image->setPixel(x, y, pix);
		} else {
			image->setPixel(x, y, image->pixel(x, y - (y % power)));
		}
	    }
	}

	return image;
}


QImage *Scene1::buildImage2(int power)
{
	QVector3D current_observer_point = applyMatrixRotate(observer_point);

	for (int y = 0; y < image->height(); y++) {
#pragma omp parallel for
		for (int x = 0; x < image->width(); x++) {
		    if (y % power != 0) {
			    QVector3D positionImage(IMAGE_WIDTH / 2 - x, IMAGE_HEIGHT / 2 - y, DEPTH);
			    QVector3D position = applyMatrixRotate(positionImage);
			    Ray ray(position, position - current_observer_point);
			    QRgb pix = rayTrace(ray);
			    image->setPixel(x, y, pix);
		    }
	    }
	    //
	}

	return image;
}


QRgb Scene1::rayTrace(const Ray &ray)
{
	QVector3D color(0.0, 0.0, 0.0);

	Hit hit = raySceneIntersection(ray);

	if (hit.distance < 0) {
	    return COLOR_BACKGROUND;
	}

	QVector3D hit_point = ray.start + ray.direction * hit.distance;
	color = QVector3D(0.01, 0.01, 0.01);

	for (int i = 0; i < vLight.size(); i++) {
	    if (isVisible(hit_point, vLight[i].position)) {
		color += Shade(hit_point, hit, vLight[i]);
	    }
	}

	normalizeColorRGB(color);
	return convertFromVec3(color);
}

bool Scene1::isVisible(const QVector3D &point, const QVector3D &light)
{
	Ray ray(light, point - light);
	Hit hit = raySceneIntersection(ray);

	if (hit.distance < 0) {
		return false;
	}

	QVector3D distance = ray.start + hit.distance * ray.direction;
	return (distance - point).length() < EPS_PIXEL;
}

QVector3D Scene1::Shade(const QVector3D &point, const Hit &hit, const Light &light)
{
	QVector3D colorF = hit.mat.color;

	colorF += hit.mat.ambient * light.ambient;


	QVector3D tmp1 = hit.mat.diffuse * light.diffuse;
	tmp1 *= Vec::multCoords(hit.normal, light.position - point);
	colorF += tmp1;

	double a = 2 * Vec::multCoords(hit.normal, light.position - point) *
		Vec::multCoords(hit.normal, observer_point - point) -
		Vec::multCoords(light.position - point, observer_point - point);


	QVector3D tmp2 = hit.mat.specular * light.specular * pow(a, hit.mat.shininess);
	colorF += tmp2;

	return colorF;
}

double Scene1::processDistance(const QVector3D &position, uint &indexObject)
{
	indexObject = 0;
	double dist = vObject[indexObject]->F(position);

	for (int i = 0; i < vObject.size(); ++i) {
		double cur_dist = vObject[i]->F(position);
		if (cur_dist < dist) {
			dist = cur_dist;
			indexObject = i;
		}
	}

	return dist;
}


bool inScene(const QVector3D &position)
{
	if ((abs(position.x()) < SCENE_BORDER) && (abs(position.y()) < SCENE_BORDER) && (abs(position.z()) < SCENE_BORDER)) {
		return true;
	} else {
		return false;
	}
}

bool inLocal(double distance)
{
	if ((-EPS_HIT < distance) && (distance < EPS_HIT)) {
		return true;
	} else {
		return false;
	}
}


Hit Scene1::raySceneIntersection(const Ray &ray)
{
	Hit rHit;
	QVector3D position = ray.start;
	uint indexObject = 0;
	double distance = processDistance(position, indexObject);

	while (inScene(position)) {
		position += ray.direction * distance;
		distance = processDistance(position, indexObject);
		if (inLocal(distance) == true) {
			double dst = (position - ray.start).length();
			Material mat = vObject[indexObject]->getMaterial();
			QVector3D normal = vObject[indexObject]->getNormal(position);
			rHit = Hit(dst, mat, normal);
			break;
		}
	}

	return rHit;
}

QVector3D Scene1::applyMatrixRotate(const QVector3D &a)
{
    return QVector3D (
			    a.x() * rotateMatrix[0][0] + a.y() * rotateMatrix[0][1] + a.z() * rotateMatrix[0][2],
			    a.x() * rotateMatrix[1][0] + a.y() * rotateMatrix[1][1] + a.z() * rotateMatrix[1][2],
			    a.x() * rotateMatrix[2][0] + a.y() * rotateMatrix[2][1] + a.z() * rotateMatrix[2][2]
    );
}

void Scene1::setRotation(int dx, int dy)
{
	viewX += dx;
	viewY += dy;
	rA = 0;
	rB = M_PI * viewX / 180;
	rG = - M_PI * viewY / 180;

	updateRotateMatrix();
}
