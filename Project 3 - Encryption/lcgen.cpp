#include "lcgen.h"

/**
 * Initializes the LCgen with a seed
 * @param seed the seed value to use for initializing the LCgen object.
 */
LCgen::LCgen(unsigned long seed) : curr{seed} { }

/**
 * Gets the next term of the random sequence
 * @return next term (byte)
 */
char LCgen::get_next() {
	curr = (A * curr + C) % M;
	return curr;
}

/**
 * Gets the next n terms of the random sequence
 * @param n number of terms to get
 * @return vector containing the n random numbers
 */
std::vector<char> LCgen::get_next(int n) {
	std::vector<char> ret(n);
	for (int i = 0; i < n; i++)
		ret[i] = get_next();
	return ret;
}