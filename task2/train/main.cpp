#include "Train.hpp"
#include "Test.hpp"

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "Not enough parameters.\n");
		exit(1);
	}

	Train train(argv[1], argv[2]);
	char *pathFileModel = train.qualifierTraining();

	Test test(argv[1], pathFileModel);
	test.imagesClassification();

	return 0;
}



/*

//for working example
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <stdlib.h>
#include "liblinear-1.8/linear.h"

#define Malloc(type,n) (type *)malloc((n)*sizeof(type))
//==========

  const int NUM_FEATURES = 4;

  // PARSE INPUT and split into train and test data
  std::vector< std::vector<double> > testFeatures;
  std::vector< std::vector<double> > trainFeatures;

  std::vector<int> testLabels;
  std::vector<int> trainLabels;

  std::ifstream in("../iris.data");
  assert(in.good());
  double tmp; // temp variable for the first feature
  int cnt = 0; // alternates train/test sets
  while (in >> tmp)
  {
    std::vector<double> features(NUM_FEATURES);
    features[0] = tmp;
    in.ignore(1,',');
    for (int i = 1; i < NUM_FEATURES; i++)
    {
      in >> features[i];
      in.ignore(1,',');
    }

    std::string label;
    in >> label;
    if (label.compare("Iris-setosa") && label.compare("Iris-versicolor")) // not one of those
    {
      assert(!label.compare("Iris-virginica")); // should be the third; ignore it
      continue;
    }

    int intLab = !label.compare("Iris-setosa")? +1: -1;

    // odd cnt => instance goes to test set, even => to train set
    ((cnt % 2) ? testFeatures : trainFeatures).push_back(features);
    ((cnt % 2) ? testLabels : trainLabels).push_back(intLab);
    ++cnt;
  }
  assert(trainLabels.size() == trainFeatures.size());
  assert(testLabels.size() == testFeatures.size());

  // FILL TRAIN STRUCTURE AND TRAIN
  struct problem prob;
  prob.l = int(trainLabels.size()); // number of instances
  prob.bias = 0; // bias feature
  prob.n = NUM_FEATURES+1; // number of features + label

  prob.y = Malloc(int, prob.l); // allocate space for labels
  prob.x = Malloc(struct feature_node *, prob.l); // allocate space for features

  for (size_t i = 0; i < trainLabels.size(); i++)
  {
    prob.x[i] = Malloc(struct feature_node, NUM_FEATURES+1);
    prob.x[i][NUM_FEATURES].index = -1;  // -1 marks the end of list
    for (int j = 0; j < NUM_FEATURES; j++)
    {
      prob.x[i][j].index = 1+j; // 1-based feature number
      prob.x[i][j].value = trainFeatures[i][j];
    }
    prob.y[i] = trainLabels[i];
  }


  // default values of params. don't change them if not sure (unless param.C)
  struct parameter param;
  param.solver_type = L2R_L2LOSS_SVC_DUAL;
  param.C = 1;
  param.eps = 1e-4;
  param.nr_weight = 0;
  param.weight_label = NULL;
  param.weight = NULL;

  struct model* modelIris = train(&prob, &param);

  // SAVE MODEL FOR PREDICTION

  if(save_model("model.txt", modelIris))
  {
    std:: cerr << "can't save model to file " << std::endl;
    exit(1);
  }

  free_and_destroy_model(&modelIris);
  destroy_param(&param);
  free(prob.y);
  for (int i = 0; i < prob.n; i++) free(prob.x[i]);
  free(prob.x);


  // NOW LOAD MODEL AND CLASSIFY
  if((modelIris=load_model("model.txt"))==0)
  {
    std:: cerr << "can't load model from file " << std::endl;
    exit(1);
  }

  int corr = 0;
  struct feature_node* x = Malloc(struct feature_node, NUM_FEATURES+1);
  x[NUM_FEATURES].index = -1;  // -1 marks the end of list
  for (size_t i = 0; i < testLabels.size(); i++)
  {
    for (int j = 0; j < NUM_FEATURES; j++)
    {
      x[j].index = 1+j; // 1-based feature number
      x[j].value = testFeatures[i][j];
    }

    int predict_label = predict(modelIris,x);
    if (predict_label == testLabels[i])
    {
      ++corr;
    }
  }
  free(x);

  std::cout << "Accuracy is " << double(corr) / testLabels.size() << std::endl;


  // PREDICTION WITH CONFIDENCE
  std::cout << "Confidences of prediction for Iris-setosa: ";
  {
  int corr = 0;
  struct feature_node* x = Malloc(struct feature_node, NUM_FEATURES+1);
  x[NUM_FEATURES].index = -1;  // -1 marks the end of list
  double prob_estimates[1];
  for (size_t i = 0; i < testLabels.size(); i++)
  {
    for (int j = 0; j < NUM_FEATURES; j++)
    {
      x[j].index = 1+j; // 1-based feature number
      x[j].value = testFeatures[i][j];
    }

    predict_values(modelIris,x,prob_estimates);
    std::cout << prob_estimates[0] << " "; // confidence for the first class (+1, in our case)
  }
  free(x);
  }

  free_and_destroy_model(&modelIris);
*/
