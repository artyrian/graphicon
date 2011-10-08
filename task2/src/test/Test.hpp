#ifndef TEST_HPP
#define TEST_HPP

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
	void classify();
};

#endif // TEST_HPP
