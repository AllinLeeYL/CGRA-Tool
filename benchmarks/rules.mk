CLANG := clang
CLANG++ := clang++
LLVM-DIS := llvm-dis

BENCHNAME ?= $(shell basename `pwd`)

CGRA_MAPPER ?= cgra-tool

source_files := $(wildcard *.cpp)
bc_files := $(source_files:%.cpp=%.bc)
ll_files := $(source_files:%.cpp=%.ll)

CFLAGS ?= -O2
# Optimizations that are usually not wanted -- they assume a more complicated ISA, and may remove loops altogether.
OPT_DISABLE_FLAGS ?= -fno-vectorize -fno-slp-vectorize -fno-unroll-loops -ffp-contract=off

## COMMON TARGETS ##

.DEFAULT: run

## FOR RUNNING ##

run: run_mapper

run_mapper:
	${CGRA_MAPPER} ${ARGS} --arch=${ARCH} ${BENCHNAME}.ll

## FOR BUILDING ##

build: ${ll_files}

.PHONY: run run_mapper build extract loopcount schedulability

.PRECIOUS: %.bc

%.bc:
	${CLANG++} -emit-llvm -c '$*.cpp' -o '$*.bc' $(CFLAGS) $(OPT_DISABLE_FLAGS)

%.ll: %.bc
	${LLVM-DIS} '$*.bc'

loopcount:
	opt -passes='print<scalar-evolution>' -disable-output ${BENCHNAME}.ll

accessinfo:
	opt -passes='print<access-info>' -disable-output ${BENCHNAME}.ll

schedulability:
	opt -load-pass-plugin=../../install/release/lib/schedulability-pass.so -passes='schedulability' ${BENCHNAME}.ll

clean:
	- rm -f *.dot

cleanall:
	- rm -f *.bc *.ll *.dot
