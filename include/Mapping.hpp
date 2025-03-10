#ifndef INCLUDE_MAPPING_H_
#define INCLUDE_MAPPING_H_

#include "DFG.hpp"
#include "CGRA.hpp"
#include "MRRG.hpp"
#include <map>

namespace cgratool{

/** @brief This class can only tell you which FU your OP is mapped to.
 * 
 *  The T of mapped FU may be larger than II.
 *  This mapping is not guaranteed to be legal. You have to use isLegal() 
 *  to check its legality.
*/
class Mapping {
protected:
    std::map<DFGNode*, MRRGNode*> mapping;
    std::map<DFGEdge, std::vector<MRRGEdge>> route_map;
public:
    DFG* dfg;
    MRRG mrrg;
public:
    Mapping();
    // Mapping(std::vector<int> placement);
    Mapping(DFG* dfg, MRRG mrrg);
    Mapping(const Mapping& other);

    /** @brief Shuffle the mapping (for RandomMapper only) */
    Mapping& shuffle(int upper_bound=-1); 

    /** @brief Check the mapping's legality */
    bool isLegal();
    /** @brief Check if the mapping is null. (for top module to use) */
    bool isNull();
    Mapping& operator=(const Mapping& other);
    bool operator==(const Mapping& other);

    // Deprecated
    MRRGNode*& operator[](DFGNode* dfgnode);

    // Deprecated
    const DFGNode* operator[](MRRGNode* mrrgnode);

    std::vector<MRRGEdge>& operator[](DFGEdge);

    void clear();
    /** @brief TODO: Output a graph in the format of pure text. */
    void serialize(std::ostream& f);
    /** @brief Output a graphviz (.dot) graph to the ostream. */
    void generateDot(std::ostream& f);
    std::map<DFGNode*, MRRGNode*>::iterator begin();
    std::map<DFGNode*, MRRGNode*>::iterator end();
};

}
#endif