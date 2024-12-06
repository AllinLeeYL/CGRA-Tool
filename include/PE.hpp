#ifndef INCLUDE_PE_H_
#define INCLUDE_PE_H_

#include <iostream>
#include <fstream>
#include <vector>
// #include"NOC.hpp"
#include "Common.hpp"

namespace cgratool{

class FU {
public:
    std::vector<OPCode> supportedInsts;
public:
    FU();
    FU(const FU& other);
    bool supportOP(OPCode op);
    FU& operator=(const FU& other);
    bool operator==(const FU& other) const;
};

/* ------------------------------------ */

class PE {
public:
    int ID;
    FU fu;
public:
    PE();
    PE(int ID);
    PE(const PE& other);
    bool supportOP(OPCode op);
    PE& operator=(const PE& other);
    bool operator==(const PE& other) const;
    void generateVerilog(std::ostream& f);
};

}

#endif
