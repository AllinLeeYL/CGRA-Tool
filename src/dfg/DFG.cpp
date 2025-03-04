#include "llvm/IR/Constants.h"
#include "llvm/Support/Casting.h"
// #include "llvm/IR/Instruction.def"
#include "DFG.hpp"
#include "rang.hpp"
#include <stdexcept>
#include <fstream>

using namespace cgratool;
using namespace llvm;
// using namespace std;

using fg = rang::fg;       // console font fontgraound color
using style = rang::style; // console font style

bool isTerminalInst(Instruction* inst) {
    unsigned int opCode = inst->getOpcode();
    // cout << "terminal op code " << inst->getOpcode() << "\n";
    if (1 <= opCode && opCode <= 11) {
        return true;
    }
    return false;
}

DFG::DFG() {
    this->cycleIsCalculated = false;
}

DFG::DFG(std::vector<Loop*>& targetLoops) {
    std::vector<BasicBlock*> BBs;
    for(Loop* loop : targetLoops) {
        for(BasicBlock* bb: loop->getBlocks()) {
            BBs.push_back(bb);
        }
    }
    this->node_counter = 0;
    this->edge_counter = 0;
    this->cycleIsCalculated = false;
    this->nodes.clear();
    this->edges.clear();
    this->insts.clear();
    // extract instructions to this->insts
    for(BasicBlock* bb: BBs) {
        outs()<<"bb "<<bb->getName().str()<<":\n";
        for(BasicBlock::iterator inst=bb->begin(); inst!=bb->end(); inst++) {
            this->insts.push_back(&*inst);
            outs()<<"  "<<*inst<<"\n";
        }
    }
    // parse instruction
    for (Instruction* inst : this->insts) {
        this->parseInst(&*inst);
    }
    // phi node and br node (control edge)
    this->parseCtrlEdge();
}

DFG::DFG(std::vector<llvm::BasicBlock*>& BBs){
    this->node_counter = 0;
    this->edge_counter = 0;
    this->cycleIsCalculated = false;
    this->nodes.clear();
    this->edges.clear();
    this->insts.clear();
    // extract instructions to this->insts
    for(BasicBlock* bb: BBs) {
        outs()<<"bb "<<bb->getName().str()<<":\n";
        for(BasicBlock::iterator inst=bb->begin(); inst!=bb->end(); inst++) {
            this->insts.push_back(&*inst);
            outs()<<"  "<<*inst<<"\n";
        }
    }
    // parse instruction
    for (Instruction* inst : this->insts) {
        this->parseInst(&*inst);
    }
    // phi node and br node (control edge)
    this->parseCtrlEdge();
}

void DFG::parseCtrlEdge() {
    Instruction* terminalInst = this->insts.back(); // TODO: This may lead to some bugs.
    assert(isTerminalInst(terminalInst));

    std::vector<Instruction*> PHIs;
    for (Instruction* inst: this->insts) {
        if (inst->getOpcode() == PHI)
            PHIs.push_back(inst);
    }

    if (isa<BranchInst>(terminalInst) && cast<BranchInst>(*terminalInst).isConditional()) {
        const BranchInst &BI(cast<BranchInst>(*terminalInst));
        for(int i=0; i<2; i++) {
            BasicBlock* bb = BI.getSuccessor(i);
            for (Instruction* inst : PHIs) {
                const PHINode *PN = dyn_cast<PHINode>(inst);
                for (unsigned op = 0, Eop = PN->getNumIncomingValues(); op < Eop; ++op) {
                    if (PN->getIncomingBlock(op) == bb) {
                        this->addEdge(this->getNode(terminalInst),
                                      this->getNode(inst),
                                      true);
                    }
                }
            }
        }
    } else {
        outs()<<"[ERROR!!!]: "<<*terminalInst<<"\n";
    }
}

int DFG::getOpCount() {
    int count = 0;
    for (DFGNode* node: this->nodes) {
        if (node->isOp())
            count++;
    }
    return count;
}

