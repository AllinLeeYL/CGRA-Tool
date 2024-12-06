#include "CGRAMapperPass.hpp"
#include "Mapper.hpp"
#include "json.hpp"
#include "rang.hpp"
#include <iostream>
#include <fstream>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/LoopIterator.h>

// using namespace std;
// using namespace llvm;
#define NDEBUG
using namespace cgratool;
using fg = rang::fg;       // console font fontgraound color
using style = rang::style; // console font style

llvm::cl::OptionCategory ToyCGRACat("toycgra Options");

static llvm::cl::opt<unsigned> ROW_SIZE (
    "cgra-row-size", llvm::cl::init(4), 
    llvm::cl::desc("Set the row size of the PE array of CGRA."),
    llvm::cl::cat(ToyCGRACat)
);

static llvm::cl::opt<unsigned> COL_SIZE (
    "cgra-col-size", llvm::cl::init(4), 
    llvm::cl::desc("Set the column size of the PE array of CGRA."),
    llvm::cl::cat(ToyCGRACat)
);

static llvm::cl::list<std::string> BasicBlocks (
    "bb",
    llvm::cl::desc("Specify <function, basic block1[;basic block2...]> pairs to accelerate.\n"
                   "If an unnamed basic block is to be accelerated,\n"
                   "'%' should be added before the basic block variable names.\n"
                   "eg:\n"
                   "  --bb=f:bb1;bb2 will extract one function with bb1 and bb2;\n"
                   "  --bb=f:%1 will extract one function with unnamed bb 1;\n"),
    llvm::cl::value_desc("function:bb1[;bb2...]"),
    llvm::cl::cat(ToyCGRACat)
);

static PARAMS parseParams();
// static std::list<llvm::Loop*>* getTargetLoops(llvm::Function& F, llvm::FunctionAnalysisManager& FAM, list<int>* trgetLoopIDs);
std::vector<std::string> split(std::string s, std::string delimiter);

