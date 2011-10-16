#include "Train.hpp"
#include <stdio.h>

int main(int argc, char **argv)
{	
	if (argc < 3) {
		fprintf(stderr, "Not enough parameters.\n");
		exit(1);
	}

//	printf("\n\n\n = = START NEW PROGRAM. C is %s.\n", argv[3]);

	Train train(argv[1], argv[2]);
//	train.setC(atof(argv[3]));
	train.qualifierTraining();
	train.bootstrapping(argv[1], argv[2]);

	return 0;
}
