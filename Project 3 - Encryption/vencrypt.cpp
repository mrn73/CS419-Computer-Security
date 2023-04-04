#include <memory>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstdint>

#define BUF_SIZE 4096

void handle_error(std::string msg) {
	std::cout << msg << std::endl;
	std::exit(1);
}

/**
 * Reads an encryption key from a file into a vector of characters.
 *
 * @param key A reference to the vector of characters that will hold the encryption key.
 * @param f The input file stream to read the key from.
 */
void read_key(std::vector<char> &key, std::ifstream &f) {
	f.seekg(0, f.end);
	int length = f.tellg();
	f.seekg(0, f.beg);
	key.resize(length);
	f.read(key.data(), length);
}

/**
 * Encrypts the data in the given buffer using the given key.
 * 
 * @param buf  The buffer containing the data to be encrypted.
 * @param key  The encryption key.
 */
void __do_encrypt(std::vector<char> &buf, const std::vector<char> &key) {
	int i = 0;
	for (char &byte : buf) {
		/* DEBUGGING */ //std::cout << key[i] << " + " << byte << " = " << (key[i] + byte) << std::endl;
		byte += key[i];
		i = i < (key.size() - 1) ? i + 1 : 0;
	}
}

/**
 * Encrypts the input plaintext file stream using the given key and writes the output to 
 * the given ciphertext file stream.
 *
 * @param key The key used to encrypt the plaintext.
 * @param plaintext The input plaintext file stream to be encrypted.
 * @param cipher The output ciphertext file stream to which the encrypted data is written.
 */
void encrypt(const std::vector<char> &key, std::ifstream &plaintext, std::ofstream &cipher) {
	std::vector<char> buf(BUF_SIZE);

	do {
		plaintext.read(buf.data(), BUF_SIZE);
		if (plaintext.gcount() < BUF_SIZE)
			buf.resize(plaintext.gcount());
		__do_encrypt(buf, key);
		cipher.write(buf.data(), buf.size());
	} while (plaintext);
}

/**
 * Entry point of the program.
 * Expects three command-line arguments:
 * 		- keyfile,
 * 		- message,
 *		- ciphertext.
 *		
 * @param argc The number of command-line arguments.
 * @param argv An array of strings containing the command-line arguments.
 * @return An integer status code indicating whether the program executed successfully.
 */
int main(int argc, char **argv) {
	if (argc != 4) {
		handle_error("Plese supply a keyfile, cyphertext, and plaintext file");
	}

	std::ifstream key_file(argv[1], std::ios::binary);
	std::ifstream plain_file(argv[2], std::ios::binary);

	if (!key_file) {
		handle_error("Error opening keyfile.");
	}

	if (!plain_file) {
		handle_error("Error opening plaintext file.");
	}

	std::ofstream cipher_file(argv[3], std::ios::out | std::ios::binary);

	if (!cipher_file) {
		handle_error("Error opening cypher file.");
	}

	std::vector<char> key;
	read_key(key, key_file);
	encrypt(key, plain_file, cipher_file);
	return 0;
}