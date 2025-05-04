import struct

# Abre el archivo binario
anterior = 0
with open('output.bin', 'rb') as f:
    while True:
        # Lee 4 bytes (32 bits)
        data = f.read(4)
        if not data:
            break
        # Desempaqueta los datos en un número entero (big-endian o little-endian según sea necesario)
        number = struct.unpack('<I', data)[0]  # <I significa little-endian, un entero sin signo de 4 bytes
        print(number)
