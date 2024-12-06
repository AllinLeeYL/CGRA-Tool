#include "Mapper.hpp"
#include "rang.hpp"
#include <cmath>
#include <ctime>
#include <random>

using namespace std;
using namespace cgratool;
using fg = rang::fg;       // console font fontgraound color
using style = rang::style; // console font style
/* ------------- ExhaustiveMapper ------------- */

ExhaustiveMapper::ExhaustiveMapper(DFG *dfg, CGRA cgra) : Mapper(dfg, cgra) { ; }

Mapping ExhaustiveMapper::map(int II, int time_limit)
{
    time_t start = time(NULL);
    Mapping mapping;
    for (; mapping.isNull(); II++)
    {
        time_t now = time(NULL);
        if (now - start > time_limit)
            return Mapping();
        mapping = this->mapII(II);
    }
    return mapping;
}

Mapping ExhaustiveMapper::mapII(int II, int time_limit)
{
    cout << fg::cyan << "[INFO] " << fg::reset << "mapping for II=" << II << " starts.\n";
    Mapping mapping(this->dfg, MRRG(this->cgra, II));
    MRRG &mrrg = mapping.mrrg;
    std::vector<DFGNode *> ops = this->dfg->getOps();
    /* --- Debugging --- */
    string fname = "mrrg-";
    fname.append(std::to_string(II));
    fname.append(".dot");
    std::ofstream fmrrg(fname, std::ios::out);
    mrrg.generateDot(fmrrg);
    fmrrg.close();
    
    /* --- Get mappable places --- */
    std::map<DFGNode *, std::vector<MRRGNode *>> mappable_places;
    dfg->calculateCycle();
    std::sort(ops.begin(), ops.end(), [](const DFGNode *lhs, const DFGNode *rhs)
              { return lhs->latestCycle < rhs->latestCycle; });
    for (DFGNode *node : ops)
    {
        std::vector<MRRGNode *> mrrgnodes;
        for (MRRGNode *mrrgnode : mrrg.getFUsOfT(node->earliestCycle,
                                                 node->latestCycle + 1))
        {
            if (mrrgnode->supportOP(node->opCode))
                mrrgnodes.push_back(mrrgnode);
        }
        mappable_places[node] = mrrgnodes;
    }

    /* --- Use a counter to record every combination --- */
    std::vector<DFGNode *> nodes;
    std::vector<std::vector<MRRGNode *>> places;
    std::vector<std::vector<MRRGNode *>> pruned_places;
    std::vector<int> counter;
    for (const auto &[dfgnode, pss] : mappable_places)
    {
        nodes.push_back(dfgnode);
        places.push_back(pss);
        pruned_places.push_back(pss);
        counter.push_back(pss.size() - 1);
    }

    /* --- The most time-consuming loop --- */
    time_t start = time(NULL);
    while (counter[0] != -1)
    {
        time_t now = time(NULL);
        if (now - start > time_limit)
            return Mapping();
        mapping.clear();
        // for (int i=0; i<counter.size(); i++) {
        //     std::cout<<counter[i]<<" ";
        // }
        // std::cout<<"\n";
        /* --- Construct a new mapping using the counter --- */
        for (int i = 0; i < nodes.size(); i++)
        {
            DFGNode *dfgnode = nodes[i];
            MRRGNode *mrrgnode = pruned_places[i][counter[i]];
            mapping[dfgnode] = mrrgnode;
        }
        if (mapping.isLegal()) // exit
            return mapping;
        /* --- Update the counter --- */
        if (counter.back() != 0)
            counter.back()--;
        else
        {
            int i;
            for (i = counter.size() - 1; i > 0; i--)
            {
                if (counter[i] != 0)
                    break;
            }
            counter[i]--;
            for (int j = i + 1; j < counter.size(); j++)
                counter[j] = places[j].size() - 1;
        }
    }
    // std::cout<<fg::red<<"here"<<fg::reset<<"\n";
    return Mapping();
}