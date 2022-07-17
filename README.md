# Multithreaded-Kiwi

## Short Description

Description Here

## Visual Presentation Part

## Prerequisites

- Have cmake installed

## How to install

1. Clone the code from the repository,
2. cd to the directory containing the first CMakeLists.txt file
3. Run the following command to build the project

```bash
cmake -S . -B <path-to-build-directory> && cmake --build <path-to-build-directory>
```

## How to use

Executable file /bench/kiwi-bench supports write and read modes
Usage:

```bash
./kiwi-bench read/write <requests_number>
```

## Known Issues

- Cmake generates files with suffixes of .c.o and .c.o.d, instead of just .o and .d
- Working on scanning for libraries (Specifically sometimes lsnappy is missing)
- Compilation takes time
- make clean command does not remove testdb in the bench folder.

## Currenlty Working on

- Building the test suite using CTest and GoogleTest.

## Credits

Originally created by [Francesco Piccinno (nopper)](https://github.com/nopper/kiwi/)
