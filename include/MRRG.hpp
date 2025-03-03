#ifndef INCLUDE_MRRG_H_
#define INCLUDE_MRRG_H_

#include "Common.hpp"
#include "CGRA.hpp"

namespace cgratool {

enum MRRGNodeType{
    FUNode,
    MuxNode,
    RegNode,
};

class MRRGNode {
public:
    int ID;
    MRRGNodeType type;
    int T;
    PE pe;
    int mux_allow;
public:
    bool occupied;
public:
    MRRGNode();
    MRRGNode(const MRRGNode& other);
    MRRGNode(int ID, MRRGNodeType type, PE pe, int T, int mux_allow=0);
    bool supportOP(OPCode op);
    std::string getLabel();
    MRRGNode& operator=(const MRRGNode& other);
    bool operator==(const MRRGNode& other) const;
    bool serialize(std::ostream& f,
                   bool display_attrs=false,
                   std::string label="") const ;
};

class MRRGEdge {
public:
    MRRGNode* src;
    MRRGNode* des;
public:
    bool occupied;
public:
    MRRGEdge(MRRGNode* src, MRRGNode* des);
};

using MRRGRoute = std::vector<MRRGNode*>;

/** @brief MRRG is a directed graph <V, E>
 *
 * MRRG is also a dynamic graph.
 * It demonstrates the connection among hardware components crossing cycles.
 * MRRG has an II to represent the initiation interval.
 * But MRRG can extend its nodes.T larger than the II.
 */
class MRRG {
public:
    int II;
    std::vector<MRRGNode*> nodes;
    std::vector<MRRGEdge*> edges; // directed edge
    CGRA cgra;
    // std::vector<std::pair<int, int>> edges; // directed edge
protected:
    int node_counter, edge_counter;
    int TT; /** TT records the cycle length of MRRG */
    std::vector<MRRGNode*> childrenOf(MRRGNode* node);
    void extendGraphUntil(int T);
public:
    MRRG();
    MRRG(CGRA cgra, int II=4);
    MRRG(const MRRG& other);
    ~MRRG();
    void reset();
    void generateDot(std::ostream& f);

    /** @brief This function plan routes from src -> des. (deprecated)
     *
     *  @return routes planned.
     */
    std::vector<MRRGRoute> planRoute(MRRGNode* src, MRRGNode* des);

    /** @brief This function find whether there is a path from src_t0 -> des_t0.
     *
     *  @param cross_cycle when true, find path from src_t0 -> des_t1. (suitable for anti-edge)
     *  @param occupy When occupy is true, the edge used to route from src to des is occupied.
     *             Please note that when occupy is true, this function will modify the MRRG graph itself.
     */
    std::vector<MRRGRoute> findPath(MRRGNode* src, MRRGNode* des, bool occupy=true);

    /** @brief This function try occupying nodes and edges on the path
     *
     *  @note This function will modify the mrrg itself.
     */
    bool tryOccupyPath(MRRGRoute path);

    /** @brief This function will retrieve FUs from the MRRG.
     *
     *  @return The returned vector will remain the same order whenever it's called
     */
    std::vector<MRRGNode*> getFUs(int T=-1);

    /** @brief This function will retrieve FUs of which T=T from the MRRG.
     *
     *  @return The returned vector will remain the same order whenever it's called
     */
    std::vector<MRRGNode*> getFUsOfT(int T);

    /** @brief This function will retrieve FUs of which T0 <= T < T1
     *
     *  @return The returned vector will remain the same order whenever it's called
     */
    std::vector<MRRGNode*> getFUsOfT(int T0, int T1);

    /** @brief This function try getting the edge from src to des.
     *
     *  @return Return the edge on success, otherwise return NULL.
     */
    MRRGEdge* getEdge(MRRGNode* src, MRRGNode* des) const;

    /** @brief Look for the node which is equal to node.
     * 
     * @return If the node is found, return the pointer to the node, else return NULL.
     */
    MRRGNode* operator[](MRRGNode node);

    /** @brief Look for the node which is equal to node.
     * 
     * @return If the node is found, return the pointer to the node, else return NULL.
     */
    MRRGNode* getNodeByDetial(MRRGNodeType, PE, int);

    MRRG& operator=(const MRRG& other);
    // bool operator==(const MRRG& other);
};

}

#endif
