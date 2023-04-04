#include <memory>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include "lcgen.h"

#define BUF_SIZE 4096
#define BLOCK_SIZE 16


void handle_error(std::string msg) {
	std::cout << msg << std::endl;
	std::exit(1);
}

/**
 * Shuffles the bytes of a block based on a random key-stream.
 * @param block The block being shuffled.
 * @param stream The stream of random bytes.
 */
void shuffle_bytes(std::vector<char> &block, std::vector<char> stream) {
	for (int i = 0; i < block.size(); i++) {
		char first = stream[i] & 0xf;
		char second = (stream[i] >> 4) & 0xf;
		char tmp = block[first];
		block[first] = block[second];
		block[second] = tmp;
	}
}

/**
 * Uses block chaining with padding and byte shuffling to encrypt a buffer of bytes.
 * @param rand The random number generator.
 * @param prev_block The last block that was encrypted.
 * @param buf The buffer of bytes to be encrypted.
 * @return The last encrypted block.
 */
std::vector<char> __do_encrypt(LCgen rand, std::vector<char> prev_block, std::vector<char> &buf) {
	unsigned int pad_count = 0;

	/*
	 * This means this buffer is from our final read. Increase its size
	 * by the number of padded bytes and initialize each new element to
	 * the value of the number of added pads. Adding padding ensures that
	 * the buffer size is a multiple of BLOCK_SIZE.
	 * NOTE: A file size that's a multiple of BUF_SIZE will result in a
	 * 	 final read of 0 bytes, since eof is triggered AFTER the last byte.
	 * 	 This means that we will have an entire block of padding.
	 */
	if (buf.size() < BUF_SIZE) {
		pad_count = BLOCK_SIZE - (buf.size() % BLOCK_SIZE);
		buf.resize(buf.size() + pad_count, pad_count);
	}

	std::vector<char> curr_block(BLOCK_SIZE);
	for (int i = 1; i <= buf.size(); i++) {
		curr_block[(i-1) % BLOCK_SIZE] = buf[i-1];

		/* Break up buf into chunks of BLOCK_SIZE */
		if (i % BLOCK_SIZE == 0) {
			/* curr_block XOR prev_block */
			for (int j = 0; j < BLOCK_SIZE; j++)
				curr_block[j] ^= prev_block[j];

			/* Get BLOCK_SIZE bytes from rand stream and shuffle curr_block */
			std::vector<char> stream = rand.get_next(BLOCK_SIZE);
			shuffle_bytes(curr_block, stream);

			/* curr_block XOR rand stream */
			for (int j = 0; j < BLOCK_SIZE; j++)
				curr_block[j] ^= stream[j];

			/* Write block back to buffer */
			for (int j = i - BLOCK_SIZE; j < i; j++)
				buf[j] = curr_block[j % BLOCK_SIZE];

			prev_block = curr_block;
		}
	}

	return prev_block;
}

/**
 * Reads bytes from an infile into a buffer which will be encrypted and written to the outfile.
 * @param seed The seed generated from the password.
 * @param infile The file read from.
 * @param outfile The file being written to. If infile is plaintext, the outfile is the encrpytion.
 * 	  If the infile was the cipher, the outfile is the decryption.
 */
void encrypt(unsigned long seed, std::ifstream &infile, std::ofstream &outfile) {
	std::vector<char> buf(BUF_SIZE);
	std::vector<char> prev_block;
	LCgen rand(seed);

	prev_block = rand.get_next(BLOCK_SIZE);
	do {
		infile.read(buf.data(), BUF_SIZE);
		if (infile.gcount() < BUF_SIZE)
			buf.resize(infile.gcount());
		prev_block = __do_encrypt(rand, prev_block, buf);
		outfile.write(buf.data(), buf.size());
	} while (infile);
}

/**
 * Uses the sbdm hash to convert a string to a seed.
 * @param str The password used to create the seed.
 * @return the Generated seed.
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
 * Expects three command-line arguments:
 * 		- password,
 * 		- plaintext file,
 *		- ciphertext file.
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