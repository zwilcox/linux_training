# Iterates from 1 to 12
for x in range(1, 13):

    import pdb

    pdb.set_trace()

    if x == 3:
        # Skips to the next iteration of the loop if the counter is 3
        continue
    elif x == 11:
        # Exits the loop early if the counter equals 11
        break
    elif x % 2 == 0:
        # Prints the counter in hex when it is even
        print("0x%x" % x)
    elif x % 2 == 1:
        # Prints the counter in decimal when it is odd
        print(x)
