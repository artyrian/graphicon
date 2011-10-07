#ifndef TRAINREADDATA_H
#define TRAINREADDATA_H
#include <fstream>
#include <QtGui/QImage>
#include <QDir>
#include <vector>

#include "liblinear-1.8/linear.h"

#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

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

	InstancesData(char *);
	int numberInstances;
	size_t getInstancesNumber();
	void setPathFileLocations(char *);
	void setPathFileModel(char *);
	void trainData();
	void predictData();

private:
	/*
	void processIntensity(int, int, int, int, QImage &);
	void processGradientX();
	void processGradientY();
	void processAtan();
	*/
	void processSign(std::vector<double> &);
	void processInstance(int, int, int, int, QImage &);
	void positive(int, int, int, int, QImage &);
	void negativeX(int &, int &, QImage &);
	void negative(int, int, int, int, QImage &);
};

class Train {
	InstancesData reader;

	struct model* modelPedestrian;
	struct problem prob;
	struct parameter param;
	char *pathFileModel;
public:
	Train(char *, char *);
	char *qualifierTraining();
	~Train();

private:
	void fillModel();
	void callTrain();
	char *saveModelToFile();
	void freeModel();
};

#endif // TRAINREADDATA_H
