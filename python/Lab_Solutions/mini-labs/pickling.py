import pickle


# dump/load
try:
    print(1 / 0)
except ZeroDivisionError as exc:
    with open("exception.txt", "wb") as fh:
        pickle.dump(exc, fh)

with open("exception.txt", "rb") as fh:
    loaded_exc = pickle.load(fh)

print("dump/load example:")
print(loaded_exc)
print()


# dumps/loads
try:
    print(x)
except NameError as exc:
    dumps_string = pickle.dumps(exc)

loads_string = pickle.loads(dumps_string)
print("dumps/loads example:")
print(loads_string)
