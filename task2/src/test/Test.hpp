#ifndef TEST_HPP
#define TEST_HPP

#include <stdlib.h>
#include <QString>
#include "../src-core/Instances.hpp"

class Test {
	InstancesData reader;
	char *pathDir;
	char *pathFileModel;

	struct model* modelPedestrian;
public:

	Test(char *, char *);
	char *imagesClassification();
private:
	void loadModelFromFile();
	void deleteDotPng (char *);
	void classify(FILE *, char *);
	void suppression ();
};

#endif // TEST_HPP
