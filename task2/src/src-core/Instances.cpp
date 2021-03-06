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

size_t InstancesData::getInstancesNumber()
{
	return numberInstances;
}

void InstancesData::incInstancesNumber()
{
	numberInstances++;
}

void InstancesData::setInstancesNumber(int i)
{
	numberInstances = i;
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
			gradientX[x][y] = (-1)	* intensity[realX(x-1)][realY(y-1)] +
					(-2)	* intensity[realX(x-1)][realY(y)] +
					(-1)	* intensity[realX(x-1)][realY(y+1)] +
					(1)	* intensity[realX(x+1)][realY(y-1)] +
					(2)	* intensity[realX(x+1)][realY(y)] +
					(1)	* intensity[realX(x+1)][realY(y+1)];
		}
	}
}

void InstancesData::processGradientY()
{
	for(int y = 0; y < Y_PIXEL; y++) {
		for(int x = 0; x < X_PIXEL; x++) {
			gradientY[x][y] = (-1)	* intensity[realX(x-1)][realY(y-1)] +
					(-2)	* intensity[realX(x)][realY(y-1)] +
					(-1)	* intensity[realX(x+1)][realY(y-1)] +
					(1)	* intensity[realX(x-1)][realY(y+1)] +
					(2)	* intensity[realX(x)][realY(y+1)] +
					(1)	* intensity[realX(x)][realY(y+1)];
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

void InstancesData::processSign()
{
	std::vector<double> features(NUM_FEATURES);

	for (int y = 0; y < Y_BLOCK; y++) {
		for (int x = 0; x < X_BLOCK; x++) {
			int nlc[8];
			for (int i = 0; i < 8; i++) {
				nlc[i] = 0;
			}

			int offset = NUM_SIGN * (y * X_BLOCK + x);
			for (int i = 0; i < SIZE_BLOCK; i++) {
				for (int j = 0; j < SIZE_BLOCK; j++) {
					int sign = getDirection(atan[x * SIZE_BLOCK + j][y * SIZE_BLOCK + i]);
					nlc[sign - 1]++;
					//features[offset + sign - 1] ++;
				}
			}

			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 2 * ORDER_APPROXIMATION + 1; j++) {
					double lambda = (j - ORDER_APPROXIMATION) * STEP_APPROXIMATION;
					double re = sqrt(nlc[i]/ cosh(M_PI * lambda)) * cos (lambda * log(nlc[i]));
					double im = - sqrt(nlc[i]/ cosh(M_PI * lambda)) * sin (lambda * log(nlc[i]));
					if (nlc[i] == 0) {
						re = 0;
						im = 0;
					}
					features [offset + i * 2 * (2 * ORDER_APPROXIMATION + 1) + 2 * j] = re;
					features [offset + i * 2 * (2 * ORDER_APPROXIMATION + 1) + 2 * j + 1] = im;
				}

			}

		}
	}

	instancesFeatures.push_back(features);
}

int InstancesData::getDirection(double d)
{
	if (d >= 0 && d < M_PI/4)
		return 1;
	else if (d >= M_PI/4 && d < M_PI/2)
		return 2;
	else if (d >= M_PI/2 && d < double(3 * M_PI)/4)
		return 3;
	else if (d >= double(3*M_PI)/4 && d <= double(M_PI))
		return 4;
	else if (d >= double(-M_PI) && d < double(-3 * M_PI)/4)
		return 5;
	else if (d >= double(-3*M_PI)/4 && d < double(-M_PI)/2)
		return 6;
	else if (d >= double(-M_PI)/2 && d < double(-M_PI)/4)
		return 7;
	else if (d >= double(-M_PI)/4 && d < 0)
		return 8;
	else {
		std::cout<< "double d:" << d << std::endl;
		std::cerr << "Not allowed value of atan." << std::endl;
		exit(1);
	}
}

void InstancesData::processInstance(int x0, int y0, int x1, int y1, QImage &img)
{
	processIntensity(x0, y0, x1, y1, img);
	processGradientX();
	processGradientY();
	processAtan();
	processSign();
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
