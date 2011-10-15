#include "QualityClassification.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src-core/Instances.hpp"

void printVector(std::vector<Item> vect);

QualityClassification::QualityClassification(char *arg1, char *arg2)
{
	pathOwnLocations = arg1;
	pathRightLocations = arg2;
	det = 0;
	gt = 0;
}

void QualityClassification::evaluateResult()
{
	std::vector<Item> vOwnLocations;
	std::vector<Item> vRightLocations;
	readFileLocations(pathOwnLocations, vOwnLocations);
	readFileLocations(pathRightLocations, vRightLocations);

	int tp = 0, fp = 0;
	int tpShift = 0;
	float recall, precision, fMeasure;

	for(std::vector<Item>::iterator i = vOwnLocations.begin(); i != vOwnLocations.end(); i++) {
		std::vector<Item>::iterator cur = findPosition(i->name, vRightLocations);
		for(std::vector<int>::iterator k = i->medianX.begin(); k != i->medianX.end(); k++) {
			det++;
			for(std::vector<int>::iterator j = cur->medianX.begin(); j != cur->medianX.end(); j++) {
				if(isInArea(*k, *j)) {
					tp++;
				}
			}
		}
	}


	for(std::vector<Item>::iterator i = vRightLocations.begin(); i != vRightLocations.end(); i++) {
		std::vector<Item>::iterator cur = findPosition(i->name, vOwnLocations);
		if (cur != vOwnLocations.end()) {
			for(std::vector<int>::iterator k = i->medianX.begin(); k != i->medianX.end(); k++) {
				gt++;
				for(std::vector<int>::iterator j = cur->medianX.begin(); j != cur->medianX.end(); j++) {
					if(isInArea(*j, *k)) {
						tpShift++;
						break;
					}
				}
			}
		} else {
			gt++;
		}
	}

	fp = det - tp;
	recall = (double) tpShift / gt;
	precision = (double) tp / det;
	fMeasure = 2 * recall * precision / ( recall + precision);
	printf ("\n\nrecall: %.5f%% (tp':%d. gt:%d)\n", recall * 100, tpShift, gt);
	printf ("precision: %.5f%% (tp:%d. fp:%d. det:%d)\n", precision * 100, tp, fp, det);
	printf ("F: %.5f%%\n", 100 * fMeasure);
}

std::vector<Item>::iterator QualityClassification::findPosition(char *name, std::vector<Item> &vect)
{
	for (std::vector<Item>::iterator i = vect.begin(); i != vect.end(); i++) {
		if (!strcmp(name, i->name)) {
			return i;
		}
	}
	std::cerr << "Not found name [" << name << "] in file my onw locations pedestrian." << std:: endl;
	return vect.end();
}

int QualityClassification::isInArea(int x, int x0)
{
	if ((x > x0 - X_PIXEL/2) && x < x0 + X_PIXEL/2) {
		return true;
	} else {
		return false;
	}
}

void QualityClassification::readFileLocations(char *pathLocations, std::vector<Item> &vLocations)
{
	int rStatus;
	char pngName[20];
	int x0, y0, x1, y1;
	FILE *fileLocations = fopen(pathLocations, "r");


	while ((rStatus = fscanf(fileLocations, "%s%d%d%d%d", pngName, &y0, &x0, &y1, &x1)) != EOF)
	{
		if (rStatus != 5) {
			fprintf(stderr, "Can't read all options for current png.\n");
			exit(1);
		}

		struct Item item;
		if (vLocations.size() && !strcmp(pngName, vLocations[vLocations.size()-1].name)) {
			vLocations[vLocations.size()-1].medianX.push_back(x0 + X_PIXEL / 2);
		} else {
			item.name = new char [strlen(pngName)+1];
			strcpy(item.name, pngName);
			item.medianX.push_back(x0 + X_PIXEL / 2);
			vLocations.push_back(item);
		}
	}
	fclose(fileLocations);
}
