#ifndef INSTANCES_H
#define INSTANCES_H
#include <fstream>
#include <QtGui/QImage>
#include <QDir>
#include <vector>

#include "liblinear-1.8/linear.h"

#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

#define STEP_X 5
#define STEP_X_DETECTING 5
#define STEP_TRAIN_X 30
#define X_PIXEL 80
#define Y_PIXEL 200
#define X_BLOCK 10
#define Y_BLOCK 25
#define SIZE_BLOCK 8
#define SIZE_STRING_NAME 24
#define NUM_FEATURES 2000
#define NUM_PARAMETERS 5
#define NUM_SIGN 8

const char NameFileModel[]  = "model.txt";

struct ItemPng {
	std::string name;
	int x;
};

class InstancesData {
	char *pathFileLocations;
	char *pathFileModel;
	char *pathDir;
	QFileInfoList list;

	int intensity[X_PIXEL][Y_PIXEL];
	int gradientX[X_PIXEL][Y_PIXEL];
	int gradientY[X_PIXEL][Y_PIXEL];
	double atan[X_PIXEL][Y_PIXEL];

	int numberInstances;
public:
	std::vector< std::vector<double> > instancesFeatures;
	std::vector<int> instancesLabels;

	InstancesData();
	size_t getInstancesNumber();
	void incInstancesNumber();
	void setInstancesNumber(int);
	void trainPredictData();

	void processInstance(int, int, int, int, QImage &);
	void positive(int, int, int, int, QImage &);
	void negativeX(int &, int &, QImage &);
	void negative(int, int, int, int, QImage &);
private:

	void processIntensity(int, int, int, int, QImage &);
	void processGradientX();
	void processGradientY();
	void processAtan();

	void processSign(std::vector<double> &);
	int getDirection(double);
};

#endif // INSTANCES_HPP
