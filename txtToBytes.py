import sys
import struct

if len(sys.argv) < 3:
    print("Usage: python convert.py <input.txt> <output.bin>")
    sys.exit(1)

input_file = sys.argv[1]
output_file = sys.argv[2]

with open(input_file, "r") as f_in, open(output_file, "wb") as f_out:
    for line in f_in:
        line = line.strip()
        if not line:
            continue

        # Parse as hex if starts with 0x, else decimal
        if line.lower().startswith("0x"):
            value = int(line, 16)
        elif line.lower().startswith("0b"):
            value = int(line, 2)
        else:
            value = int(line, 10)

        # Pack as little-endian 32-bit unsigned integer
        # < means little endian, I means 4 byte unsigned int
        f_out.write(struct.pack("<I", value))

print(f"Conversion complete: {output_file}")
