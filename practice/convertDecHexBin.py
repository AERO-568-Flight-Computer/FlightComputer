# Binary to Decimal
def bin_to_dec(bin):
    dec = 0
    for exponent, bit in enumerate(reversed(bin)):
        dec += int(bit) * (2 ** exponent)
    return dec

# Decimal to Hexadecimal
def dec_to_hex(dec):
    hex_chars = "0123456789ABCDEF"
    hex = ""
    while dec > 0:
        remainder = dec % 16
        hex = hex_chars[remainder] + hex
        dec = dec // 16
    return hex

# Binary to Hexadecimal
def bin_to_hex(bin):
    dec = bin_to_dec(bin)
    hex = dec_to_hex(dec)
    return hex

# Hexidecimal to Decimal
def hex_to_dec(hex):
    hex_chars = "0123456789ABCDEF"
    dec = 0
    hex = hex.upper()
    for exponent, char in enumerate(reversed(hex)):
        dec += hex_chars.index(char) * (16 ** exponent)
    return dec

# Decimal to Binary
def dec_to_bin(dec):
    bin = ""
    while dec > 0:
        bin = str(dec % 2) + bin
        dec = dec // 2
    return bin 

# Hexadecimal to Binary
def hex_to_bin(hex):
    dec = hex_to_dec(hex)
    bin = dec_to_bin(dec)
    return bin

'''
# EXAMPLES
# Convert dec to hex
print(dec_to_hex(220))  # in: 255 -> expected: FF

# Convert hex to dec
print(hex_to_dec('DC'))  # in: 'FF' -> expected: 255

# Convert dec to bin
print(dec_to_bin(220))  # in: 255 -> expected: 11111111

# Convert bin to dec
print(bin_to_dec('11011100'))  # in: 11111111 -> expected: 255

# Convert bin to hex
print(bin_to_hex('11011100'))  # in: 11111111 -> expected: FF

# Convert hex to bin
print(hex_to_bin('DC'))  # in: 'FF' -> expected: 11111111
'''

# Convert hex to bin
print(hex_to_bin('40AB'))  # in: 'FF' -> expected: 11111111