std::vector<DFGNode*> DFG::getOps() {
    std::vector<DFGNode*> ops;
    for (DFGNode* node : this->nodes) {
        if (node->isOp())
            ops.push_back(node);
    }
    return ops;
}

std::vector<DFGEdge> DFG::getEdges(bool visibleOnly) {
    std::vector<DFGEdge> edges;
    for (DFGEdge edge : this->edges) {
        if (!edge.isVisible() && visibleOnly)
            continue;
        edges.push_back(edge);
    }
    for (DFGEdge edge : this->ctrlEdges) {
        edges.push_back(edge);
    }
    return edges;
}

int DFG::getRecMII() {
    LOG_INFO<<"hello\n";
    this->calculateCycle(); // Assign an earliest and latest start cycle to each OPNode
    std::vector<DFGEdge> tedges; // Anti edges
    // Initialize
    for (DFGEdge edge : this->ctrlEdges) {
        tedges.push_back(edge);
    }
    for (DFGEdge edge : this->edges) {
        if (edge.isAnti) 
            tedges.push_back(edge);
    }
    // Core
    int maxII = -1;
    for (DFGEdge edge : tedges) {
        int II = edge.src->earliestCycle - edge.des->latestCycle + 1;
        maxII = II > maxII ? II : maxII;
    }
    return maxII;
}

/* A topology-sort-like algorithm that assigns each DFG node a start cycle. */
void DFG::calculateCycle() {
    if (this->cycleIsCalculated)
        return;
    // topology sort
    bool reversed[2] = {false, true};
    for (int i=0; i<2; i++) {
        bool r = reversed[i];
        std::vector<DFGNode*> visited;
        std::vector<DFGNode*> notVstd;
        std::vector<DFGEdge> tedges;
        // initialize
        for (DFGNode* node : this->nodes) {
            if (node->isOp()) 
                notVstd.push_back(node);
        }
        for (DFGEdge edge : this->edges) {
            if (edge.isVisible() && !edge.isAnti)
                tedges.push_back(edge);
        }
        int cycle;
        for (cycle=0; !notVstd.empty(); cycle++) {
            std::vector<DFGNode*> tnodes; // Store the nodes whose in-degree is 0.
            tnodes.clear();
            // Get nodes whose in-degree is zero.
            for (DFGNode* node : notVstd) {
                bool degree0 = true;
                for (DFGEdge edge : tedges) {
                    if (edge.des == node && r==false) {
                        degree0 = false;
                        break;
                    } else if (edge.src == node && r==true) {
                        degree0 = false;
                        break;
                    }
                }
                if (degree0)
                    tnodes.push_back(node);
            }
            // Delete from not visited and add to visited.
            for (DFGNode* node : tnodes) {
                auto idx = std::find(notVstd.begin(), notVstd.end(), node);
                notVstd.erase(idx);
                visited.push_back(node);
            }
            // Delete the edge pointed from/to this node. TODO: there might be bugs. [*] FIXED
            for (DFGNode* node : tnodes) {
                while (true) {
                    bool found_edge = false;
                    for (DFGEdge edge : tedges) {
                        if (edge.src==node && r==false) {
                            auto idx = std::find(tedges.begin(), tedges.end(), edge);
                            tedges.erase(idx);
                            found_edge = true;
                            break;
                        } else if (edge.des==node && r==true) {
                            auto idx = std::find(tedges.begin(), tedges.end(), edge);
                            tedges.erase(idx);
                            found_edge = true;
                            break;
                        }
                    }
                    if (!found_edge)
                        break;
                }
            }
            /* If not reversed, assign the cycle for it.
               Else, assign a temporary cycle (negative). */
            for (DFGNode* node : tnodes) {
                if (r == false)
                    node->earliestCycle = cycle;
                else
                    node->latestCycle = cycle;
            }
        }

        if (r==true) {
            for (DFGNode* node : visited) {
                node->latestCycle = cycle - node->latestCycle - 1;
            }
        }
    }
    this->cycleIsCalculated = true;
}

