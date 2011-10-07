#include "Train.hpp"


int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "Not enough parameters.\n");
		exit(1);
	}

	Train train(argv[1], argv[2]);
	train.qualifierTraining();

	return 0;
}
