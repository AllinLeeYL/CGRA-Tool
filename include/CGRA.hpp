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
    std::vector<std::vector<int>> adjList;
    std::vector<std::vector<int>> daisyChains;
    std::vector<std::pair<PE, PE>> connections; // deprecated default 1-direction conn.
    int nodeID;

public:
    int row, col;


public:
    CGRA() = default;
    CGRA(int row, int col, std::string arch);
    CGRA(const CGRA& other);
    CGRA& operator=(const CGRA& other);
    ~CGRA();
    void init2dmesh(int row, int col);
    int getPECount();

    /* graph modification */
    // bool addVertex(PE);
    // bool addEdge(int, int);

    /* IO */
    static CGRA loadFromToml(std::istream& f);
    void generateDot(std::ostream& f);
    void generateVerilog(std::ostream& f);
};

}

#endif