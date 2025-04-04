add_subdirectory(dfg)
add_subdirectory(hardware-modeler)
add_subdirectory(mapper)

add_executable(cgra-tool
    CGRATool.cpp
)

target_link_libraries(cgra-tool
    PUBLIC hardware-modeler
    PUBLIC dfg
    PUBLIC mapper
)

if(BUILD_LLVM_PASSES)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/llvm-plugin)
endif()