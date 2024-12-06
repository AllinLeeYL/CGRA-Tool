#ifndef INCLUDE_CGRA_H_
#define INCLUDE_CGRA_H_

#include <list>
#include <vector>
#include <iostream>
#include <fstream>
#include "PE.hpp"

namespace cgratool{

class CGRA{
public:
    std::vector<PE> PEs; 
    std::vector<std::pair<PE, PE>> connections; // default 1-direction conn.
    int nodeID;

public:
    int row, col;

public:
    CGRA();
    CGRA(int row, int col);
    CGRA(const CGRA& other);
    CGRA& operator=(const CGRA& other);
    ~CGRA();
    int getPECount();
    void generateDot(std::ostream& f);
    void generateVerilog(std::ostream& f);
};

}

#endif