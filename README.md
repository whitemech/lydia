# Lydia

![](https://img.shields.io/github/v/release/whitemech/lydia?sort=semver)
![Lydia CI pipeline](https://github.com/whitemech/lydia/workflows/Lydia%20CI%20pipeline/badge.svg)
![](https://codecov.io/gh/whitemech/lydia/branch/master/graph/badge.svg)
![](https://img.shields.io/badge/iso-c%2B%2B17-ff69b4)
[![](https://img.shields.io/badge/build-cmake-lightgrey)](cmake.org/)
[![](https://img.shields.io/github/license/whitemech/lydia)](./LICENSE)

Lydia is a tool for the translation from Linear Temporal Logic on finite traces (LTLf) and 
Linear Dynamic Logic on finite traces (LDLf)  to DFA, and for LTLf/LDLf synthesis.

## Usage

This is the output of `lydia --help`:

```
A tool for LTLf/LDLf automata translation and LTLf/LDLf synthesis.
Usage: lydia [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -n,--no-empty               Enforce non-empty semantics.
  -q,--quiet                  Set quiet mode.
  -v,--verbose                Set verbose mode.
  -V,--version                Print the version and exit.
  -s,--summary                Print the summary.
  -p,--print                  Print the DFA.
  -g,--graphviz TEXT:PATH(non-existing)
                              Output the automaton in Graphviz format.
  -l,--logic ENUM:value in {ldlf->0,ltlf->1} OR {0,1} REQUIRED
                              Logic.
  --part TEXT:FILE            Part file.
  --env Needs: --part         Check env realizability.
[Option Group: Input format]
   
  [Exactly 1 of the following options is required]
  Options:
    -i,--inline TEXT Excludes: --file
                                Formula.
    -f,--file TEXT:FILE Excludes: --inline
                                File to formula.
```

For example, this command translates the LDLf formula in the file `examples/liveness.ldlf` to a DFA:
```
lydia -l ldlf -f examples/liveness.ldlf -g liveness
```

The output is in `liveness.svg`.

To do synthesis of an LTLf formula, you have to provide both the path to the formula
e.g. `examples/counter_1.ltlf` and a file to the partition file, e.g. `examples/counter_1.part`:
```
lydia -l ltlf -f examples/counter_1.ltlf --part examples/counter_1.part
```

> :warning: We apologize for lack of thorough documentation, e.g. on the grammar accepted by 
>   the tool. We will address this issue as soon as possible. Please
>   feel free to contact one of the authors for specific help, or 
>   [open an issue](https://github.com/whitemech/lydia/issues/new/choose).


## Use the Docker image

The easiest way to use the software 
is to download the [Docker image](https://hub.docker.com/repository/docker/whitemech/lydia/general) 
with all the needed dependencies
and both the library and the tool already built and installed.

To pull the Docker image `whitemech/lydia:latest`
```
docker pull whitemech/lydia:latest
```

Instead, to build it from source:
```shell
./scripts/docker-build.sh
```

To run it, with the current working directory mounted, run:
```shell
./scripts/docker-run.sh
```

or, you can define an alias for `lydia` such that it always run inside a Docker container:
```
alias lydia="docker run -v$(pwd):/home/default -it whitemech/lydia lydia "
```

## Build from source

### Install the dependencies

#### CMake
We use CMake as a build tool. Please
check the [official website](https://cmake.org/)
to download it for your platform.

#### Flex and Bison
The project uses Flex and Bison for parsing purposes.

Firse check that you have them: `whereis flex bison`

If no item occurs, then you have to install them:  
```sudo apt-get install -f flex bison```

#### CUDD

The project depends on the CUDD library (version 3.0.0).

To install the CUDD library, run the following commands:

```shell script
wget https://github.com/whitemech/cudd/releases/download/v3.0.0/cudd_3.0.0_linux-amd64.tar.gz
tar -xf cudd_3.0.0_linux-amd64.tar.gz
cd cudd_3.0.0_linux-amd64
sudo cp -P lib/* /usr/local/lib/
sudo cp -Pr include/* /usr/local/include
```

Otherwise, build from source (customize `PREFIX` variable as you see fit).
```
git clone https://github.com/whitemech/cudd && cd cudd
PREFIX="/usr/local"
./configure --enable-silent-rules --enable-obj --enable-dddmp --prefix=$PREFIX
sudo make install
```

If you get an error about aclocal, this might be due to either
1. Not having automake:
   ```sudo apt-get install automake```
2. Needing to reconfigure, do this before configuring:
   ```autoreconf -i```
3. Using a version of aclocal other than 1.14:
   modify the version 1.14 in configure accordingly.

#### MONA

The projects depends on the MONA library, version v1.4 (patch 19).
We require that the library is compiled with different values for
parameters such as `MAX_VARIABLES`, and `BDD_MAX_TOTAL_TABLE_SIZE`
(you can have a look at the details [here](https://github.com/whitemech/MONA/releases/tag/v1.4-19.dev0)).

To install the MONA library, run the following commands:

```shell script
wget https://github.com/whitemech/MONA/releases/download/v1.4-19.dev0/mona_1.4-19.dev0_linux-amd64.tar.gz
tar -xf mona_1.4-19.dev0_linux-amd64.tar.gz
cd mona_1.4-19.dev0_linux-amd64
sudo cp -P lib/* /usr/local/lib/
sudo cp -Pr include/* /usr/local/include
```

Otherwise, download the source code and build the library:
```
git clone https://github.com/whitemech/MONA.git && cd MONA
./configure && make && sudo make install
```

#### Syft+

Lydia depends on Syft+ to perform synthesis.

First, install the Boost libraries.
```
sudo apt-get install libboost-dev
```

Install it with:
```shell script
git clone https://github.com/whitemech/Syft.git
cd Syft
git checkout v0.1.1
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j
sudo make install
```

#### Graphviz

This tool uses Graphviz to display automata.
Please follow the install instructions on the official website:
<https://graphviz.gitlab.io/download/>.

On Ubuntu, this should work:
```bash
sudo apt-get install libgraphviz-dev
```

### Build from source

To build from source, clone the repository:
```
git clone https://github.com/whitemech/lydia.git --recursive
cd lydia
```

Then:

```bash
mkdir build && cd build
cmake ..
make -j4
```

Afterwards, to install Lydia in your system:
```
sudo make install
```
This will install Lydia under the default path: `/usr/local/bin`. 
To specify a custom path, use the flag 
`-DCMAKE_INSTALL_PREFIX:PATH="<custom/path>"`
at configuration time.

## Tests

Finally, to run the tests:
```
make test
```

To configure the build for development, 
use the flag `-DCMAKE_BUILD_TYPE=Debug`

## Development

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

## License

This software is released under the GNU Lesser General Public License version 3 or later.

## Acknowledgements

This work has been partially funded by the [ERC Advanced Grant "WhiteMech"](whitemech.github.io/)
and by the [TAILOR research network](https://tailor-network.eu/).
