add_library(mapper OBJECT
    MRRG.cpp
    Mapper.cpp
    ExhaustiveMapper.cpp
    FeasibleMapper.cpp
    ILPMapper.cpp
    Mapping.cpp
)

target_link_libraries(mapper
    PUBLIC hardware-modeler
    PUBLIC dfg
    PUBLIC ortools::ortools
)

# add_custom_command(TARGET mapper
#     POST_BUILD
#     COMMAND ${CMAKE_COMMAND}
#     ARGS -E copy ${ORTOOLS_INCLUDE_DIRS}/../lib/libortools.9.dylib ${CMAKE_BINARY_DIR}/lib/.
#     COMMAND ${CMAKE_COMMAND}
#     ARGS -E copy ${ORTOOLS_INCLUDE_DIRS}/../lib/libortools.9.10.dylib ${CMAKE_BINARY_DIR}/lib/.)