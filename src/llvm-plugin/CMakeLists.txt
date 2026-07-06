if(NOT BUILD_LLVM_PASSES)
    return()
endif()

add_llvm_pass_plugin(schedulability-pass
    SchedulabilityPass.cpp
)

target_link_libraries(schedulability-pass
)
