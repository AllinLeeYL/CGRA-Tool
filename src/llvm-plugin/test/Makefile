-include ../../config.mk
.PHONY: compile extract

compile:
	${CLANG} -emit-llvm -fno-unroll-loops -ffp-contract=off -O1 -o kernel.bc -c kernel.cpp
	${CLANG} -S -emit-llvm -o kernel.ll kernel.bc
	${CLANG} -emit-llvm -fno-unroll-loops -ffp-contract=off -O1 -o sum.bc -c sum.cpp
	${CLANG} -S -emit-llvm -o sum.ll sum.bc

extract:
	llvm-extract -S --bb "main:%2" kernel.ll -o extracted.ll