#ifndef LLVM_TRANSFORMS_TOYMAPPERPASS_H
#define LLVM_TRANSFORMS_TOYMAPPERPASS_H

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include <vector>
#include <map>
#include "llvm/Support/CommandLine.h"

namespace cgratool{

typedef struct PARAMS {
    int rows                      = 4;
    int columns                   = 4;
    std::vector<std::pair<std::string, std::string>> bb_pairs;
    bool heuristicMapping         = true;
    std::list<int>* targetLoopIDs = new std::list<int>;
    std::list<std::string>* pipelinedOpt    = new std::list<std::string>();
    std::map<std::string, int>* execLatency = new std::map<std::string, int>();
    std::map<std::string, std::list<int>*>* additionalFunc = new std::map<std::string, std::list<int>*>();
} PARAMS;

class ToyMapper: public llvm::PassInfoMixin<ToyMapper>{
public:
    llvm::PreservedAnalyses run(llvm::Function& F, llvm::FunctionAnalysisManager& AM);
protected:
    PARAMS readParams(std::string path);
};

}

#endif