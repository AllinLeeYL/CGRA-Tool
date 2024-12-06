-include ../../config.mk
-include ../rules.mk

.PHONY: extract
extract:
	llvm-extract -S --bb "_Z6kernelv:%6" ${BENCHNAME}.ll -o extracted.ll