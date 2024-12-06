#include"Mapper.hpp"
#include"rang.hpp"
#include<cmath>
#include<ctime>
#include<random>

using namespace std;
using namespace cgratool;
using fg = rang::fg;       // console font fontgraound color
using style = rang::style; // console font style

/* ------------- HeuristicMapper ------------- */

HeuristicMapper::HeuristicMapper(DFG* dfg, CGRA cgra) : Mapper(dfg, cgra) {;}

Mapping HeuristicMapper::map(int II, int time_limit) {
    time_t start = time(NULL);
    Mapping mapping;
    for (; mapping.isNull(); II++) {
        time_t now = time(NULL);
        if (now - start > time_limit) {
            return Mapping();
        }
        mapping = this->mapII(II);
    }
    return mapping;
}

Mapping HeuristicMapper::mapII(int II, int time_limit) {
    cout<<fg::cyan<<"[INFO] "<<fg::reset<<"mapping for II="<<II<<" starts.\n";
    MRRG mrrg(this->cgra, II);
    Mapping mapping;

    /* --- Debugging --- */
    string fname = "mrrg-";
    fname.append(std::to_string(II));
    fname.append(".dot");
    std::ofstream fmrrg(fname, std::ios::out);
    mrrg.generateDot(fmrrg);
    fmrrg.close();

    /* --- Mapping --- */
    std::map<DFGNode*, std::vector<MRRGNode*>> mappable_places;
    dfg->calculateCycle();
    for (DFGNode* node : dfg->getOps()) {
        mappable_places[node] = mrrg.getFUsOfT(node->earliestCycle, 
                                               node->latestCycle+1);
    }
    for (const auto & [node, places] : mappable_places) {

    }
    return mapping;
}