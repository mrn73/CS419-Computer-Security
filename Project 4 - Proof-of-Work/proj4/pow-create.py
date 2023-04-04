#!/usr/bin/python3
import sys
import hashlib
import time

# Generates all combinations of string from a given set of chars
class StrGen():
    def __init__(self):
        self.str = ""

        # List of chars that can be used to make a string
        self.char_vals = [val for val in range(33,127)]
        self.char_vals.remove(34)
        self.char_vals.remove(39)
        self.char_vals.remove(96)

        # Current sequence of characters (string). Each element holds an index value
        # of a char in char_vals
        self.char_seq = [0]

    # Gets the next string and computes the next iteration.
    def get_next(self):
        string = ""
        for val in self.char_seq:
            string = string + chr(self.char_vals[val])

        self.char_seq[-1] = self.char_seq[-1] + 1
        carry_over = 0
        for i in range(len(self.char_seq) - 1, -1, -1):
            self.char_seq[i] = self.char_seq[i] + carry_over
            if self.char_seq[i] < len(self.char_vals):
                carry_over = 0
                break
            carry_over = 1
            self.char_seq[i] = 0

        # Every combination at current str size is expired. Add a new character
        if carry_over > 0:
            self.char_seq[0] = 0
            self.char_seq.append(0)

        return string

def handle_error(msg):
    print("Error: " + msg)
    exit(1)

def num_leading_zeros(hex_str):
    zeros = 0
    for hex_val in hex_str:
        bin_rep = bin(int(hex_val, 16))[2:].zfill(4)
        for bit in bin_rep:
            if int(bit) != 0:
                return zeros
            zeros += 1
    return zeros

# Uses a string to make a sha256 hash.
# The string can be decoded or in bytes format
def sha2_hex_str(string):
    if type(string) is bytes:
        ret = hashlib.sha256(string).hexdigest()
    else:
        ret = hashlib.sha256(string.encode()).hexdigest()
    return ret

def main(n, infile):
    with open(infile, 'rb') as f:
        f_bytes = f.read()
    sha_hash = sha2_hex_str(f_bytes)
    str_gen = StrGen()

    iterations = 0
    start = time.time()
    while True:
        suffix = str_gen.get_next()
        new_hash = sha2_hex_str(sha_hash + suffix)
        leading_zeros = num_leading_zeros(new_hash)
        iterations += 1
        if leading_zeros >= n:
            break
    end = time.time()

    print("File: " + str(infile))
    print("Initial-hash: " + str(sha_hash))
    print("Proof-of-work: " + suffix)
    print("Hash: " + new_hash)
    print("Leading-zero-bits: " + str(leading_zeros))
    print("Iterations: " + str(iterations))
    print("Compute-time: " + str(end-start))

if __name__ == '__main__':
    if len(sys.argv) != 3:
        handle_error("Input difficulty and file")
    main(int(sys.argv[1]), sys.argv[2])
