if(NOT BUILD_LLVM_PASSES)
    return()
endif()

add_llvm_pass_plugin(cgra-mapper-pass
    CGRAMapperPass.cpp
)

target_link_libraries(cgra-mapper-pass
    PUBLIC mapper
    PUBLIC hardware-modeler
)