DFGNode* DFG::parseInst(Instruction* inst) {
    DFGNode* node = this->getNode(inst);
    if (node != NULL) {
        return node;
    }
    unsigned int opCode = inst->getOpcode();
    switch (opCode) {
        case PHI: 
            return this->parsePhi(inst);
        case GetElementPtr:
            return this->parseGetElementPtr(inst);
        case Load:
            return this->parseLoad(inst);
        case Store:
            return this->parseStore(inst);
        case Add:
            return this->parseInt32OP(inst);
        case FAdd:
            return this->parseFloat32OP(inst);
        case FMul:
            return this->parseFloat32OP(inst);
        case ICmp:
            return this->parseIcmp(inst);
        case Br:
            return this->parseBr(inst);
        case Call:
            return this->parseCall(inst);
        default:
            std::cout<<"not recognized instruction: "<<fg::red<<inst->getOpcodeName()<<fg::reset<<"\n";
            exit(1);
    }
}

/* Create node and connect the edge for 'phi' instruction. */
DFGNode* DFG::parsePhi(Instruction* inst) {
    DFGNode* curNode = new DFGNode(this->node_counter++, OPNode, inst, PHI);
    this->nodes.push_back(curNode);
    const PHINode *PN = dyn_cast<PHINode>(inst);
    for (unsigned op = 0, Eop = PN->getNumIncomingValues(); op < Eop; ++op) {
        Value* inComingVal = PN->getIncomingValue(op);
        if (auto *constant = dyn_cast<ConstantInt>(inComingVal)) {
            // outs() << "[DEBUG] phi constant: " << *constant << "\n";
            DFGNode* constNode = new DFGNode(this->node_counter++, ConstNode);
            constNode->intVal = constant->getZExtValue();
            constNode->bb = PN->getIncomingBlock(op);
            this->nodes.push_back(constNode);
            this->addEdge(constNode, curNode);
        } else if (auto *value = dyn_cast<Value>(inComingVal)) {
            DFGNode* valueNode = this->getNodeElseCreate((Instruction*)value);
            valueNode->bb = PN->getIncomingBlock(op);
            this->addEdge(valueNode, curNode);
        } else {
            outs()<<"[ERROR] unknown phi op"<<*inComingVal<<"\n";
        }
    }
    return curNode;
}

DFGNode* DFG::parseGetElementPtr(Instruction* inst) {
    DFGNode* curNode = new DFGNode(this->node_counter++, OPNode, inst, GetElementPtr);
    this->nodes.push_back(curNode);
    for (const auto& operand: inst->operands()) {
        if (auto *constant = dyn_cast<ConstantInt>(operand)) { // constant int
            // outs()<<"[DEBUG]: add constant int "<<constant->getZExtValue()<<" to the nodes\n";
            DFGNode* constNode = new DFGNode(this->node_counter++, ConstNode);
            constNode->intVal = constant->getZExtValue();
            constNode->dataType = INT;
            this->nodes.push_back(constNode);
            this->addEdge(constNode, curNode);
        } else if (auto *value = dyn_cast<Value>(operand)) { // value (from a instruction)
            // outs()<<"[DEBUG]: found value ID:"<<value->getValueID()<<"; "<<*value<<"; Value pointer:"<<value<<"\n";
            DFGNode* valueNode = this->getNodeElseCreate((Instruction*)value);
            this->addEdge(valueNode, curNode);
        } else {
            outs()<<"[ERROR!!!]: "<<*operand<<"\n";
        }
    }
    return curNode;
}

