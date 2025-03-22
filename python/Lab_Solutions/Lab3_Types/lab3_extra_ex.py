l = list(range(26))
print(l)
print()
from string import ascii_lowercase

print(ascii_lowercase)
print()
d = {chr(i + ord("a")): i for i in range(26)}
print(d)
du = {chr(i + ord("A")): i for i in range(26)}
d.update(du)
print(d)
print()
with open("mypairs.txt", "w") as fh:
    fh.write("\n".join(f"{k}, {v}" for k, v in sorted(d.items())))
    fh.write("\n\n")
    fh.write("\n".join(f"{k}, {v}" for k, v in sorted(d.items(), reverse=True)))
