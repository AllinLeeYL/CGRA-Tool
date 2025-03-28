# CGRA-Tool

A CGRA tool, including a mapper that can map LLVM loops in the form of IR to targeted CGRA, integrated with LLVM as an standalone executable binary.

The idea of developing such a tool affiliating CGRA-related research and developing is to build a user-friendly, easy-to-use,  CGRA compilation framework. 

# Install

## Prerequisities

- Google ORTools
- LLVM (15.0.0+)
- make
- cmake

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