DFGNode* DFG::parseInt32OP(Instruction* inst) {
    DFGNode* curNode = new DFGNode(this->node_counter++, OPNode, inst, (OPCode)inst->getOpcode(), INT);
    this->nodes.push_back(curNode);
    for (const auto& operand: inst->operands()) {
        if (auto *constant = dyn_cast<ConstantInt>(operand)) { // constant int
            // outs()<<"[DEBUG]: add constant int "<<constant->getZExtValue()<<" to the nodes\n";
            DFGNode* constNode = new DFGNode(this->node_counter++, ConstNode);
            constNode->intVal = constant->getZExtValue();
            constNode->dataType = INT;
            this->nodes.push_back(constNode);
            this->addEdge(constNode, curNode);
        } else if (auto *value = dyn_cast<Value>(operand)) { // value (from a instruction)
            // outs()<<"[DEBUG]: found value ID:"<<value->getValueID()<<"; "<<*value<<"; Value pointer:"<<value<<"\n";
            DFGNode* valueNode = this->getNodeElseCreate((Instruction*)value);
            this->addEdge(valueNode, curNode);
        } else {
            outs()<<"[ERROR!!!]: "<<*operand<<"\n";
        }
    }
    return curNode;
}

DFGNode* DFG::parseFloat32OP(Instruction* inst) {
    DFGNode* curNode = new DFGNode(this->node_counter++, OPNode, inst, (OPCode)inst->getOpcode(), FLOAT);
    this->nodes.push_back(curNode);
    for (const auto& operand: inst->operands()) {
        if (auto *constant = dyn_cast<ConstantInt>(operand)) { // constant int
            // outs()<<"[DEBUG]: add constant int "<<constant->getZExtValue()<<" to the nodes\n";
            DFGNode* constNode = new DFGNode(this->node_counter++, ConstNode);
            constNode->intVal = constant->getZExtValue();
            constNode->dataType = INT;
            this->nodes.push_back(constNode);
            this->addEdge(constNode, curNode);
        } else if (auto *value = dyn_cast<Value>(operand)) { // value (from a instruction)
            // outs()<<"[DEBUG]: found value ID:"<<value->getValueID()<<"; "<<*value<<"; Value pointer:"<<value<<"\n";
            DFGNode* valueNode = this->getNodeElseCreate((Instruction*)value);
            this->addEdge(valueNode, curNode);
        } else {
            outs()<<"[ERROR!!!]: "<<*operand<<"\n";
        }
    }
    return curNode;
}

DFGNode* DFG::parseLoad(Instruction* inst) {
    DFGNode* curNode = new DFGNode(this->node_counter++, OPNode, inst, Load);
    this->nodes.push_back(curNode);
    for (const auto& operand: inst->operands()) {
        if (auto *constant = dyn_cast<ConstantInt>(operand)) { // constant int
            // outs()<<"[DEBUG]: add constant int "<<constant->getZExtValue()<<" to the nodes\n";
            DFGNode* constNode = new DFGNode(this->node_counter++, ConstNode);
            constNode->intVal = constant->getZExtValue();
            constNode->dataType = INT;
            this->nodes.push_back(constNode);
            this->addEdge(constNode, curNode);
        } else if (auto *value = dyn_cast<Value>(operand)) { // value (from a instruction)
            // outs()<<"[DEBUG]: found value ID:"<<value->getValueID()<<"; "<<*value<<"; Value pointer:"<<value<<"\n";
            DFGNode* valueNode = this->getNodeElseCreate((Instruction*)value);
            this->addEdge(valueNode, curNode);
        } else {
            outs()<<"[ERROR!!!]: "<<*operand<<"\n";
        }
    }
    return curNode;
}

DFGNode* DFG::parseStore(Instruction* inst) {
    DFGNode* curNode = new DFGNode(this->node_counter++, OPNode, inst, Store);
    this->nodes.push_back(curNode);
    for (const auto& operand: inst->operands()) {
        if (auto *constant = dyn_cast<ConstantInt>(operand)) { // constant int
            // outs()<<"[DEBUG]: add constant int "<<constant->getZExtValue()<<" to the nodes\n";
            DFGNode* constNode = new DFGNode(this->node_counter++, ConstNode);
            constNode->intVal = constant->getZExtValue();
            constNode->dataType = INT;
            this->nodes.push_back(constNode);
            this->addEdge(constNode, curNode);
        } else if (auto *value = dyn_cast<Value>(operand)) { // value (from a instruction)
            // outs()<<"[DEBUG]: found value ID:"<<value->getValueID()<<"; "<<*value<<"; Value pointer:"<<value<<"\n";
            DFGNode* valueNode = this->getNodeElseCreate((Instruction*)value);
            this->addEdge(valueNode, curNode);
        } else {
            outs()<<"[ERROR!!!]: "<<*operand<<"\n";
        }
    }
    return curNode;
}

