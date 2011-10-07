#ifndef TRAINREADDATA_H
#define TRAINREADDATA_H

#include "../src-core/Instances.hpp"

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
