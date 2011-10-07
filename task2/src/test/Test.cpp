#include "Test.hpp"
#include <iostream>
#include <stdlib.h>
#include <QImage>

Test::Test(char *argv1, char *argv2)
	: reader()
{
	reader.setPathDir(argv1);
	reader.setPathFileModel(argv2);
	reader.testPredictData();
}

char *Test::imagesClassification()
{
	/*
	loadModelFromFile();

	for (int i = 0; i < list.size(); ++i) {
		QFileInfo fileInfo = list.at(i);
		QImage img(fileInfo.fileName());

		int x0, x1, y0, y1;
		for (x0 = 0; x1 < img.width(); x0 += 5) {
			x1 = x0 + X_PIXEL;
			reader.processInstance(x0, y0, x1, y1, img);
	//		reader.instancesLabels.push_back(1);
			reader.numberInstances++;
		}
		classify();
	}
	*/

	return 0;
}

void Test::loadModelFromFile()
{
	if((modelPedestrian = load_model(NameFileModel))==0)
	{
	  std:: cerr << "can't load model from file " << std::endl;
	  exit(1);
	}
}

void Test::classify()
{
	/*
//	int corr = 0;
	struct feature_node* x = Malloc(struct feature_node, NUM_FEATURES+1);
	x[NUM_FEATURES].index = -1;  // -1 marks the end of list

	for (size_t i = 0; i < reader.getInstancesNumber(); i++)
	{
	  for (int j = 0; j < NUM_FEATURES; j++)
	  {
	    x[j].index = 1+j; // 1-based feature number
	    x[j].value = reader.instancesFeatures[i][j];
	  }

	  int predict_label = predict(modelPedestrian, x);
	  std::cout << "predict_label" >> predict_label;

	  if (predict_label == reader.instancesLabels[i])
	  {
	    ++corr;
	  }

	}

	free(x);
*/
//	std::cout << "Accuracy is " << double(corr) / reader.getInstancesNumber() << std::endl;
}