DFGNode* DFG::parseIcmp(Instruction* inst) {
    DFGNode* curNode = new DFGNode(this->node_counter++, OPNode, inst, ICmp);
    this->nodes.push_back(curNode);
    for (const auto& operand: inst->operands()) {
        if (auto *constant = dyn_cast<ConstantInt>(operand)) { // constant int
            // outs()<<"[DEBUG]: add constant int "<<constant->getZExtValue()<<" to the nodes\n";
            DFGNode* constNode = new DFGNode(this->node_counter++, ConstNode);
            constNode->intVal = constant->getZExtValue();
            constNode->dataType = INT;
            this->nodes.push_back(constNode);
            this->addEdge(constNode, curNode);
        } else if (auto *value = dyn_cast<Value>(operand)) { // value (from a instruction)
            // outs()<<"[DEBUG]: found value ID:"<<value->getValueID()<<"; "<<*value<<"; Value pointer:"<<value<<"\n";
            DFGNode* valueNode = this->getNodeElseCreate((Instruction*)value);
            this->addEdge(valueNode, curNode);
        } else {
            outs()<<"[ERROR!!!]: "<<*operand<<"\n";
        }
    }
    return curNode;
}

DFGNode* DFG::parseBr(Instruction* inst) {
    DFGNode* curNode = new DFGNode(this->node_counter++, OPNode, inst, Br);
    this->nodes.push_back(curNode);
    for (const auto& operand: inst->operands()) {
        // outs() << "[DEBUG] br: " << *operand << "\n";
        if (auto *value = dyn_cast<Value>(operand)) { // value (from a instruction)
            // outs()<<"[DEBUG] found value ID:"<<value->getValueID()<<"; "<<*value<<"; Value pointer:"<<value<<"\n";
            DFGNode* valueNode = this->getNodeElseCreate((Instruction*)value);
            this->addEdge(valueNode, curNode);
        } else {
            // outs()<<"[ERROR!!!]: "<<*operand<<"\n";
        }
    }
    return curNode;
}

DFGNode* DFG::parseCall(Instruction* inst) {
    DFGNode* curNode = new DFGNode(this->node_counter++, OPNode, inst, Call);
    this->nodes.push_back(curNode);
    for (const auto& operand: inst->operands()) {
        // outs() << *operand<<"\n";
        if (auto *constant = dyn_cast<ConstantInt>(operand)) { // constant int
            // outs()<<"[DEBUG]: add constant int "<<constant->getZExtValue()<<" to the nodes\n";
            DFGNode* constNode = new DFGNode(this->node_counter++, ConstNode);
            constNode->intVal = constant->getZExtValue();
            constNode->dataType = INT;
            this->nodes.push_back(constNode);
            this->addEdge(constNode, curNode);
        } else if (auto *value = dyn_cast<Value>(operand)) { // value (from a instruction)
            // outs()<<"[DEBUG]: found value ID:"<<value->getValueID()<<"; "<<*value<<"; Value pointer:"<<value<<"\n";
            DFGNode* valueNode = this->getNodeElseCreate((Instruction*)value);
            this->addEdge(valueNode, curNode);
            this->addEdge(valueNode, curNode);
        } else {
            outs()<<"[ERROR!!!]: "<<*operand<<"\n";
        }
    }
    return curNode;
}

