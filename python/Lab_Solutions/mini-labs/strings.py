phrase = "The quick brown fox jumped over the lazy dog"
print(phrase)
print()


# Use replace() to replace one string with another.  Note that since strings
# are immutable, the original string is unchanged.
print('Replacing "the" with "a"')
print(phrase.replace("the", "a"))
print()

print("The length of the string is {} characters".format(len(phrase)))
print()

# Use lower() to make the string all lowercase.  Again, the original string
# has not changed since strings are immutable.
print('The string in all uppercase is "{}"'.format(phrase.upper()))
print()


# Use the boolean keyword "in" to determine if the word is in the sequence
search_for = "Brown"
print('The word "{}" is in the string: {}'.format(search_for, search_for in phrase))
print()


# Use split() to split on whitespace to determine how many words are in the
# phrase
print("There are {} words in the string".format(len(phrase.split(" "))))

print()
print(phrase)
