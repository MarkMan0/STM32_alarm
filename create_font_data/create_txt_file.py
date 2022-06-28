
def main():
    with open("out.txt", "w") as f:
        for i in range(32, 127):
            print(i)
            f.write(str(chr(i)))


if __name__ == '__main__':
    main()
