#include <QtCore/QCoreApplication>
#include "QualityClassification.h"
#include <iostream>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	if (argc < 2) {
		std::cerr << "Not enough parameters.\n";
		exit(1);
	}

	QualityClassification quality(argv[1], argv[2]);
	quality.evaluateResult();

	return 0;
}
