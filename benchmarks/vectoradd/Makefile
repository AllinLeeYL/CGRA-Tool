-include ../../config.mk
-include ../rules.mk

.PHONY: extract
extract:
	llvm-extract -S --bb "_Z6kernelPfS_S_:%5" ${BENCHNAME}.ll -o extracted.ll
	sed -i 's/@_Z6kernelPfS_S_.extracted/@extracted/g' extracted.ll