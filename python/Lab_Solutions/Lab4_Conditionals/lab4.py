# Iterates from 1 to 12

for x in range(1, 13):
    if x == 3:
        # Skips to the next iteration of the loop if the counter is 3
        continue
    if x == 11:
        # Exits the loop early if the counter equals 11
        break
    if x % 2 == 0:
        # Prints the counter in hex when it is even
        print(f"0x{x:x}")
    else:
        # Prints the counter in decimal when it is odd
        print(x)
