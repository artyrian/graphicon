#include "Train.hpp"
#include <iostream>

Train::Train(char *arg1, char *arg2)
	: reader ()
{
	pathDir = arg1;
	pathFileLocations = arg2;
//	reader.setPathDir(arg1);
//	reader.setPathFileLocations(arg2);
//	reader.trainPredictData();
}

Train::~Train()
{

}

/* FILL TRAIN STRUCTURE */
void Train::fillModel()
{
	prob.l = int(reader.getInstancesNumber()); // number of instances
	prob.bias = 0; // bias feature
	prob.n = NUM_FEATURES+1; // number of features + label

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
	param.C = 1;
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
	trainPredictData();
	fillModel();
	callTrain();
	char *pathFileModel = saveModelToFile();
	freeModel();

	return pathFileModel;
}


void Train::trainPredictData()
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

		reader.positive(x0, y0, x1, y1, img);

/*
		reader.negativeX(x0, x1, img);
		reader.negative(x0, y0, x1, y1, img);
*/

		for(int i = 0; i + X_PIXEL < x0; i += STEP_TRAIN_X) {
			reader.negative(i, y0, i + X_PIXEL, y1, img);
		}
		for(int i = x1; i + X_PIXEL < img.width(); i += STEP_TRAIN_X) {
			reader.negative(i, y0, i + X_PIXEL, y1, img);
		}

	}

	fclose(fileLocations);
	delete []pngName;
	delete []fileName;
}
