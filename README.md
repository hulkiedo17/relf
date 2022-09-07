# What is it?

relf is simple elf (Executable and Linkable Format) files reader.

# Features

Right now this program can read and print elf header of executable files. In the future i will add more options so you can see what the executable file contains (See TODO section).

# Dependencies

To build project, you need this:
- C compiler (gcc, clang)
- Shell (bash)
- Meson (meson, ninja)

# Build

Clone repo or download it, go to repo dir, and type this command:
```shell
$ ./build.sh
```

To install program, after build type this:
```shell
$ ./build.sh -i
```

# Options

Program options (type -h option):
```sh
usage: relf [options...]

options:
	-h        - help message
	-e [file] - prints elf header of specified executable file
```

Build script options (also type -h option):
```sh
$ ./build.sh -h
usage: ./build.sh [options...]

options:
	-r - compile release
	-d - compile debug
	-i - install
	-c - clean
	-h - prints this help message
```

# TODO

- [ ] option to print program header
- [ ] option to print section header
- [ ] option to print string and symbol tables
- [ ] option to print notes