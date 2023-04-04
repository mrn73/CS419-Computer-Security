#!/usr/bin/python3
import sys
import hashlib

INITIAL_HASH = "Initial-hash"
PROOF_OF_WORK = "Proof-of-work"
LEADING_ZEROS = "Leading-zero-bits"
HASH = "Hash"
REQS = [INITIAL_HASH, PROOF_OF_WORK, LEADING_ZEROS, HASH]

def print_error(msg):
    print("ERROR: " + msg)

def print_output(match, key, value, expected_val):
    if match:
        print("PASSED: " + str(key) + " matches header")
    else:
        print_error(key + " does not match header\n\texpected: " + str(expected_val) + "\n\treceived: " + str(value))

def num_leading_zeros(hex_str):
    zeros = 0
    for hex_val in hex_str:
        bin_rep = bin(int(hex_val, 16))[2:].zfill(4)
        for bit in bin_rep:
            if int(bit) != 0:
                return zeros
            zeros += 1
    return zeros

def check_zeros(num_zeros, pow_hash):
    zeros = num_leading_zeros(pow_hash)
    if zeros == int(num_zeros):
        return (True, zeros)
    return (False, zeros)

def check_hash(header_hash, hash_val):
    if header_hash == hash_val:
        return True
    return False

def check_validity(header_info, f_bytes):
    # Our initial hash calculation of the file
    sha_hash = hashlib.sha256(f_bytes).hexdigest()

    # Our proof-of-work value from the header-file. Assumed to be "" when none given.
    pow_val = header_info.get(PROOF_OF_WORK, "")
    if pow_val is None:
        pow_val = ""

    # Our final hash calculation -- hash(initial hash + provided proof-of-work)
    pow_hash = hashlib.sha256((sha_hash + pow_val).encode()).hexdigest()

    hash1_match = False
    zero_match = (False, None)
    hash2_match = False

    # For the required headers, check if they are in the dictionary. If they aren't
    # in the dictionary or they have no associated value, they are marked missing.
    # Otherwise, extract their values and compare them to our own calculations to
    # ensure their correctness.
    for req in REQS:
        if req not in header_info.keys() or header_info[req] is None:
            print_error("missing " + req + " in header")
        elif req == INITIAL_HASH:
            hash1_match = check_hash(header_info[INITIAL_HASH], sha_hash)
            print_output(hash1_match, INITIAL_HASH, header_info[INITIAL_HASH],
                    sha_hash)
        elif req == LEADING_ZEROS:
            zero_match = check_zeros(header_info[LEADING_ZEROS], pow_hash)
            print_output(zero_match[0], LEADING_ZEROS, header_info[LEADING_ZEROS],
                    zero_match[1])
        elif req == HASH:
            hash2_match = check_hash(header_info[HASH], pow_hash)
            print_output(hash2_match, HASH, header_info[HASH], pow_hash)

    # If these headers matched our calculations, then the header file passes.
    if hash1_match and zero_match[0] and hash2_match:
        print("pass")
    else:
        print("fail")

def main(headerfile, infile):
    with open(headerfile, 'r') as f:
        header_lines = f.readlines()

    # For every line in the file, extract the key (Initial-hash, Hash, etc) and the value.
    # The key and value must be separated by a space
    header_info = dict()
    for line in header_lines:
        key = ''
        if INITIAL_HASH in line:
            key = INITIAL_HASH
        elif PROOF_OF_WORK in line:
            key = PROOF_OF_WORK
        elif LEADING_ZEROS in line:
            key = LEADING_ZEROS
        elif HASH in line:
            key = HASH
        if key:
            # If we see a key but no value with it, the value is None
            try:
                header_info[key] = line.split()[1]
            except IndexError:
                header_info[key] = None

    with open(infile, 'rb') as f:
        f_bytes = f.read()
    check_validity(header_info, f_bytes)

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print_error("Input headerfile and original file")
        exit(1)
    main(sys.argv[1], sys.argv[2])
