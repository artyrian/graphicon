#include "Train.hpp"
#include <stdio.h>

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "Not enough parameters.\n");
		exit(1);
	}

	Train train(argv[1], argv[2]);
//	train.setC(atof(argv[3]));
	train.qualifierTraining();
	printf("Send any key when ready to bootstrapping.\n");
	getchar();
	train.bootstrapping();

	return 0;
}
