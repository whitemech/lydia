# Lydia
Lydia is a tool for LDLf translation to DFA and for LDLf synthesis.

## Requirements

We use CMake as a build tool. Please 
check the [official website](https://cmake.org/)
to download it for your platform.

## Build

To build:

```bash
mkdir build && cd build
cmake ..
make -j4
```
Afterwards, to run the tests:
```
./lib/test/LydiaLibTest
```
