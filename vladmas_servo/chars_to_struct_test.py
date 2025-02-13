import struct

char_str = b'ABCDAFA'
char_bytes = struct.pack('2s', char_str)

print('char_str:', char_str)
print('char_bytes:', char_bytes)