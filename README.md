# Lydia

![](https://img.shields.io/github/v/release/whitemech/lydia?sort=semver)
![Lydia CI pipeline](https://github.com/whitemech/lydia/workflows/Lydia%20CI%20pipeline/badge.svg)
![](https://codecov.io/gh/whitemech/lydia/branch/master/graph/badge.svg)
![](https://img.shields.io/badge/iso-c%2B%2B17-ff69b4)
[![](https://img.shields.io/badge/build-cmake-lightgrey)](cmake.org/)
[![](https://img.shields.io/badge/test-Catch2-yellow)](https://github.com/catchorg/Catch2/)
[![](https://img.shields.io/badge/license-LGPLv3%2B-blue)](./LICENSE)

Lydia is a tool for LDLf translation to DFA and for LDLf synthesis.

## Preliminaries

Clone the repository (with submodules):
```
git clone https://github.com/whitemech/lydia.git --recursive
```

### CMake
We use CMake as a build tool. Please 
check the [official website](https://cmake.org/)
to download it for your platform.

### Flex and Bison
The project uses Flex and Bison for parsing purposes.

Firse check that you have them: ```whereis flex bison```

If no item occurs, then you have to install them:  
```sudo apt-get install -f flex bison```


### CUDD

The project depends on the CUDD library.

Firse check that you have it:
```whereis libcudd```

If no item occurs, then you have to install it.
Follow the instructions:

- enter `third_party/cudd`:
```
cd third_party/cudd
``` 
- Install CUDD:
```
./configure --enable-silent-rules --enable-obj --enable-dddmp --prefix=[install location]
sudo make install
```
  If you get an error about aclocal, this might be due to either
  1. Not having automake: 
```sudo apt-get install automake```
  2. Needing to reconfigure, do this before configuring: 
```autoreconf -i```
  3. Using a version of aclocal other than 1.14:
     modify the version 1.14 in configure accordingly.

# CryptoMiniSat

We use Cryptominisat as SAT solver library.

For Ubuntu systems:
```
sudo apt-get install libcryptominisat5-dev
```

For other systems, please look at
the [documentation](https://github.com/msoos/cryptominisat#compiling-in-linux).

## Graphviz

This tool uses Graphviz to display automata.
Please follow the install instructions on the official website:
<https://graphviz.gitlab.io/download/>.

On Ubuntu, this should work:
```bash
sudo apt-get install libgraphviz-dev
```

## Build

To build from source, clone the repository:
```
git clone https://github.com/whitemech/lydia.git --recursive
cd lydia
```

To build:

```bash
mkdir build && cd build
cmake ..
make -j4
```
Afterwards, to run the tests:
```
make test
```

To configure the build for development, 
use the flag `-DCMAKE_BUILD_TYPE=Debug`

## Scripts

In `scripts/` you can find useful scripts for development. In particular:

- `scripts/apply-clang-format.sh` applies the fixes provided by
  `clang-format`.
- `scripts/apply-clang-tidy.sh` applies the fixes provided by
  `clang-tidy`.
  
The same scripts but with the `check` prefix lets you check whether
fixes are needed or not.

For building:
- `build.sh`: for normal build
- `build-dev.sh` for development build

For benchmarking, use `./scripts/benchmark.sh` 
(after `./scripts/build.sh`). 

## Docker development image

We provide a Docker image for development.

- Build: `./scripts/docker-build.sh`

- Run: `./scripts/docker-run.sh`

## Notes for macOS users

Although we run and test Lydia on a Linux machine, you can build and run
Lydia on macOS systems. However, you may need to apply the following.
 
First, be sure you have the compiler toolchain for macOS installed.

Second, we recommend installing dependencies via [Homebrew](https://brew.sh/) (e.g. 
`brew install flex`, etc.). Also, notice that although macOS systems natively come with flex and bison pre-installed,
those are not up to date versions and won't work with Lydia. 
Hence, you have to make sure new versions of `flex` and `bison` executables are in your
system path as:
```
export PATH="/usr/local/opt/flex/bin:/usr/local/opt/bison/bin:$PATH"
```
Finally, in order to get the Lydia parser working, you may have to set the 
following environment variable up (e.g. in your `.bashrc`) as follows:

```
export CPLUS_INCLUDE_PATH="/System/Volumes/Data/usr/local/Cellar/flex/2.6.4_1/include/:$CPLUS_INCLUDE_PATH"
```
