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
 * Decrypts a buffer of data using the given key.
 *
 * @param buf The buffer of data to be decrypted.
 * @param key The encryption key used to decrypt the data.
 */
void __do_decrypt(std::vector<char> &buf, const std::vector<char> &key) {
	int i = 0;
	for (char &byte : buf) {
		/* DEBUGGING */ //std::cout << key[i] << " + " << byte << " = " << key[i] + byte << std::endl;
		byte = 256 - key[i] + byte;
		i = i < (key.size() - 1) ? i + 1 : 0;
	}
}

/**
 * Decrypts the contents of the ciphertext file using the given key, and writes the
 * decrypted data to the plaintext file.
 *
 * @param key The encryption key used to decrypt the data.
 * @param cipher The input file stream containing the encrypted data.
 * @param plaintext The output file stream to write the decrypted data to.
 */
void decrypt(const std::vector<char> &key, std::ifstream &cipher, std::ofstream &plaintext) {
	std::vector<char> buf(BUF_SIZE);

	do {
		cipher.read(buf.data(), BUF_SIZE);
		if (cipher.gcount() < BUF_SIZE)
			buf.resize(cipher.gcount());
		__do_decrypt(buf, key);
		plaintext.write(buf.data(), buf.size());
	} while (cipher);
}

/**
 * Entry point of the program.
 * Expects three command-line arguments:
 * 		- keyfile,
 * 		- ciphertext file,
 *		- message.
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
	std::ifstream cipher_file(argv[2], std::ios::binary);

	if (!key_file) {
		handle_error("Error opening keyfile.");
	}

	if (!cipher_file) {
		handle_error("Error opening cipher file.");
	}

	std::ofstream plain_file(argv[3], std::ios::out | std::ios::binary);

	if (!plain_file) {
		handle_error("Error opening plaintext file.");
	}

	std::vector<char> key;
	read_key(key, key_file);
	decrypt(key, cipher_file, plain_file);
	return 0;
}