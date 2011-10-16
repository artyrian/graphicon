#ifndef TRAINREADDATA_H
#define TRAINREADDATA_H

#include "../src-core/Instances.hpp"
#include "string"

class Train {
	double valueC;

	InstancesData reader;
	char *pathDir;
	char *pathFileLocations;
	struct model *modelPedestrian;
	struct problem prob;
	struct parameter param;
	char *pathFileModel;
public:
	Train(char *, char *);
	char *qualifierTraining();
	char *bootstrapping(char *, char *);
	~Train();
	void setC(double param);

private:
	void trainPredictData(std::vector<ItemPng> &vectPos);
	void createPositiveVector(std::vector<ItemPng> &vectPos);
	void createNegativeVector(std::vector<ItemPng> &vectN, std::vector<ItemPng> &vectPos, std::vector<ItemPng> &vectNP);
	void trainPredictData2(std::vector<ItemPng> &vectN);
	void fillModel();
	void callTrain();
	char *saveModelToFile();
	void freeModel();
};

#endif // TRAINREADDATA_H
