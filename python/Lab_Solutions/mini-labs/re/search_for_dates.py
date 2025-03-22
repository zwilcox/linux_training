"""
import re

pattern = r"(19|20)\d\d[- /.](0[1-9]|1[012])[- /.](0[1-9]|[12][0-9]|3[01])"
my_re = re.compile(pattern)

with open("History.txt") as fh:
    lines = fh.readlines()

num_matches_found = 0
for line in lines:
    match_obj = my_re.search(line)
    if match_obj is None:
        continue
    num_matches_found += 1
    print(line, end='')

print()
print("{num_of_matches_found} matches found'
"""

#####
import re

pattern = r"(19|20)\d\d[- /.](0[1-9]|1[012])[- /.](0[1-9]|[12][0-9]|3[01])"
my_re = re.compile(pattern)

with open("History.txt") as fh:
    lines = [line for line in fh if my_re.search(line)]
for line in lines:
    print(line, end="")
print(len(lines))
