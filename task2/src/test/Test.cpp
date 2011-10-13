#include "Test.hpp"
#include <iostream>
#include <string>

#include <QImage>

Test::Test(char *argv1, char *argv2)
	: reader()
{
	pathDir = argv1;
	pathFileModel = argv2;
}

const char *Test::imagesClassification()
{
	loadModelFromFile();
	QDir dir(pathDir);
	dir.setFilter(QDir::Files);
	dir.setSorting(QDir::Name);
	QFileInfoList list = dir.entryInfoList(QStringList() << "*.png");
	std::string pathFileLoc(pathLocations);
	FILE *fileLocations = fopen(pathFileLoc.c_str(), "w");

	std::cout << "It's work. Wait sometime..." <<std::endl;
	for (int i = 0; i < list.size(); ++i) {
		QImage img(list.at(i).filePath());

		reader.setInstancesNumber(0);
		for (int x0 = 0; x0 < img.width() - X_PIXEL; x0 += STEP_X_DETECTING) {
			reader.processInstance(x0, 0, x0 + X_PIXEL, Y_PIXEL, img);
			reader.incInstancesNumber();
		}

		std::string name(list.at(i).fileName().toStdString());
		classify(fileLocations, name.substr(0, name.length() - 4).c_str());
	}

	fclose(fileLocations);
	return pathFileLoc.c_str();
}


void Test::loadModelFromFile()
{
	if((modelPedestrian = load_model(pathFileModel))==0)
	{
	  std:: cerr << "can't load model from file " << std::endl;
	  exit(1);
	}
}

void Test::classify(FILE *fileLocations, const char *name)
{
	struct feature_node* x = Malloc(struct feature_node, NUM_FEATURES+1);
	x[NUM_FEATURES].index = -1;  // -1 marks the end of list
	std::vector<double> vPredictValues;
	double prob_estimates[1];

	for (size_t i = 0; i < reader.getInstancesNumber(); i++) {
		for (int j = 0; j < NUM_FEATURES; j++) {
			x[j].index = 1+j;	// 1-based feature number
			x[j].value = reader.instancesFeatures[i][j];
		}
		int predict_label = predict(modelPedestrian, x);
		if (predict_label == 1) {
			predict_values(modelPedestrian, x, prob_estimates);
	//		fprintf(fileLocations, "%s\t%d\t%d\t%d\t%d\n", name, 0, STEP_X_DETECTING * i, 200, STEP_X_DETECTING * i + X_PIXEL);
		} else {
			prob_estimates[0] = 0;
		}
		vPredictValues.push_back(prob_estimates[0]);
	}
	suppression(fileLocations, name, vPredictValues);
	reader.instancesFeatures.clear();
	free(x);
}


void Test::suppression(FILE *fileLocations, const char *namePng, std::vector<double> &vect)
{
	double max = vect[0];
	int max_index = 0;

	findMaximum(vect, &max, &max_index);
	while (max != 0) {
		writeToFile(fileLocations, namePng, max_index);
		vect[max_index] = 0;
		nullInArea(vect, max_index);
		max = vect[0];
		max_index = 0;
		findMaximum(vect, &max, &max_index);
	}
}

void Test::findMaximum(std::vector<double> &vect, double *max, int *max_index)
{
	for(uint i = 0; i < vect.size(); i++) {
		if (vect[i] > *max) {
			*max = vect[i];
			*max_index = i;
		}
	}
}


void Test::writeToFile(FILE *fileLocations, const char *name, int index)
{
	fprintf(fileLocations, "%s\t%d\t%d\t%d\t%d\n", name, 0, STEP_X_DETECTING * index, 200, STEP_X_DETECTING * index + X_PIXEL);
	ItemPng item;
	item.name = name;
	item.x = STEP_X_DETECTING * index;
	vectNPLocations.push_back(item);
}


void Test::nullInArea(std::vector<double> &vect, int index)
{
	int localArea = X_PIXEL / (2 * STEP_X_DETECTING);
	for(int i = - localArea; i <= localArea;  i++) {
		if ((index + i >= 0) && (index + i < (int)vect.size())) {
			vect[index + i] = 0;
		}
	}
}
