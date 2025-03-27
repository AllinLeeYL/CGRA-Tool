#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_

#include "rang.hpp"

namespace cgratool{

enum OPCode {
    Br = 2,

    Add = 13,
    FAdd = 14,
    Sub = 15, 
    FSub = 16,
    Mul = 17, 
    FMul = 18,
    UDiv = 19,
    SDiv = 20,
    FDiv = 21,
    URem = 22,
    SRem = 23,
    FRem = 24,
    // Logical operators (integer operands)
    Shl = 25,  // Shift left  (logical)
    LShr = 26, // Shift right (logical)
    AShr = 27, // Shift right (arithmetic)
    And = 28, 
    Or = 29,
    Xor = 30, 

    Load = 32,
    Store = 33,
    GetElementPtr = 34,
    ZExt = 39,
    ICmp = 53,
    PHI = 55,
    Call = 56,
    /* Deprecated: Complex OP not defined in "Instruction.def" */
    FMulAdd = 100,
};

#define LOG_INFO std::cout<<rang::fg::cyan<<"[INFO] "<<rang::fg::reset
#define LOG_WARNING std::cout<<rang::fg::yellow<<"[WARNING] "<<rang::fg::reset
#define LOG_IDT std::cout<<"  "
#define GREEN(text) rang::fg::green<<text<<rang::fg::reset
#define YELLOW(text) rang::fg::yellow<<text<<rang::fg::reset

}
#endif