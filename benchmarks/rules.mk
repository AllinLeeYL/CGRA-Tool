CLANG := clang
CLANG++ := clang++
LLVM-DIS := llvm-dis

BENCHNAME ?= $(shell basename `pwd`)

source_files := $(wildcard *.cpp)
bc_files := $(source_files:%.cpp=%.bc)
ll_files := $(source_files:%.cpp=%.ll)

CFLAGS ?= -O2
# Optimizations that are usually not wanted -- they assume a more complicated ISA, and may remove loops altogether.
OPT_DISABLE_FLAGS ?= -fno-vectorize -fno-slp-vectorize -fno-unroll-loops -ffp-contract=off

## COMMON TARGETS ##

.DEFAULT: build

build: ${ll_files}

.PHONY: build

run: run_mapper

## FOR BUILDING ##

.PRECIOUS: %.bc
%.bc:
	${CLANG++} -emit-llvm -c '$*.cpp' -o '$*.bc' $(CFLAGS) $(OPT_DISABLE_FLAGS)

%.ll: %.bc
	${LLVM-DIS} '$*.bc'

clean:
	rm -f *.bc *.ll

## FOR RUNNING ##

run_mapper:
	'$(CGRA_MAPPER)' $(CGRA_MAPPER_ARGS) $(CGRA_ARCH_ARGS) -g $(firstword $(DFG_TARGETS))
