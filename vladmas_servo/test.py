import struct
a = "Helllo"
def print_a():
    print(a)

print_a()

def main():
    print_a()
    print("Abra")
    c = 1.254
    d = struct.pack('<dd',c,c)
    e = struct.unpack('<dd',d)
    print(c)
    print(d)
    print(e)

if __name__ == '__main__' :
    main()