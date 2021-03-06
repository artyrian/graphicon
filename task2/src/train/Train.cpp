#include "Train.hpp"
#include <iostream>
#include <string>
#include <map>

#include "../test/Test.hpp"

Train::Train(char *arg1, char *arg2)
	: reader ()
{
	valueC = 1;

	pathDir = arg1;
	pathFileLocations = arg2;
}

void Train::setC(double p)
{
	std::cout << "Set valueC: " << p << std::endl;
	valueC = p;
}

Train::~Train()
{

}

/* FILL TRAIN STRUCTURE */
void Train::fillModel()
{
	prob.l = int(reader.getInstancesNumber()); // number of instances
	prob.bias = -1; // bias feature
	prob.n = NUM_FEATURES;

	prob.y = Malloc(int, prob.l); // allocate space for labels
	prob.x = Malloc(struct feature_node *, prob.l); // allocate space for features

	for (size_t i = 0; i < reader.getInstancesNumber(); i++)
	{
	  prob.x[i] = Malloc(struct feature_node, NUM_FEATURES+1);
	  prob.x[i][NUM_FEATURES].index = -1;  // -1 marks the end of list
	  for (int j = 0; j < NUM_FEATURES; j++)
	  {
	    prob.x[i][j].index = 1+j; // 1-based feature number
	    prob.x[i][j].value = reader.instancesFeatures[i][j];
	  }
	  prob.y[i] = reader.instancesLabels[i];
	}

	// default values of params. don't change them if not sure (unless param.C)
	param.solver_type = L2R_L2LOSS_SVC_DUAL;
	printf ("ValueC: %f\n", valueC);
	param.C = valueC;
	param.eps = 1e-4;
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL;
}

void Train::callTrain()
{
	modelPedestrian = train(&prob, &param);
}

/* SAVE MODEL FOR PREDICTION */
char *Train::saveModelToFile()
{
	pathFileModel = new char [strlen(NameFileModel) + 1];
	strcpy(pathFileModel, NameFileModel);

	if(save_model(pathFileModel, modelPedestrian))
	{
	  std:: cerr << "can't save model to file " << std::endl;
	  exit(1);
	}

	return pathFileModel;
}


void Train:: freeModel()
{
	free_and_destroy_model(&modelPedestrian);
	destroy_param(&param);
	free(prob.y);

	for (int i = 0; i < prob.l; i++) {
		free(prob.x[i]);
	}

	free(prob.x);
}

char *Train::qualifierTraining()
{
	std::vector<ItemPng> vectPos;
	createPositiveVector(vectPos);
	trainPredictData(vectPos);
	fillModel();
	callTrain();
	char *pathFileModel = saveModelToFile();
	freeModel();

	return pathFileModel;
}

void Train::createPositiveVector(std::vector<ItemPng> &vectPos)
{
	int rStatus;
	char *pngName = new char [SIZE_STRING_NAME];
	struct ItemPng item;
	int x0, y0, x1, y1;
	FILE *fileLocations = fopen(pathFileLocations, "r");
	while ((rStatus = fscanf(fileLocations, "%s%d%d%d%d", pngName, &y0, &x0, &y1, &x1)) != EOF)
	{
		if (rStatus != NUM_PARAMETERS) {
			fprintf(stderr, "Can't read all option for current png.\n");
			exit(1);
		}
		item.name = pngName;
		item.x = x0;
		vectPos.push_back(item);
	}

	fclose(fileLocations);
	delete []pngName;
}

void Train::trainPredictData(std::vector<ItemPng> &vectPos)
{
	for (uint i = 0; i < vectPos.size(); i++) {
		std::string fileName;
		fileName = fileName.append(pathDir);

		if (fileName[fileName.size() - 1] != '/') {
		       fileName = fileName.append("/");
		}
		fileName = fileName.append(vectPos[i].name).append(".png");

		QImage img(fileName.c_str());
		reader.positive(vectPos[i].x, 0, vectPos[i].x + X_PIXEL, Y_PIXEL, img);
		for(int k = 0; k + X_PIXEL < vectPos[i].x; k += STEP_TRAIN_X) {
			reader.negative(k, 0, k + X_PIXEL, Y_PIXEL, img);
		}
		for(int k = vectPos[i].x + X_PIXEL; k + X_PIXEL < img.width(); k += STEP_TRAIN_X) {
			reader.negative(k, 0, k + X_PIXEL, Y_PIXEL, img);
		}
	}
}

void Train::trainPredictData2(std::vector<ItemPng> &vectNeg)
{
	for (uint i = 0; i < vectNeg.size(); i++) {
		std::string fileName;
		fileName = fileName.append(pathDir).append(vectNeg[i].name).append(".png");
		QImage img(fileName.c_str());
		reader.negative(vectNeg[i].x, 0, vectNeg[i].x + X_PIXEL, Y_PIXEL, img);
	}
}

int isInArea(int x, int x0)
{
	if ((x > x0 - X_PIXEL/2) && x < x0 + X_PIXEL/2) {
		return true;
	} else {
		return false;
	}
}

void Train::createNegativeVector(std::vector<ItemPng> &vectN, std::vector<ItemPng> &vectPs, std::vector<ItemPng> &vectNP)
{
	for (uint i = 0; i < vectNP.size(); i++) {
		bool positive = false;
		for (uint j = 0; j < vectPs.size(); j++) {
			if ((vectNP[i].name == vectPs[j].name) && isInArea(vectNP[i].x, vectPs[j].x)) {
				positive = true;
			}
		}
		if (positive == false) {
			vectN.push_back(vectNP[i]);
		}
	}
}

char *Train::bootstrapping(char *pathDirBt, char *pathFileLocBt)
{
	Test test(pathDirBt, pathFileModel);
	test.imagesClassification();

	std::cout<< "reader.instances before bootsrt:" << reader.getInstancesNumber() << std::endl;

	std::vector<ItemPng> negVector, posVector;
	pathDir = pathDirBt;
	pathFileLocations = pathFileLocBt;
	createPositiveVector(posVector);
	createNegativeVector(negVector, posVector,  test.vectNPLocations);
	trainPredictData2(negVector);

	std::cout<< "reader.instances after bootsrt:" << reader.getInstancesNumber() << std::endl;

	fillModel();
	callTrain();
	char *pathFileModel = saveModelToFile();
	freeModel();

	return pathFileModel;
}
