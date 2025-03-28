-include ../../config.mk
-include ../rules.mk

.PHONY: extract
extract:
	llvm-extract -S --bb "multiply:%5" ${BENCHNAME}.ll -o extracted.ll