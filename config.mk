# LLVM_DIR := /opt/homebrew/opt/llvm
LLVM_DIR := /usr
ORTOOLS_DIR := /Users/allin/Code/or-tools-14.4.1
ABSL_CMAKE_PATH := /opt/homebrew/opt/abseil/lib/cmake/absl

OPT := ${LLVM_DIR}/bin/opt
CLANG := ${LLVM_DIR}/bin/clang
CLANG++ := ${LLVM_DIR}/bin/clang++
LLVM-DIS := ${LLVM_DIR}/bin/llvm-dis

LLVM_CMAKE_PATH := ${LLVM_DIR}/lib/cmake/llvm
ORTOOLS_CMAKE_PATH := $(subst $() $(),;,$(wildcard ${ORTOOLS_DIR}/lib/cmake/*))

CMAKE_PREFIX_PATH := ${LLVM_CMAKE_PATH};${ORTOOLS_CMAKE_PATH};${ABSL_CMAKE_PATH}
CMAKE_MODULE_PATH := ${CMAKE_PREFIX_PATH}

C_COMPILER := ${CLANG}
CXX_COMPILER := ${CLANG++}
