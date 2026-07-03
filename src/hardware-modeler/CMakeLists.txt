add_library(hardware-modeler OBJECT
    PE.cpp
    CGRA.cpp
)

target_link_libraries(hardware-modeler 
    PRIVATE tomlplusplus::tomlplusplus)