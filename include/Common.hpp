#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_

#include "rang.hpp"

namespace cgratool{

enum OPCode {
    Br = 2,
    Add = 13,
    FAdd = 14,
    FMul = 18,
    Load = 32,
    Store = 33,
    GetElementPtr = 34,
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