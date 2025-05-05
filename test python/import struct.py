import struct

# Abre el archivo binario
anterior = 0
with open('output.bin', 'rb') as f:
    while True:
        # Lee 8 bytes (64 bits)
        data = f.read(8)
        if not data:
            break
        number = struct.unpack('<q', data)[0]
        print(number)