llvm::PreservedAnalyses ToyMapper::run(llvm::Function& F, llvm::FunctionAnalysisManager& FAM) {
    static int run_func_counter = 0;
    PARAMS params;
    if (run_func_counter == 0) {
        /* Parse parameters */
        params = parseParams();
        std::cout<<"-------------------------------------\n";
        std::cout<<"| Params of ToyMapper:\n";
        std::cout<<"|   row-size:"<<ROW_SIZE<<"\n";
        std::cout<<"|   col-size:"<<COL_SIZE<<"\n";
        std::cout<<"|   target BBs:";
        for (std::string strPair : BasicBlocks)
            std::cout<<strPair<<";";
        std::cout<<"\n-------------------------------------\n";
    }
    run_func_counter += 1;
    
    std::vector<llvm::Loop*> targetLoops;
    for (auto bb_pair : params.bb_pairs) {
        if (F.getName().str() != bb_pair.first) {
            std::cout<<fg::cyan<< F.getName().str()<<fg::reset<< " is not in our target list\n";
            return llvm::PreservedAnalyses::all();
        } 
        std::cout<<fg::green<< F.getName().str()<<fg::reset<< " is one of our target list\n";
        llvm::LoopInfo &loopInfo = FAM.getResult<llvm::LoopAnalysis>(F);
        for (llvm::Loop* loopIter : loopInfo) {
            if (!loopIter->getSubLoops().empty() || 
                loopIter->getBlocks().size() != 1) {
                continue;
            }
#ifndef NDEBUG
            std::string bb_name = loopIter->getBlocks()[0]->getNameOrAsOperand();
#else
            std::string bb_name;
            if (bb_pair.second[0] != '%')
                bb_name = loopIter->getBlocks()[0]->getName().str();
            else {
                std::string BBName;
                llvm::raw_string_ostream OS(BBName);
                loopIter->getBlocks()[0]->printAsOperand(OS, false);
                bb_name = OS.str();
            }
#endif
            if (bb_name == bb_pair.second) {
                std::cout<<fg::green<<bb_name<<fg::reset<< "\n";
                targetLoops.push_back(loopIter);
            }
        }
    }
    if (targetLoops.empty()) {
        return llvm::PreservedAnalyses::all();
    }
    /* Construct the DFG, the CGRA, and the mapper. */
    DFG dfg(targetLoops);
    CGRA cgra(params.rows, params.columns);
    ExhaustiveMapper mapper(&dfg, cgra);
    // HeuristicMapper mapper(&dfg, &cgra);

    int RecMII = mapper.getRecMII();
    int ResMII = mapper.getResMII();
    std::cout<<fg::cyan<<"[INFO] "<<fg::reset<<"RecMII="<<RecMII<<" ResMII="<<ResMII<<"\n";
    int MII = std::max(RecMII, ResMII);

    // Display & Debug
    std::ofstream fdfg("dfg.dot", std::ios::out);
    dfg.generateDot(fdfg);
    fdfg.close();
    fdfg = std::ofstream("dfg-more.dot", std::ios::out);
    dfg.generateDot(fdfg, true);
    fdfg.close();
    std::ofstream fcgra("cgra.dot", std::ios::out);
    cgra.generateDot(fcgra);
    fcgra.close();
    std::ofstream fverilog("cgra.v", std::ios::out);
    cgra.generateVerilog(fverilog);
    fverilog.close();

    time_t start = time(NULL);
    Mapping mapping = mapper.map(MII);
    time_t end = time(NULL);

    if (mapping.isNull())
        std::cout<<fg::yellow<<"[Warning] "<<fg::reset<<"Could not find a legal mapping.\n";
    else {
        std::cout<<fg::green<<"Congratulations! "<<fg::reset<<"Exists a legal mapping\n";
        std::ofstream fmapping("mapping.dot", std::ios::out);
        mapping.generateDot(fmapping);
        fmapping.close();
    }
    std::cout<<fg::cyan<<"[INFO] "<<fg::reset<<"time used: "<<(end-start)<<" seconds.\n";
    return llvm::PreservedAnalyses::all();
}

static PARAMS parseParams(){
    PARAMS params;
    params.rows = ROW_SIZE;
    params.rows = COL_SIZE;
    for (std::string strPair : BasicBlocks) {
        std::string func_name = split(strPair, ":")[0];
        std::vector<std::string> bbs = split(split(strPair, ":")[1], ";");
        for (std::string bb : bbs)
            params.bb_pairs.push_back(std::pair<std::string, std::string>(func_name, bb));
    }
    return params;
}

// static list<llvm::Loop*>* getTargetLoops(llvm::Function& F, llvm::FunctionAnalysisManager& FAM, list<int>* targetLoopIDs) {
//     std::list<llvm::Loop*>* targetLoops = new list<llvm::Loop*>();
//     llvm::LoopInfo &LI = FAM.getResult<llvm::LoopAnalysis>(F);
//     int id = 0;
//     for (int loopID; !targetLoopIDs->empty(); targetLoopIDs->pop_front()) {
//         loopID = targetLoopIDs->front();
//         int curLoopID = 0;
//         for (llvm::LoopInfo::iterator loopIter=LI.begin(); loopIter!=LI.end(); loopIter++, id++) {
//             llvm::Loop* loop = *loopIter;
//             if (!loop->getSubLoops().empty()) { // only run on innermost loops
//                 continue;
//             }
//             if (curLoopID == loopID) {
//                 targetLoops->push_back(loop);
//             }
//             curLoopID++;
//         }
//     }
//     return targetLoops;
// }

// for string delimiter
std::vector<std::string> split(std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return {
        .APIVersion = LLVM_PLUGIN_API_VERSION,
        .PluginName = "toymapper",
        .PluginVersion = "v0.1",
        .RegisterPassBuilderCallbacks = [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef name, llvm::FunctionPassManager &FPM, llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                    FPM.addPass(ToyMapper());
                    return true;
                });
        }
    };
}
