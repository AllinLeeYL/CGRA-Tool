-include ../../config.mk
-include ../rules.mk

ARGS ?= --bb "main:%7"

build:
	clang -S -emit-llvm -fno-vectorize -fno-slp-vectorize -fno-unroll-loops -ffp-contract=off -O1 -c -o mac.ll fibonacci.cpp
extract:
	llvm-extract -S --bb "main:%7" ${BENCHNAME}.ll -o extracted.ll