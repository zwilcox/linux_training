phrase = "The quick brown fox jumped over the lazy dog"

# Split the string on whitespace with split().  Reverse the list using reverse().
# Join the list back together into a string using join() and a space delimiter.
print("Using reverse()")
splits = phrase.split()
splits.reverse()
print(" ".join(splits))
print()

# Split the string on whitespace using split().  Iterate over the string
# backwards using slicing and join it back together using join().
print("Iterating over the string backwards")
splits = phrase.split()
print(" ".join(splits[::-1]))
