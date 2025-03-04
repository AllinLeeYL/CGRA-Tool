cmake_minimum_required(VERSION 3.19)

project(CGRA-Tool
        VERSION 2.0.0
        LANGUAGES C CXX)

## VSCode ##
# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)

# whether to compile LLVM pass plugin (default: OFF)
set (BUILD_LLVM_PASSES OFF)
set (CMAKE_CXX_STANDARD 20)
set (BUILD_SHARED_LIBS OFF)
# set(CMAKE_MACOSX_RPATH 0)
# set(CMAKE_CXX_FLAGS_DEBUG "-O2")
# set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(INCLUDE_DIR ${ROOT_DIR}/include)

# set output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

## LLVM ##
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Using LLVM config at ${LLVM_CONFIG}")
if (LLVM_PACKAGE_VERSION LESS 18.1.0)
    message(WARNING "Current LLVM version ${LLVM_PACKAGE_VERSION} is too low!")
endif()
include(AddLLVM)
add_definitions(${LLVM_DEFINITIONS})
set(NO_RTTI "-fno-rtti")
add_definitions(${NO_RTTI})
# llvm_map_components_to_libnames(llvm_libs
#         support core irreader native passes option codegen
# )

## OR-TOOLS ##
find_package(ortools REQUIRED CONFIG)
get_target_property(ORTOOLS_INCLUDE_DIRS ortools::ortools INTERFACE_INCLUDE_DIRECTORIES)
list(GET ORTOOLS_INCLUDE_DIRS 0 ORTOOLS_INCLUDE_DIRS)

# set_target_properties(ortools::ortools PROPERTIES
#     IMPORTED_NO_SONAME FALSE
#     IMPORTED_LOCATION "${ORTOOLS_INCLUDE_DIRS}/../lib/libortools.9.dylib"
# )

include_directories(BEFORE
    ${INCLUDE_DIR}
    ${LLVM_INCLUDE_DIRS}
    ${ORTOOLS_INCLUDE_DIRS}
)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib:${CMAKE_INSTALL_PREFIX}/lib64")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

install(TARGETS cgra-tool)
# install(TARGETS cgra-tool hardware-modeler dfg mapper)
# install(FILES 
#     ${ORTOOLS_INCLUDE_DIRS}/../lib/libortools.9.dylib
#     ${ORTOOLS_INCLUDE_DIRS}/../lib/libortools.9.10.dylib
#     TYPE LIB)
