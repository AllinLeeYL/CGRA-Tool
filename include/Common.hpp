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

    Trunc = 38, // Truncate integers
    ZExt = 39, // Zero extend integers
    SExt = 40, // Sign extend integers
    FPToUI = 41, // floating point -> UInt
    FPToSI = 42, // floating point -> SInt
    UIToFP = 43, // UInt -> floating point
    SIToFP = 44, // SInt -> floating point
    FPTrunc = 45, // Truncate floating point
    FPExt = 46, // Extend floating point
    PtrToInt = 47, // Pointer -> Integer
    IntToPtr = 48, // Integer -> Pointer
    BitCast = 49, // Type cast
    AddrSpaceCast = 50, // addrspace cast

    ICmp = 53,
    PHI = 55,
    Call = 56,
    Select = 57,
    /* Deprecated: Complex OP not defined in "Instruction.def" */
    FMulAdd = 100,
};

#define LOG_INFO std::cout<<rang::fg::cyan<<"[INFO] "<<rang::fg::reset
#define LOG_WARNING std::cout<<rang::fg::yellow<<"[WARNING] "<<rang::fg::reset
#define LOG_ERROR std::cout<<rang::fg::red<<"[ERROR] "<<rang::fg::reset
#define LOG_IDT std::cout<<"  "
#define GREEN(text) rang::fg::green<<text<<rang::fg::reset
#define YELLOW(text) rang::fg::yellow<<text<<rang::fg::reset

}
#endif