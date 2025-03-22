class SuperList(list):
    def __init__(self, *args):
        super().__init__(args)

    def __iadd__(self, value):
        for index in range(len(self)):
            super().__setitem__(index, self[index] + value)
        return self

    def __setitem__(self, index, value):
        super().__setitem__(index, value * 4)


if __name__ == "__main__":
    my_super_list = SuperList(10, 11, 12, 13)
    print(my_super_list)
    my_super_list += 5
    print(my_super_list)
    my_super_list[1] = 3
    print(my_super_list)
