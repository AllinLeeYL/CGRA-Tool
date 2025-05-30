#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include <map>
#include <chrono>
#include "Mapper.hpp"
#include "json.hpp"
#include "rang.hpp"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopIterator.h"
#include "llvm/Analysis/LoopAnalysisManager.h"
#include "llvm/Analysis/CGSCCPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/WithColor.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/CommandLine.h"

#define NDEBUG

using namespace llvm;
using fg = rang::fg;       // console font fontgraound color
using style = rang::style; // console font style

cl::OptionCategory CGRACat("toycgra Options");

static cl::opt<std::string> InputFileName(cl::Positional,
                                          cl::desc("<input IR file>"),
                                          cl::init(""),
                                          cl::value_desc(".ll filename"));

static cl::opt<std::string> OutputFileName("o",
                                           cl::desc("output mapping filename"),
                                           cl::init("mapping.dot"));

static cl::list<std::string> BasicBlocks (
    "bb",
    cl::desc("Specify <function, basic block1[;basic block2...]> pairs to accelerate.\n"
                   "If an unnamed basic block is to be accelerated,\n"
                   "'%' should be added before the basic block variable names.\n"
                   "eg:\n"
                   "  --bb=f:bb1,bb2 will extract one function with bb1 and bb2;\n"
                   "  --bb=f:%1 will extract one function with unnamed bb 1;\n"),
    cl::value_desc("function:bb1[,bb2...]"),
    cl::cat(CGRACat)
);
/* optional parameters */
static cl::opt<unsigned> ROW_SIZE (
    "row-size", cl::init(4), 
    cl::desc("Set the row size of the PE array of CGRA."),
    cl::cat(CGRACat)
);

static cl::opt<unsigned> COL_SIZE (
    "col-size", cl::init(4), 
    cl::desc("Set the column size of the PE array of CGRA."),
    cl::cat(CGRACat)
);

static cl::opt<std::string> ILPSolver (
    "ilp-solver", cl::init("CP-SAT"), 
    cl::desc("select the solver"),
    cl::value_desc("CP-SAT|SCIP"),
    cl::cat(CGRACat)
);

static cl::opt<std::string> Arch (
    "arch", cl::init("mesh"), 
    cl::desc("select the pre-defined architecture"),
    cl::value_desc("mesh|1hop"),
    cl::cat(CGRACat)
);

static cl::opt<bool> Debug (
    "ndebug", 
    cl::desc("not output debug information (dfg.dot) if this flag is set"),
    cl::cat(CGRACat)
);

void do_mapping(std::vector<BasicBlock*>);

