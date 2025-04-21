#include "rang.hpp"
#include "DFG.hpp"
#include "llvm/Support/WithColor.h"
#include <iostream>
#include <fstream>

using namespace cgratool;
// using namespace std;
using namespace llvm;
using fg = rang::fg;       // console font fontgraound color
using style = rang::style; // console font style

/* -------------------- DFGNode -------------------- */
DFGNode::DFGNode() {;}

DFGNode::DFGNode(const DFGNode& other) {
    *this = other;
}

DFGNode::DFGNode(int ID, NodeType nodeType, Instruction* inst, OPCode opCode, DataType dataType) {
    this->ID = ID;
    this->nodeType = nodeType;
    this->opCode = opCode;
    this->dataType = dataType;
    this->inst = inst;
    this->cycle = -1;
    if (nodeType == CallNode) {
        Function* func = ((CallInst*)inst)->getCalledFunction();
        llvm::StringRef funcName = func->getName();
        if (funcName.find("llvm") == -1) {
            WithColor(errs(), raw_ostream::RED)<<"not recognized call function: "<<funcName<<"\n";
            exit(1);
        } else if (funcName.compare("llvm.fmuladd.f32") == 0) {
            this->opCode = FMulAdd;
        } else {
            WithColor(errs(), raw_ostream::RED)<<"not supported call function: "<<funcName<<"\n";
            exit(1);
        }
    }
}

/* Judge whether the node is an operation node. */
bool DFGNode::isOp() {
    return (this->nodeType==OPNode || this->nodeType==CallNode);
}

void DFGNode::serialize(std::ostream& f, bool debug) const {
    // name
    if (this->nodeType == ConstNode) {
        if (this->dataType == INT) {
            f << "const_int_" << this->intVal;
        }
    } else if (this->nodeType == ValueNode) {
        f << "value_"<<this->ID;
    } else if (this->opCode == FMulAdd) {
        f << "fmuladdf32_" << this->ID;
    } else if (this->nodeType == OPNode ){
        f<<this->inst->getOpcodeName()<<this->ID;
    }
    // information -- cycle
    if (!debug) 
        return;
    
    f << " [cycle="<<this->cycle<<"]";
}

DFGNode::~DFGNode () {;}

DFGNode& DFGNode::operator=(const DFGNode& other) {
    this->ID = other.ID;
    this->nodeType = other.nodeType;
    this->opCode = other.opCode;
    this->dataType = other.dataType;
    
    this->bb = other.bb;
    this->inst = other.inst;
    this->intVal = other.intVal;
    this->floatVal = other.floatVal;
    this->cycle = other.cycle;
    return *this;
}

bool DFGNode::operator==(const DFGNode& other) {
    return this->ID == other.ID &&
           this->nodeType == other.nodeType &&
           this->opCode == other.opCode &&
           this->dataType == other.dataType &&
           this->bb == other.bb &&
           this->inst == other.inst &&
           this->intVal == other.intVal &&
           this->floatVal == other.floatVal &&
           this->cycle == other.cycle;
}

bool DFGNode::operator!=(const DFGNode& other) {
    return !(*this == other);
}