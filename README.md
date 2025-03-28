# CGRA-Tool

A CGRA tool, including a mapper that can map LLVM loops in the form of IR to targeted CGRA, integrated with LLVM as an standalone executable binary.

The idea of developing such a tool affiliating CGRA-related research and developing is to build a user-friendly, easy-to-use,  CGRA compilation framework. 

# Install on Ubuntu24.04

## Build from Source

Prerequisities:

 - Google ORTools
 - LLVM (15.0.0+)
 - make
 - cmake
 - git

Install Google OR-Tools from source (recommended). Installing Google OR-Tools to `/usr/local` saves your time struggling with exposing 'ortoolsConfig.cmake' to CMake.

```bash
sudo apt update
sudo apt install -y build-essential cmake make lsb-release
git clone https://github.com/google/or-tools
cd or-tools && git checkout tags/v9.11 -b v9.11
cmake -S . -B build -DBUILD_DEPS=ON
cd build && make
make install
```

You can also install OR-Tools from pre-built binary. But you have to make sure your cmake can find cmake configuration files provided by OT-Tools, such as 'ortoolsConfig.cmake'.

```bash
sudo apt update
sudo apt install -y build-essential cmake make lsb-release curl
curl -O -J "https://github.com/google/or-tools/releases/download/v9.11/or-tools_amd64_ubuntu-24.04_cpp_v9.11.4210.tar.gz"
tar xvf or-tools_amd64_ubuntu-24.04_cpp_v9.11.4210.tar.gz
```

Install LLVM

```bash
sudo apt install llvm clang
```

Clone this repository

```bash
git clone https://github.com/AllinLeeYL/CGRA-Tool.git
```

Use CMake preset to build

```bash
cd CGRA-Tool && cmake . --preset release-make
cd build/release-make && make
make install
cd ../.. && export PATH=$PATH:${PWD}/install/release-make/bin
```

Now, you can use type `cgra-tool --help` to get started.

Or instead, you want to install this tool to /usr/local

```bash
cd CGRA-Tool && cmake -B build . -DCMAKE_BUILD_TYPE=Release
cd build && make
make install
```

# Usage

```
$ cgra-tool --help
OVERVIEW: A CGRA mapper takes IR as input and outputs mapping.

USAGE: cgra-tool [options] <input IR file>

cgra-tool Options:

  --bb=<function:bb1[;bb2...]> - Specify <function, basic block1[;basic block2...]> pairs to accelerate.
                                 If an unnamed basic block is to be accelerated,
                                 '%' should be added before the basic block variable names.
                                 eg:
                                   --bb=f:bb1;bb2 will extract one function with bb1 and bb2;
                                   --bb=f:%1 will extract one function with unnamed bb 1;
  --cgra-col-size=<uint>       - Set the column size of the PE array of CGRA.
  --cgra-row-size=<uint>       - Set the row size of the PE array of CGRA.
  --ilp-solver=<string>        - Available solvers: <CP-SAT|SCIP>
```

For example, in `benchmarks/sum` directory, you can use the command below to map an extracted kernel onto a mesh topology homogeneous CGRA architecture.

```
cgra-tool --cgra-row-size=2 --cgra-col-size=2 --bb="_Z6kernelv.extracted:%1" extracted.ll
```

# Work in Progress

 - [x] ILP mapper.
 - [x] Visualize mapping result.
 - [ ] Support customized CGRA arhitecture.
 - [ ] Robost CMD parameters support.

# Existing Problems

 - Bugs encountered while mapping complex kernels.
