-include ../../config.mk
-include ../rules.mk

build:
	clang -S -emit-llvm -fno-vectorize -fno-slp-vectorize -fno-unroll-loops -ffp-contract=off -O1 -c -o mac2.ll mac2.c

extract:
	llvm-extract -S --bb "main:%7" ${BENCHNAME}.ll -o extracted.ll