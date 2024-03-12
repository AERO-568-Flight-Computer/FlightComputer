import struct

data = bytes([1,2,3,4,5,6,7,8,1,2,3,4])

interp_data = struct.unpack('3i', data)

print(str(len(data)))

print(list(map(lambda d:hex(d),interp_data)))