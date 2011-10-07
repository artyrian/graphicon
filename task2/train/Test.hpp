#ifndef TEST_HPP
#define TEST_HPP

#include "Train.hpp"


class Test {
	InstancesData reader;
//	char *pathDir;
//	char *pathFileModel;

	struct model* modelPedestrian;
public:

	Test(char *, char *);
	char *imagesClassification();
private:
	void loadModelFromFile();
	void classify();
};

#endif // TEST_HPP
