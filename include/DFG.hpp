#ifndef INCLUDE_DFG_H_
#define INCLUDE_DFG_H_

#include "Common.hpp"
#include "llvm/IR/PassManager.h"
#include "llvm/Analysis/LoopInfo.h"
#include <list>
#include <iostream>

namespace cgratool{

enum NodeType {
    OPNode, /* Each OP node maps to an operation (instruction). */
    CallNode, /* Call nodes are function calls, such as "call float @llvm.fmuladd.f32" */
    ValueNode, /* Value nodes are values not computed inside the loop. 
   Value nodes should appear in DFG graph. */
    ConstNode, /* Constant nodes should appear in DFG graph. */
    NULLNode,
};

enum DataType {
    INT,
    FLOAT,
};

/*  
        / OPNode -> CallNode
DFGNode - ValueNode   (Hidden)
        \ ConstantNode (Hidden) 
*/

class DFGNode {
public:
    int ID;
    NodeType nodeType;
    OPCode opCode;
    DataType dataType; // int or float

public:
    /* Information the node might carry */
    llvm::BasicBlock* bb;
    llvm::Instruction* inst;
    int intVal;
    float floatVal;
    int cycle;
    
public:
    DFGNode();
    DFGNode(const DFGNode& other);
    DFGNode(int ID, NodeType nodeType, 
            llvm::Instruction* inst=NULL,
            OPCode opCode=PHI, 
            DataType dataType=INT);
    bool isOp(); // Judge whether the node is an operation node.
    void serialize(std::ostream& f, bool debug=false) const;
    ~DFGNode();
    DFGNode& operator=(const DFGNode& other);
    bool operator==(const DFGNode& other);
    bool operator!=(const DFGNode& other);
};


class DFGEdge {
public:
    int ID;
    DFGNode* src;
    DFGNode* des;
    int latency;
    bool isAnti;
    bool isCtrl;
public:
    DFGEdge(int ID, DFGNode* src, DFGNode* des, bool isAnti=false, bool isCtrl=false);
    bool isVisible();
    void serialize(std::ostream& f) const;
    DFGEdge();
    DFGEdge(const DFGEdge& other);
    ~DFGEdge();
    DFGEdge& operator=(const DFGEdge& other);
    bool operator==(const DFGEdge& other) const;
    bool operator!=(const DFGEdge& other) const;
    bool operator<(const DFGEdge& other) const;
};


class DFG
{
private:
    int node_counter, edge_counter;
    std::vector<DFGNode*> nodes;
    std::vector<DFGEdge> edges;
    std::vector<DFGEdge> ctrlEdges; // Control edges are all anti edges.

    std::vector<llvm::Instruction*> insts;
private: /* flags */
    bool cycleIsCalculated;
protected:
    DFGNode* parseCall(llvm::Instruction* inst);
    DFGNode* parsePhi(llvm::Instruction* inst);
    DFGNode* parseInt32OP(llvm::Instruction* inst);
    DFGNode* parseFloat32OP(llvm::Instruction* inst);
    DFGNode* parseOtherOP(llvm::Instruction* inst);
    DFGNode* parseGetElementPtr(llvm::Instruction* inst);
    DFGNode* parseLoad(llvm::Instruction* inst);
    DFGNode* parseStore(llvm::Instruction* inst);
    DFGNode* parseIcmp(llvm::Instruction* inst);
    DFGNode* parseBr(llvm::Instruction* inst);

    DFGEdge& addEdge(DFGNode* src, DFGNode* des, bool isCtrl=false);
    DFGNode* getNode(llvm::Instruction* inst);
    DFGNode* getNodeElseCreate(llvm::Instruction* inst);

    bool shouldIgnore(llvm::Instruction* inst);
    bool isAnti(DFGNode* srcNode, DFGNode* desNode);

    DFGNode* parseInst(llvm::Instruction* inst);
    void parseCtrlEdge();
public:
    DFG();
    DFG(std::vector<llvm::Loop*>& targetLoops);
    DFG(std::vector<llvm::BasicBlock*>& BBs);
    void generateDot(std::ofstream& f, bool show_more=false);
    int getOpCount();
    std::vector<DFGNode*> getOps();

    /** @brief This function will retrieve OPs of which (cycle % II) == T
     *
     *  @return The returned vector will remain the same order whenever it's called
     */
    std::vector<DFGNode*> getOpsOfCycleModII(int T, int II);

    /** @brief This function will retrieve OPs of which T == cycle
     *
     *  @return The returned vector will remain the same order whenever it's called
     */
    std::vector<DFGNode*> getOpsOfCycle(int T);
    /** @brief This function will retrieve OPs of which T0 <= cycle < T1
     *
     *  @return The returned vector will remain the same order whenever it's called
     */
    std::vector<DFGNode*> getOpsOfCycle(int T0, int T1);

    std::vector<DFGEdge> getEdges(bool visibleOnly=true);

    std::vector<DFGEdge> getEdgesTo(DFGNode*, bool visibleOnly=true, bool noAnti=true);
    std::vector<DFGEdge> getEdgesFrom(DFGNode*, bool visibleOnly=true, bool noAnti=true);
    std::vector<DFGEdge> getAntiEdgesTo(DFGNode*);
    std::vector<DFGEdge> getAntiEdgesFrom(DFGNode*);
    
    /** @brief This function will get edge whose source with srcID and destination with desID
     *
     *  @return DFGEdge&
     */
    DFGEdge& getEdge(int srcID, int desID);

    std::vector<std::vector<DFGNode*>> topologySortedNodes(bool r=false);
    /** @brief This function calculates the cycle of each OPNode and assign them 
     *  three properties: earestCycle, latestCycle, and cycle */
    void calculateCycle();
    int getRecMII();
    DFGNode& operator[](int ID);
    DFGNode& operator[](DFGNode node) const;
    ~DFG();
    // DFG& operator=(const DFG& other);
};

}

#endif