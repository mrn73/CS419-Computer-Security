#include <memory>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#define BUF_SIZE 4096
#define M 256
#define A 1103515245
#define C 12345

void handle_error(std::string msg) {
	std::cout << msg << std::endl;
	std::exit(1);
}

/**
 * Encrypts a buffer of bytes by XOR'ing bytes generated by a linear congruential generator with
 * the bytes in the buffer.
 * @param x The current value of the random sequence. This value is the seed on the first call.
 * @param buf Buffer that holds bytes read from a file.
 * @return the Next byte of the random sequence to be used in future calls of this function.
 */
char __do_encrypt(unsigned long x, std::vector<char> &buf) {
	for (char &byte : buf) {
		x = (A * x + C) % M;
		byte ^= x;
	}
	return x;
}

/**
 * Encrypts a file using a stream cipher.
 * @param seed The seed generated from the password,
 * @param infile The file read from
 * @param outfile The file being written to. If infile is plaintext, the outfile is the encrpytion.
 * 	  If the infile was the cipher, the outfile is the decryption.
 */
void encrypt(unsigned long seed, std::ifstream &infile, std::ofstream &outfile) {
	std::vector<char> buf(BUF_SIZE);
	unsigned long x = seed;

	do {
		infile.read(buf.data(), BUF_SIZE);
		if (infile.gcount() < BUF_SIZE)
			buf.resize(infile.gcount());
		x = __do_encrypt(x, buf);
		outfile.write(buf.data(), buf.size());
	} while (infile);
}

/**
 * Uses the sbdm hash to convert a string to a seed.
 * @param str The password used to create the seed.
 * @return The generated seed.
 */
unsigned long get_seed(const std::string &str) {
	unsigned long hash = 0;
	for (auto c : str) {
		hash = c + (hash << 6) + (hash << 16) - hash;
	}
	return hash;
}

/**
 * Entry point of the program.
 * Expects three command-line arguments for encryption:
 * 		- password,
 * 		- plaintext file,
 *		- ciphertext file.
 * for decryption:
 *		- password,
 * 		- ciphertext file.
 * 		- plaintext file.
 *		
 * @param argc The number of command-line arguments.
 * @param argv An array of strings containing the command-line arguments.
 * @return An integer status code indicating whether the program executed successfully.
 */
int main(int argc, char **argv) {
	if (argc != 4) {
		handle_error("Plese supply a password, cyphertext file, and plaintext file");
	}

	std::ifstream infile(argv[2], std::ios::binary);

	if (!infile) {
		handle_error("Error opening file.");
	}

	std::ofstream outfile(argv[3], std::ios::out | std::ios::binary);

	if (!outfile) {
		handle_error("Error opening file.");
	}

	unsigned long seed = get_seed(std::string(argv[1]));
	encrypt(seed, infile, outfile);
	return 0;
}