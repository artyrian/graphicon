#ifndef TEST_HPP
#define TEST_HPP

#include <stdlib.h>
#include <QString>
#include "../src-core/Instances.hpp"
#include "vector"

const char pathLocations[]  = "test-own-locations.idl";

class Test {
	InstancesData reader;
	char *pathDir;
	char *pathFileModel;	
	struct model* modelPedestrian;
public:
	Test(char *, char *);
	const char *imagesClassification();
	std::vector<ItemPng> vectNPLocations;
private:
	void loadModelFromFile();
	void classify(FILE *, const char *);
	void suppression (FILE * fileLocations, const char *namePng, std::vector<double> &vect);
	void findMaximum(std::vector<double> &vect, double *max, int *max_index);
	void writeToFile(FILE * fileLocations, const char *name, int index);
	void nullInArea(std::vector<double> &vect, int index);
};

#endif // TEST_HPP
