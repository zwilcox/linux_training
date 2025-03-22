import string

# TASK 1: create a list (nums) with numbers 0-25 in order
# the long way
nums = []  # creates an empty list --- # also works: nums = list()
# iterate over each number in range 0-25 (26 is NOT included)
for index in range(26):
    # and add it to the list
    nums.append(index)

# list comprehension
nums = [index for index in range(26)]

################################################################################

# TASK 2: using a imported constant from the 'string' module

# Note: ascii_lowercase is a pre-defined string constant - not a method/function
lowercase_letters = string.ascii_lowercase

################################################################################

# TASK 3: create dictionary with letter keys and position values

# the long way (using TASK 2 answer)
lowercase_letters_dict = {}

for lowercase_letter in lowercase_letters:
    lowercase_letters_dict[lowercase_letter] = lowercase_letters.index(lowercase_letter)

# zip to iterate two iterables together (using TASK 2 answer)
for num, lowercase_letter in zip(nums, lowercase_letters):
    lowercase_letters_dict[lowercase_letter] = num

# dict constructor takes in key, value pairs so this is shorter
lowercase_letters_dict = dict(zip(lowercase_letters, nums))

# enumerate to iterate with index number and value (using TASK 1 and 2 answers)
for index, letter in enumerate(lowercase_letters):
    lowercase_letters_dict[letter] = nums[index]

# dict comphrehension has similar syntax to above list comprehension
lowercase_letters_dict = {lowercase_letters[i]: i for i in range(26)}

print("lowercase letters dictionary = {}".format(lowercase_letters_dict))
print()
################################################################################

# TASK 4:

# Note: with open()... automatically closes the file so no close() is needed!
with open("mypairs.txt", "w") as fh:
    # sort the dictionary and write key, values to file
    # iteration over dict defaults to keys. Could also use items() for key,val pair
    for key in sorted(lowercase_letters_dict):
        fh.write("key = {}, value = {}\n".format(key, lowercase_letters_dict[key]))
    fh.write("\n")

    # sort the dictionary in reverse order and write key, values to file
    for key in sorted(lowercase_letters_dict, reverse=True):
        fh.write("key = {}, value = {}\n".format(key, lowercase_letters_dict[key]))
    fh.write("\n")

data = open("mypairs.txt").read()
print("mypairs.txt")
print("-----------")
print(data)
