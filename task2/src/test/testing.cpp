#include "Test.hpp"


int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "Not enough parameters.\n");
		exit(1);
	}

	Test test(argv[1], argv[2]);
	test.imagesClassification();



	return 0;
}
