#include "llvm/IR/PassManager.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/LoopAccessAnalysis.h"
#include "llvm/IR/Instructions.h"

// 1. Check if the new LLVM 22+ path exists
#if __has_include(<llvm/Plugins/PassPlugin.h>)
    #include <llvm/Plugins/PassPlugin.h>
#else
    // 2. If not, fall back to the pre-LLVM 22 path
    #include <llvm/Passes/PassPlugin.h>
#endif

#include <llvm/Passes/PassBuilder.h>

using namespace llvm;

bool isStaticallySchedulable(Loop &L, ScalarEvolution &SE, const LoopAccessInfo &LAI) {
    
    // ---- CONDITION 1: Compile-Time Constant Trip Count ----
    // Returns 0 if the trip count is dynamic or cannot be determined at compile time
    unsigned tripCount = SE.getSmallConstantTripCount(&L);
    if (tripCount == 0) {
        // Loop count is variable (e.g., depends on a runtime argument)
        return false; 
    }

    // ---- CONDITION 2: No Data-Dependent Branching ----
    // For a pure static schedule, we want predictable execution paths.
    for (BasicBlock *BB : L.getBlocks()) {
        // Ignore the loop latch/header branching logic itself
        if (L.isLoopLatch(BB)) continue;

        Instruction *term = BB->getTerminator();
        if (auto *BI = dyn_cast<BranchInst>(term)) {
            // If an internal block branches conditionally based on data, 
            // it's not statically schedulable without hardware predication.
            if (BI->isConditional()) {
                return false;
            }
        }
    }

    // ---- CONDITION 3: Deterministic Memory & No Aliasing ----
    // Check if LLVM requires runtime pointer checks (as seen in your previous log)
    const RuntimePointerChecking *rtCheck = LAI.getRuntimePointerChecking();
    if (rtCheck && !rtCheck->getChecks().empty()) {
        // Requires dynamic pointer overlap verification -> Bad for pure static scheduling
        return false;
    }

    // ---- CONDITION 4: No Side-Effecting Calls ----
    // Ensure the loop body doesn't call opaque functions or system calls
    for (BasicBlock *BB : L.getBlocks()) {
        for (Instruction &I : *BB) {
            if (auto *CI = dyn_cast<CallInst>(&I)) {
                // If it calls a function that isn't intrinsic/inlined, we can't schedule it
                if (!CI->getCalledFunction() || !CI->getCalledFunction()->isIntrinsic()) {
                    return false;
                }
            }
        }
    }

    // All checks passed! The loop is a deterministic dataflow graph.
    return true;
}

class SchedulabilityPass : public PassInfoMixin<SchedulabilityPass> {
public:
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
        // Retrieve the required analyses from the manager
        LoopInfo &LI = FAM.getResult<LoopAnalysis>(F);
        ScalarEvolution &SE = FAM.getResult<ScalarEvolutionAnalysis>(F);
        LoopAccessInfoManager &LAA = FAM.getResult<LoopAccessAnalysis>(F);

        // Iterate over all top-level loops in the function
        for (Loop *L : LI) {
            // Get memory access details specific to this loop
            const LoopAccessInfo &LAI = LAA.getInfo(*L);

            if (isStaticallySchedulable(*L, SE, LAI)) {
                errs() << "SUCCESS: Loop in function " << F.getName() 
                       << " is perfectly statically schedulable!\n";
                // Your CGRA tool can now safely extract bounds and map to PEs here
            } else {
                errs() << "INFO: Loop in function " << F.getName() 
                       << " requires dynamic scheduling / fallback.\n";
            }
        }

        return PreservedAnalyses::all();
    }
};

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, 
        "Schedulability", 
        LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            // Register your pass string name with the pipeline
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "schedulability") {
                        FPM.addPass(SchedulabilityPass());
                        return true;
                    }
                    return false;
                });
        }
    };
}