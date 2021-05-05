import os, copy

working_dir = "./sketching"
input_filename = "hexagon_center_marked.img.bin"
output_filename = "hexagon_center_gen.img.bin"

os.chdir(working_dir)

with open(input_filename, "rb") as f:
    base = bytearray(f.read())

start_index = 5

indexes = {
    5: 2**0,
    6: 2**1,
    7: 2**2,
    8: 2**3,
    9: 2**4,
    10:2**5
}

final_data = bytearray()

last_edited_data = None

for index in range(64):
    print(f"Writing chunk {bin(index)}")
    edited_data = copy.copy(base)
    for pal, bitmask in indexes.items():
        # pal is even - bg is 2
        # pal is odd  - bg is 1
        if pal & 1:
            bg = b'\x02'
            fg = b'\x05'
        else:
            bg = b'\x01'
            fg = b'\x04'

        if index & bitmask:
            print(f" Replacing {pal} with {fg} (fg)")
            edited_data = edited_data.replace(pal.to_bytes(1, "little"), fg)
        else:
            print(f" Replacing {pal} with {bg} (bg)")
            edited_data = edited_data.replace(pal.to_bytes(1, "little"), bg)

    if (len(edited_data) != len(base)):
        raise ValueError("what")
            
    final_data += edited_data

with open(output_filename, "wb") as f:
    print(len(final_data))
    f.write(final_data)