import math, os

table_name = "sin_lut"

def function(index):
    return math.sin(index)

count = 1024
fixed_point_size = 8

output_filename = table_name + ".c"

if os.path.exists(output_filename):
    os.remove(output_filename)

with open(output_filename, "w") as f:
    f.write(f"const short {table_name}[{count}] = {{\n")

    column_count = 0

    for i in range(0, count):
        current_index = i*2*math.pi / count
        calc = function(current_index)
        
        fixedpoint = int(round(calc * 256))
        
        if column_count == 0:
            f.write("    ")
        
        f.write(f"{hex(fixedpoint)}, ")

        if column_count == 7:
            column_count = 0
            f.write("\n")
        else:
            column_count+=1

    f.write("};")
    