int main(int argc, char** argv) {
    InitLLVM X(argc, argv);
    cl::HideUnrelatedOptions({&CGRACat, &getColorCategory()});
    cl::ParseCommandLineOptions(argc, argv, "A CGRA mapper takes IR as input and outputs mapping.\n");

    SMDiagnostic Err;
    LLVMContext Context;
    std::unique_ptr<Module> M = getLazyIRFileModule(InputFileName, Err, Context);
    if (!M) {
        Err.print(argv[0], errs());
        return 1;
    }
    
    LoopAnalysisManager LAM;
    FunctionAnalysisManager FAM;
    CGSCCAnalysisManager CGAM;
    ModuleAnalysisManager MAM;
    PassBuilder PB;
    PB.registerModuleAnalyses(MAM);
    PB.registerCGSCCAnalyses(CGAM);
    PB.registerFunctionAnalyses(FAM);
    PB.registerLoopAnalyses(LAM);
    PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);
    ModulePassManager PM;
    PM.run(*M, MAM);

    /* --- Below are the code building function <-> basic mapping. --- */
    std::map<Function*, std::vector<BasicBlock*>> FBMap; /* <func:<bb, ...>> */
    for (StringRef strPair : BasicBlocks) {
        const auto & [func_name, bbs] = strPair.split(":");
        Function* F = M->getFunction(func_name);
        if (!F) {
            errs() << argv[0] << ": program doesn't contain a function named '"
                   << func_name << "'!\n";
            return 1;
        }
        SmallVector<StringRef> bb_names;
        bbs.split(bb_names, ",", /*MaxSplit=*/-1, /*KeepEmpty=*/false);
        for (StringRef bb_name : bb_names) {
/* The current version of llvm prefer not to use getNameOrAsOperand() somehow.
   The code below is to provide support of unnamed BBs. */
#ifndef NDEBUG
            auto Res = llvm::find_if(func->first(), [&](const BasicBlock &BB) {
                return BB.getNameOrAsOperand() == bb_name;
            });
#else   
            llvm::Function::iterator Res;
            if (bb_name.substr(0, 1) == "%") {
                Res = std::find_if(F->begin(), 
                                   F->end(), 
                                   [&](const BasicBlock &BB) {
                    std::string tmpName;
                    raw_string_ostream OS(tmpName);
                    BB.printAsOperand(OS, false);
                    return OS.str() == bb_name;
                });
            } else {
                Res = std::find_if(F->begin(), 
                                   F->end(),
                                   [&](const BasicBlock &BB) {
                    return BB.getName() == bb_name;
                });
            }   
#endif
            if (Res == F->end()) {
                errs() << argv[0] << ": program doesn't contain a basic block named '"
                       << bb_name << "'!\n";
                return 1;
            }
            if (FBMap.count(F))
                FBMap[F].push_back(&*Res);
            else
                FBMap[F] = std::vector<BasicBlock*> {&*Res};
        }
    }
    if (FBMap.empty()) {
        cl::PrintHelpMessage();
        exit(1);
    }

    /* Make sure that BBs make up a loop */
    std::vector<std::vector<BasicBlock*>> loops;
    for (auto & [func, bbs] : FBMap) {
        LoopInfo &LI = FAM.getResult<LoopAnalysis>(*func);
        for (Loop* loop : LI) {
            std::vector<BasicBlock*> lbbs = loop->getBlocks().vec();
            if (std::find(bbs.begin(), bbs.end(), *lbbs.begin()) != bbs.end()) {
                if (!loop->getSubLoops().empty()) {
                    errs()<<"Subloop isn't empty!\n";
                    exit(1);
                }
                for (BasicBlock* bb : lbbs) {
                    auto iter = std::find(bbs.begin(), bbs.end(), bb);
                    if (iter == bbs.end()) {
                        errs()<<"Couldn't find bb"<<*bb<<" in loop "<<*loop<<"\n";
                        exit(1);
                    }
                    bbs.erase(iter);
                }
                loops.push_back(lbbs);
            }
        }
        if (!bbs.empty()) {
            errs()<<"bb";
            for (BasicBlock* bb : bbs)
                errs()<<*bb;
            errs()<<"don't belong to any loop. Treating them as loops may causing unexpected behaviours.\n";
            std::vector<BasicBlock*> dangerousLoop;
            for (BasicBlock* bb : bbs)
                dangerousLoop.push_back(bb);
            loops.push_back(dangerousLoop);
        }
    }

    for (std::vector<BasicBlock*> loop : loops) {
        do_mapping(loop);
    }
}

void do_mapping(std::vector<BasicBlock*> bbvec) {
    /* Construct the DFG, the CGRA, and the mapper. */
    cgratool::DFG dfg(bbvec);
    cgratool::CGRA cgra;
    if (Arch == "mesh")
        cgra = cgratool::CGRA(ROW_SIZE, COL_SIZE);
    else if (Arch == "1hop")
        cgra = cgratool::CGRA1Hop(ROW_SIZE, COL_SIZE);
    else {
        LOG_ERROR<<"no such architecture: "<<Arch<<"\n";
        exit(1);
    }

    /* Choose a mapper */
    cgratool::ILPMapper mapper(&dfg, cgra, ILPSolver);
    // cgratool::ExhaustiveMapper mapper(&dfg, cgra);
    // HeuristicMapper mapper(&dfg, &cgra);

    int RecMII = mapper.getRecMII();
    int ResMII = mapper.getResMII();
    std::cout<<fg::cyan<<"[INFO] "<<fg::reset<<"RecMII="<<RecMII<<" ResMII="<<ResMII<<"\n";
    int MII = std::max(RecMII, ResMII);

    // Display & Debug
    if (Debug) {
        std::ofstream fdfg("dfg.dot", std::ios::out);
        dfg.generateDot(fdfg);
        fdfg.close();
        fdfg = std::ofstream("dfg-more.dot", std::ios::out);
        dfg.generateDot(fdfg, true);
        fdfg.close();
    }
    // std::ofstream fcgra("cgra.dot", std::ios::out);
    // cgra.generateDot(fcgra);
    // fcgra.close();
    // std::ofstream fverilog("cgra.v", std::ios::out);
    // cgra.generateVerilog(fverilog);
    // fverilog.close();

    auto start = std::chrono::high_resolution_clock::now();
    // cgratool::Mapping mapping = mapper.map(MII);
    cgratool::Mapping mapping = mapper.map(MII); //! restore to original after debugging dummy
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    if (mapping.isNull())
        LOG_WARNING<<"Could not find a legal mapping.\n";
    else {
        std::cout<<GREEN("Congratulations! ")<<"A legal mapping exists.\n";
        std::ofstream fmapping(OutputFileName, std::ios::out);
        mapping.generateDot(fmapping);
        fmapping.close();
        LOG_INFO<<"The mapping has been written into \""<<OutputFileName<<"\"\n";
    }
    LOG_INFO<<"Mapping time used: "<<duration.count()<<"ms.\n";
    return;
}