#include "Instances.hpp"
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <math.h>

const double coeffRed = 0.2125;
const double coeffGreen = 0.7154;
const double coeffBlue = 0.0721;

int realX(int);
int realY(int);

InstancesData::InstancesData()
{
	numberInstances = 0;
}

void InstancesData::setPathDir(char *arg)
{
	pathDir = arg;
}

void InstancesData::setPathFileLocations(char *arg)
{
	pathFileLocations = arg;
}

void InstancesData::setPathFileModel(char *arg)
{
	pathFileModel = arg;
}

size_t InstancesData::getInstancesNumber()
{
	return numberInstances;
}

void InstancesData::incInstancesNumber()
{
	numberInstances++;
}

void InstancesData::trainPredictData()
{
	int rStatus;
	char *pngName = new char [SIZE_STRING_NAME];
	char *fileName = new char [strlen(pathDir) + SIZE_STRING_NAME + strlen(".png")];
	int x0, y0, x1, y1;

	FILE *fileLocations = fopen(pathFileLocations, "r");
	while ((rStatus = fscanf(fileLocations, "%s%d%d%d%d", pngName, &y0, &x0, &y1, &x1)) != EOF)
	{
		if (rStatus != NUM_PARAMETERS) {
			fprintf(stderr, "Can't read all option for current png.\n");
			exit(1);
		}
		strcpy(fileName, pathDir);
		strcat(fileName, pngName);
		strcat(fileName, ".png");

		QImage img(fileName);

		positive(x0, y0, x1, y1, img);

		negativeX(x0, x1, img);
		negative(x0, y0, x1, y1, img);
	}

	fclose(fileLocations);
	delete []pngName;
	delete []fileName;
}

void InstancesData::testPredictData()
{
	QDir dir;
	dir.setFilter(QDir::Files);
	dir.setSorting(QDir::Name);
	list = dir.entryInfoList(QStringList() << "*.png");

/*
	for (int i = 0; i < list.size(); ++i) {
		QFileInfo fileInfo = list.at(i);
		QImage img(fileInfo.fileName());
		std::cout << qPrintable(QString("%1").arg(fileInfo.fileName())) << std::endl;
	}
*/
}

void InstancesData::predictData()
{
	QDir dir;
	dir.setFilter(QDir::Files);
	dir.setSorting(QDir::Name);
	list = dir.entryInfoList(QStringList() << "*.png");

/*
	for (int i = 0; i < list.size(); ++i) {
		QFileInfo fileInfo = list.at(i);
		QImage img(fileInfo.fileName());
		std::cout << qPrintable(QString("%1").arg(fileInfo.fileName())) << std::endl;
	}
*/
}


/* */
void InstancesData::processIntensity(int x0, int y0, int x1, int y1, QImage &img)
{
	for(int y = y0; y <y1; y++) {
		for(int x = x0; x < x1; x++) {
			intensity[x-x0][y-y0] = coeffRed * qRed(img.pixel(x,y)) +
					coeffGreen * qGreen(img.pixel(x,y)) +
					coeffBlue * qBlue(img.pixel(x,y));
		}
	}
}

void InstancesData::processGradientX()
{
	for(int y = 0; y < Y_PIXEL; y++) {
		for(int x = 0; x < X_PIXEL; x++) {
			gradientX[x][y] = (-1) * intensity[realX(x-1)][realY(y-1)] +
					2 * intensity[realX(x)][realY(y-1)] +
					(-1) * intensity[realX(x+1)][realY(y-1)] +
					(-1) * intensity[realX(x-1)][realY(y+1)] +
					2 * intensity[realX(x)][realY(y+1)] +
					(-1) * intensity[realX(x+1)][realY(y+1)];
		}
	}
}

void InstancesData::processGradientY()
{
	for(int y = 0; y < Y_PIXEL; y++) {
		for(int x = 0; x < X_PIXEL; x++) {
			gradientY[x][y] = (-1) * intensity[realX(x-1)][realY(y-1)] +
					2 * intensity[realX(x-1)][realY(y)] +
					(-1) * intensity[realX(x-11)][realY(y+1)] +
					(-1) * intensity[realX(x+1)][realY(y-1)] +
					2 * intensity[realX(x+1)][realY(y)] +
					(-1) * intensity[realX(x+1)][realY(y+1)];
		}
	}
}

void InstancesData::processAtan()
{
	for(int y = 0; y < Y_PIXEL; y++) {
		for(int x = 0; x < X_PIXEL; x++) {
			atan[x][y] = atan2(gradientY[x][y], gradientX[x][y]);
		}
	}
}

void InstancesData::processSign(std::vector<double> &features)
{
	for (int y = 0; y < Y_BLOCK; y++) {
		for (int x = 0; x < X_BLOCK; x++) {
			int offset = NUM_SIGN * (y * X_BLOCK + x);
			for (int i = 0; i < SIZE_BLOCK; i++) {
				for (int j = 0; j < SIZE_BLOCK; j++) {
					int sign = 5 ;//atan[x * SIZE_BLOCK + j][y * SIZE_BLOCK + i];
					features[offset + sign] ++;
				}
			}
		}
	}
}

void InstancesData::processInstance(int x0, int y0, int x1, int y1, QImage &img)
{
	processIntensity(x0, y0, x1, y1, img);
	processGradientX();
	processGradientY();
	processAtan();
	std::vector<double> features(NUM_FEATURES);
	processSign(features);

	instancesFeatures.push_back(features);
}

void InstancesData::positive(int x0, int y0, int x1, int y1, QImage &img)
{
	processInstance(x0, y0, x1, y1, img);
	instancesLabels.push_back(1);
	numberInstances++;
}

void InstancesData::negativeX(int &x0, int &x1, QImage &img)
{
	if ((x0 + X_PIXEL / 2 ) > (img.width()/2)) {
		x0 -= X_PIXEL;
		x1 -= X_PIXEL;
	} else {
		x0 += X_PIXEL;
		x1 += X_PIXEL;
	}
}

void InstancesData::negative(int x0, int y0, int x1, int y1, QImage &img)
{
	processInstance(x0, y0, x1, y1, img);
	instancesLabels.push_back(-1);
	numberInstances++;
}

int realX(int x)
{
	if (x < 0) {
		return realX(-x - 1);
	} else if (x >= X_PIXEL){
		return realX(2 * X_PIXEL - 1 - x);
	} else {
		return x;
	}
}

int realY(int y)
{
	if  (y < 0) {
		return realY(-y - 1);
	} else if (y >= Y_PIXEL) {
		return realY(2 * Y_PIXEL - 1 - y);
	} else {
		return y;
	}
}
