#include "Test.hpp"
#include <iostream>

#include <QImage>

Test::Test(char *argv1, char *argv2)
	: reader()
{
	pathDir = argv1;
	pathFileModel = argv2;
//	reader.setPathDir(argv1);
//	reader.setPathFileModel(argv2);
//	reader.testPredictData();
}

char *Test::imagesClassification()
{
	loadModelFromFile();
	QDir dir(pathDir);
	dir.setFilter(QDir::Files);
	dir.setSorting(QDir::Name);
	QFileInfoList list = dir.entryInfoList(QStringList() << "*.png");
	char *pathFileLocations = new char[40];
	strcpy(pathFileLocations, "test-own-processed.idl");
	FILE *fileLocations = fopen(pathFileLocations, "w");

	std::cout << "It's work. Wait sometime..." <<std::endl;
	for (int i = 0; i < list.size(); ++i) {
		QString pathPng(pathDir);
		pathPng.append(list.at(i).fileName());
		QImage img(pathPng);

		reader.setInstancesNumber(0);
		for (int x0 = 0; x0 < img.width() - X_PIXEL; x0 += STEP_X_DETECTING) {
			reader.processInstance(x0, 0, x0 + X_PIXEL, Y_PIXEL, img);
			reader.incInstancesNumber();
		}

//		std::cout << qPrintable(QString("%1").arg(pathPng)) << "[" << img.width() << "] : ";
		char name[20];
		strcpy (name, list.at(i).fileName().toAscii().data());
		deleteDotPng(name);
		classify(fileLocations, name);
	}

	fclose(fileLocations);
	return pathFileLocations;
}

void Test::deleteDotPng(char *name)
{
	for (uint i = 0; i < strlen(name); i++) {
		if (name[i] == '.') {
			name[i] = '\0';
			break;
		}
	}

}

void Test::loadModelFromFile()
{
	if((modelPedestrian = load_model(pathFileModel))==0)
	{
	  std:: cerr << "can't load model from file " << std::endl;
	  exit(1);
	}
}

void Test::classify(FILE *fileLocations, char *name)
{
	struct feature_node* x = Malloc(struct feature_node, NUM_FEATURES+1);
	x[NUM_FEATURES].index = -1;  // -1 marks the end of list

	for (size_t i = 0; i < reader.getInstancesNumber(); i++) {
		for (int j = 0; j < NUM_FEATURES; j++) {
			x[j].index = 1+j;	// 1-based feature number
			x[j].value = reader.instancesFeatures[i][j];
		}
		int predict_label = predict(modelPedestrian, x);
//		std::cout  << predict_label;
		if (predict_label == 1) {
			fprintf(fileLocations, "%s\t%d\t%d\t%d\t%d\n", name, 0, STEP_X_DETECTING * i, 200, STEP_X_DETECTING  *i + X_PIXEL);
		}
	}
	reader.instancesFeatures.clear();
	free(x);
}
