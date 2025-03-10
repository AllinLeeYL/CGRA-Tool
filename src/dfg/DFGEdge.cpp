#include "DFG.hpp"

using namespace cgratool;
using namespace std;
using namespace llvm;

/* -------------------- DFGEdge -------------------- */

DFGEdge::DFGEdge(int ID, DFGNode* src, DFGNode* des, bool isAnti, bool isCtrl) {
    this->ID = ID;
    this->src = src;
    this->des = des;
    this->isAnti = isAnti;
    this->isCtrl = isCtrl;
}

bool DFGEdge::isVisible() {
    return this->src->isOp() && this->des->isOp();
}

void DFGEdge::serialize(ostream& f) const {
    this->src->serialize(f);
    f <<" -> ";
    this->des->serialize(f);
}

DFGEdge::DFGEdge() {;}

DFGEdge::DFGEdge(const DFGEdge& other) {
    *this = other;
}

DFGEdge::~DFGEdge() {;}

DFGEdge& DFGEdge::operator=(const DFGEdge& other) {
    this->ID = other.ID;
    this->src = other.src;
    this->des = other.des;
    this->latency = other.latency;
    this->isAnti = other.isAnti;
    this->isCtrl = other.isCtrl;
    return *this;
}

bool DFGEdge::operator==(const DFGEdge& other) const {
    return this->ID == other.ID
        && this->src == other.src
        && this->des == other.des
        && this->latency == other.latency
        && this->isAnti == other.isAnti
        && this->isCtrl == other.isCtrl;
}

bool DFGEdge::operator!=(const DFGEdge& other) const {
    return !(*this == other);
}

bool DFGEdge::operator<(const DFGEdge& other) const {
    return this->ID < other.ID;
}

/* -------------------- CtrlEdge -------------------- */

// CtrlEdge::CtrlEdge(DFGNode src, DFGNode des) : DFGEdge(src, des) {
//     ;
// }

// bool CtrlEdge::isVisible() {
//     return true;
// }

// CtrlEdge::~CtrlEdge() {
//     ;
// }

// CtrlEdge& CtrlEdge::operator=(const CtrlEdge& other) {
//     this->src = other.src;
//     this->des = other.des;
//     this->latency = other.latency;
//     this->isAnti = other.isAnti;
//     return *this;
// }