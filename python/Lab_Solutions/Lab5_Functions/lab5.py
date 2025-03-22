import hashlib
import os
import string
import sys

from typing import NoReturn

my_global = 81

# Part 1
def looper(match_num, lower_bound, upper_bound):
    even_counter = 0
    for num in range(lower_bound, upper_bound + 1):
        if num == match_num:
            break
        elif num % 2 == 0:
            even_counter += 1
    return even_counter


# Part 2
def modify_global(to_what):
    global my_global
    my_global = to_what
    print(f"Global now = {my_global}")


# Part 3
def divisibility(div, *args):
    answer_list = []
    for num in args:
        if num % div == 0:
            answer_list.append(str(num))

    return ",".join(answer_list)


# Part 4
def file_hash(file_path):

    # simple
    # with open(file_path, 'rb') as my_file:
    #     data = my_file.read()
    #     return hashlib.md5(data).hexdigest()

    chunk_size = 1024
    with open(file_path, "rb") as my_file:
        hash_obj = hashlib.md5()
        bytes_read = my_file.read(chunk_size)
        while bytes_read:
            hash_obj.update(bytes_read)
            bytes_read = my_file.read(chunk_size)

        return hash_obj.hexdigest()


# Part 5
def hash_generator(directory):
    for my_file in os.listdir(directory):
        full_path = os.path.join(directory, my_file)
        if os.path.isfile(full_path):
            yield full_path, file_hash(full_path)


def hash_generator_ex(directory):
    for root, dirs, files in os.walk(directory):
        for f in files:
            full_path = os.path.join(root, f)
            yield full_path, file_hash(full_path)


def find_dupes(directory):
    """
    Given a directory, this find duplicate files based upon their contents.
    This is an example of a python documentation string.

    :param directory: (str) path to a directory
    :return: None
    """
    duplicate_hashes = []
    hash_dict = {}

    for file_path, hash_digest in hash_generator(directory):
        if hash_digest in hash_dict:
            hash_dict[hash_digest].append(file_path)
            if hash_digest not in duplicate_hashes:
                duplicate_hashes.append(hash_digest)
        else:
            hash_dict[hash_digest] = [file_path]

    if duplicate_hashes:
        for hash_digest in duplicate_hashes:
            print(f"{hash_digest} - {hash_dict[hash_digest]}")
    else:
        print("No duplicates found")


# Part 6
def rot13(in_str):
    letters = {}
    for index in range(26):
        letters[string.ascii_lowercase[index]] = string.ascii_lowercase[
            (index + 13) % 26
        ]
        letters[string.ascii_uppercase[index]] = string.ascii_uppercase[
            (index + 13) % 26
        ]

    out_str = ""
    for char in in_str:
        out_str += letters.get(char, char)

    return out_str


"""
EXAMPLE OF HOW TO DO IT EASIER
import codecs
def rot13Tom(string):
    return codecs.encode(string, "rot_13")

print(rot13("foobar"))
print(rot13(rot13("foobar")))
print(rot13Tom(rot13Tom("foobar")))
"""


def main() -> NoReturn:

    even_counter = looper(20, 0, 100)
    print(f"even count = {even_counter}")

    modify_global(18)

    the_str = divisibility(2, 8, 9, 10, 11, 12, 15, 16)
    print(the_str)

    for filename, hash_string in hash_generator("."):
        print(filename, hash_string)

    find_dupes(".")

    new_str = rot13("Hello")
    print(new_str)

    sys.exit(0)

if __name__ == "__main__":
    main()
