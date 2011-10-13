#ifndef TRAINREADDATA_H
#define TRAINREADDATA_H

#include "../src-core/Instances.hpp"
#include "string"



class Train {
	InstancesData reader;
	char *pathDir;
	char *pathFileLocations;
	struct model *modelPedestrian;
	struct problem prob;
	struct parameter param;
	char *pathFileModel;
	std::vector<ItemPng> vectP;
public:
	Train(char *, char *);
	char *qualifierTraining();
	char *bootstrapping();
	~Train();

private:
	void trainPredictData();
	void createNegativeVector(std::vector<ItemPng> &vectN, std::vector<ItemPng> &vectNP);
	void trainPredictData2(std::vector<ItemPng> &vectN);
	void fillModel();
	void callTrain();
	char *saveModelToFile();
	void freeModel();
};

#endif // TRAINREADDATA_H
