add_subdirectory(dfg)
add_subdirectory(hardware-modeler)
add_subdirectory(mapper)

add_executable(cgra-tool
    CGRAMapper.cpp
)

target_link_libraries(cgra-tool
    PUBLIC hardware-modeler
    PUBLIC dfg
    PUBLIC mapper
    PUBLIC LLVMSupport 
    PUBLIC LLVMPasses 
    PUBLIC LLVMCore 
    PUBLIC LLVMIRReader
    PUBLIC LLVMCodeGen
)

if(BUILD_LLVM_PASSES)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/llvm-plugin)
endif()