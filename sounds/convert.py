import os

#get a list of all files in the "in" directory
in_files = os.listdir("sounds/in")
print(in_files)

for f in os.listdir("sounds/out"):
    os.remove(f"sounds/out/{f}")
for f in os.listdir("components/sounds"):
    if os.path.isfile(f"components/sounds/{f}"):
        os.remove(f"components/sounds/{f}")
for f in os.listdir("components/sounds/include"):
        os.remove(f"components/sounds/include/{f}")

for i,f in enumerate(in_files):
    #convert each file to a wav file
    out_name = f"s{i}.wav"
    c_file_name = f"s{i}.c"
    os.system(f'ffmpeg -i "sounds/in/{f}" -ar 44100 -ac 1 -c:a pcm_s16le -bitexact sounds/out/s{i}.wav')
    #write the header file
    with open(f"components/sounds/{c_file_name}", "w") as h:
        h.write(f"#include <stddef.h>\n")
        h.write(f"const size_t s{i}_len = {os.path.getsize(f'sounds/out/s{i}.wav')};\n")
        h.write(f"const unsigned char s{i}_data[] = {{")
        with open(f"sounds/out/{out_name}", "rb") as w:
            for byte in w.read():
                h.write(f"{byte},")
        h.write("};")

with open("components/sounds/include/sounds.h", "w") as h:
    h.write("#pragma once\n")

    for i in range(len(in_files)):
        h.write(f"extern const size_t s{i}_len;\n")
        h.write(f"extern const unsigned char s{i}_data[];\n")

    sound_names = " ".join([f"s{i}_data," for i in range(len(in_files))])
    h.write("const unsigned char* sounds[] = {" + sound_names + "};\n")

    sound_lengths = " ".join([f"&s{i}_len," for i in range(len(in_files))])
    h.write("const size_t* const sound_lengths[] = {"+sound_lengths+"};\n")
    
    h.write(f"const size_t num_sounds = {len(in_files)};\n")

cmakefile = "idf_component_register(SRCS "
srcs = " ".join([f"s{i}.c" for i in range(len(in_files))])
cmakefile += srcs + '\n INCLUDE_DIRS "include")\n'

with open("components/sounds/CMakeLists.txt", "w") as h:
    h.write(cmakefile)
