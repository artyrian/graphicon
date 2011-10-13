#ifndef QUALITYCLASSIFICATION_H
#define QUALITYCLASSIFICATION_H

#include <vector>

struct Item {
	char *name;
	std::vector<int> medianX;
};

class QualityClassification {
	char *pathOwnLocations;
	char *pathRightLocations;
	int det, gt;
public:
	QualityClassification(char *, char *);
	void readFileLocations(char *, std::vector<Item> &);
	void readFileRightLocations(char *, std::vector<Item> &);
	void evaluateResult();
	std::vector<Item>::iterator findPosition (char *, std::vector<Item> &);
	int isInArea(int, int);
	void deleteName(std::vector<Item> &);
};

#endif // QUALITYCLASSIFICATION_H
