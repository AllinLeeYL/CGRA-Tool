#include "Mapper.hpp"

using namespace cgratool;

FeasibleMapper::FeasibleMapper(DFG* dfg, CGRA cgra) : Mapper(dfg, cgra) { ; }

Mapping FeasibleMapper::map(int II, int time_limit) {
    return Mapping();
}