DFGEdge& DFG::addEdge(DFGNode* src, DFGNode* des, bool isCtrl) {
    DFGEdge edge(this->edge_counter++, src, des, 
                 this->isAnti(src, des), isCtrl);
    if (isCtrl) {
        this->ctrlEdges.push_back(edge);
        return this->edges.back();
    }
    else {
        this->edges.push_back(edge);
        return this->ctrlEdges.back();
    }
}

/* Find the node from the node list.
   return the node if found the node, else return NULL */
DFGNode* DFG::getNode(Instruction* inst) {
    for (DFGNode* node : this->nodes) {
        if (node->inst == inst) {
            return node;
        }
    }
    return NULL;
}

/* look up the nodes table for current node.
   if not exist, create one. */
DFGNode* DFG::getNodeElseCreate(Instruction* inst) {
    DFGNode* curNode = this->getNode(inst);
    if (curNode != NULL) {
        return curNode;
    } else if (this->shouldIgnore(inst)){
        curNode = new DFGNode(this->node_counter++, ValueNode);
        this->nodes.push_back(curNode);
        return curNode;
    } else {
        return this->parseInst(inst);
    }
}

/* If the instruction is inside the loop, return false.
   Else return true. */
bool DFG::shouldIgnore(Instruction* inst) {
    for (Instruction* inst_iter : this->insts) {
        if (inst_iter == inst) {
            return false;
        }
    }
    return true;
}

bool DFG::isAnti(DFGNode* srcNode, DFGNode* desNode) {
    Instruction* src = srcNode->inst;
    Instruction* des = desNode->inst;
    auto s = find(this->insts.begin(), this->insts.end(), src);
    auto d = find(this->insts.begin(), this->insts.end(), des);
    if (s == this->insts.end()) {
        return false;
    }
    assert(d != this->insts.end());
    return (d-s) > 0 ? false : true;
}

void DFG::generateDot(std::ofstream& f, bool show_more) {
    f << "digraph G {\n";
    for (DFGNode* node : this->nodes) {
        if (!show_more && !node->isOp()) {
            continue;
        }
        f<<"    ";
        node->serialize(f, true);
        f<<";\n";
    }
    f << "edge [color=black]\n"; // normal edges
    for (DFGEdge edge : this->edges) {
        if (!show_more && !edge.isVisible()) {
            continue;
        }
        if (edge.isAnti) {
            continue;
        }
        f<<"    ";
        edge.serialize(f);
        f<<";\n";
    }
    f << "edge [color=blue]\n"; // anti-edges
    for (DFGEdge edge : this->edges) {
        if (!show_more && !edge.isVisible()) {
            continue;
        }
        if (edge.isAnti) {
            f<<"    ";
            edge.serialize(f);
            f<<";\n";
        }
    }
    f << "edge [color=red]\n"; // control edge
    for (DFGEdge edge : this->ctrlEdges) {
        f<<"    ";
        edge.serialize(f);
        f<<";\n";
    }
    f << "}\n";
}

DFG::~DFG() {
    for (DFGNode* node : this->nodes) {
        delete node;
    }
    // for (DFGEdge* edge : this->edges) {
    //     delete edge;
    // }
    // for (DFGEdge* edge : this->ctrlEdges) {
    //     delete edge;
    // }
}

DFGNode& DFG::operator[](int ID) {
    for (DFGNode* node : this->nodes) {
        if (node->isOp() && node->ID == ID)
            return *node;
    }
    std::cout<<"could not find node with ID:"<<ID<<"\n";
    exit(1);
}

DFGNode& DFG::operator[](DFGNode node) const {
    for (DFGNode* n : this->nodes) {
        if (node == *n)
            return *n;
    }
    std::cout<<"could not find node";
    node.serialize(std::cout);
    exit(1);
}

// DFG& DFG::operator=(const DFG& other) {
//     this->node_counter = other.node_counter;
//     this->insts = other.insts;
//     this->cycleIsCalculated = other.cycleIsCalculated;
//     this->nodes = other.nodes;
//     this->edges = other.edges;
//     this->ctrlEdges = other.ctrlEdges;
//     return *this;
// }