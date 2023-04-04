#ifndef LCGEN_H
#define LCGEN_H
#include <vector>

class LCgen {
	const unsigned long M = 256;
	const unsigned long A = 1103515245;
	const unsigned long C = 12345;

	/* Current value in sequence */
	unsigned long curr;

	public:
		LCgen(unsigned long seed);
		char get_next();
		std::vector<char> get_next(int n);
};
#endif
