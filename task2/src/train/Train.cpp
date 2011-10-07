#include "Train.hpp"
#include <iostream>

Train::Train(char *argv1, char *argv2)
	: reader ()
{
	reader.setPathDir(argv1);
	reader.setPathFileLocations(argv2);
	reader.trainPredictData();
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
	fillModel();
	callTrain();
	char *pathFileModel = saveModelToFile();
	freeModel();

	return pathFileModel;
}
