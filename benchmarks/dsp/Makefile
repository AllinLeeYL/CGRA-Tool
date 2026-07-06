-include ../../config.mk
-include ../rules.mk

.PHONY: extract
extract:
	llvm-extract -S --bb "main:%7" ${BENCHNAME}.ll -o extracted.ll