add_library(dfg OBJECT
    DFG.cpp
    DFGNode.cpp
    DFGEdge.cpp
)

target_link_libraries(dfg
    PUBLIC LLVMSupport 
    PUBLIC LLVMPasses 
    PUBLIC LLVMCore 
    PUBLIC LLVMIRReader
    PUBLIC LLVMCodeGen
)