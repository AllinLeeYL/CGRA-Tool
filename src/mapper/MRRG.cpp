#include "MRRG.hpp"
#include "rang.hpp"
#include <assert.h>
#include <sstream>

using namespace cgratool;
using fg = rang::fg;
static int MAXT = 8192;
/* ------------- MRRGNode ------------- */

MRRGNode::MRRGNode() {;}

MRRGNode::MRRGNode(const MRRGNode& other) {
    *this = other;
}

MRRGNode::MRRGNode(int ID, MRRGNodeType type, PE pe, int T, int mux_allow) {
    this->ID = ID;
    this->type = type;
    this->pe = pe;
    this->T = T;
    this->mux_allow = mux_allow;
    this->occupied = false;
}

bool MRRGNode::supportOP(OPCode op) {
    return this->pe.supportOP(op);
}

std::string MRRGNode::getLabel() {
    return std::to_string(this->T)+"_"+std::to_string(this->pe.ID);
}

MRRGNode& MRRGNode::operator=(const MRRGNode& other) {
    this->ID = other.ID;
    this->type = other.type;
    this->pe = other.pe;
    this->T = other.T;
    this->mux_allow = other.mux_allow;
    this->occupied = other.occupied;
    return *this;
}

bool MRRGNode::operator==(const MRRGNode& other) const {
    return this->ID == other.ID
        && this->type == other.type
        && this->pe == other.pe
        && this->T == other.T
        && this->mux_allow == other.mux_allow
        && this->occupied == other.occupied;
}

bool MRRGNode::serialize(std::ostream& f, bool display_attrs, std::string label, bool emphasise) const {
    // llvm::raw_ostream name;
    std::ostringstream name;
    if (this->type == FUNode) {
        name<<"FU"<<this->pe.ID<<"_"<<this->ID;
    } else if (this->type == RegNode) {
        name<<"Reg"<<this->pe.ID<<"_"<<this->ID;
    } else if (this->type == MuxNode) {
        name<<"Mux"<<this->pe.ID<<"_"<<this->ID;
    } else if (this->type == ROOTNode) {
        ;
    }
    else {
        std::cout<<fg::red<<"wrong <MRRGNode> Type!"<<this->type<<"\n"<<fg::reset;
        return false;
    }
    f << name.str();
    if (display_attrs) {
        f<<" [z="<<this->T;
        if (label != "")
            f<<" label=\""<<name.str()<<"\\n"<<label<<"\"";
        if (emphasise)
            f<<" color=cadetblue style=bold";
        f<<"]";
    }
    return true;
}

/* ------------- MRRGEdge ------------- */

MRRGEdge::MRRGEdge(int ID, MRRGNode* src, MRRGNode* des) {
    this->ID = ID;
    this->src = src;
    this->des = des;
    this->occupied = false;
}

MRRGEdge& MRRGEdge::operator=(const MRRGEdge& other) {
    this->ID = other.ID;
    this->src = other.src;
    this->des = other.des;
    this->occupied = other.occupied;
    return *this;
}

bool MRRGEdge::operator==(const MRRGEdge& other) const {
    return this->ID == other.ID
        && this->src == other.src
        && this->des == other.des
        && this->occupied == other.occupied;
}

bool MRRGEdge::operator!=(const MRRGEdge& other) const {
    return !(*this == other);
}

bool MRRGEdge::operator<(const MRRGEdge& other) const {
    return this->ID < other.ID;
}

/* ------------- MRRG ------------- */

MRRG::MRRG() {
    this->node_counter = 0;
    this->edge_counter = 0;
    this->nodes.clear();
    this->edges.clear();
}

MRRG::MRRG(const MRRG& other) {
    *this = other;
}

MRRG::MRRG(CGRA cgra, int II) {
    this->II = II;
    this->TT = II;
    this->cgra = cgra;
    this->node_counter = 0;
    this->edge_counter = 0;
    this->edges.clear();
    this->nodes.clear();
    this->extendGraphUntil(TT);
}

void MRRG::reset() {
    for (MRRGNode* node : this->nodes)
        node->occupied = false;
    for (MRRGEdge* edge : this->edges)
        edge->occupied = false;
}

std::vector<MRRGNode*> MRRG::getFUs(int T) {
    if (T == -1)
        T = this->II;
    return this->getFUsOfT(0, T);
}

std::vector<MRRGNode*> MRRG::getFUsOfT(int T) {
    return this->getFUsOfT(T, T+1);
}

std::vector<MRRGNode*> MRRG::getFUsOfT(int T0, int T1) {
    assert(T0 < T1);
    if (T1 > this->TT+1)
        this->extendGraphUntil(T1);
    std::vector<MRRGNode*> FUs;
    for (MRRGNode* node : this->nodes) {
        if (node->type == FUNode &&
            T0 <= node->T        &&
            node->T < T1)
            FUs.push_back(node);
    }
    return FUs;
}

MRRGEdge* MRRG::getEdge(MRRGNode* src, MRRGNode* des) const {
    for (MRRGEdge* edge : this->edges) {
        if (edge->src == src && edge->des == des)
            return edge;
    }
    return NULL;
}

MRRGEdge& MRRG::getEdge(int srcID, int desID) const {
    for (MRRGEdge* edge : this->edges) {
        if (edge->src->ID == srcID && edge->des->ID == desID)
            return *edge;
    }
    LOG_WARNING<<"Could not find edge: "<<srcID<<"->"<<desID<<" in MRRG\n";
    exit(1);
}

std::vector<MRRGEdge*> MRRG::getEdgesOfT(int T0, int T1) {
    std::vector<MRRGEdge*> vec;
    for (MRRGEdge* e : this->edges) {
        if (e->src->T == T0 && e->des->T == T1) {
            vec.push_back(e);
        }
    }
    return vec;
}

std::vector<MRRGRoute> MRRG::planRoute(MRRGNode* src, MRRGNode* des) {
    std::vector<MRRGRoute> routes;
    // if src -> des unreachable in current cycle (0...II), des->T += II
    for (routes=this->findPath(src, des); routes.size()==0; routes=this->findPath(src, des)) {
        MRRGNode node = *des;
        node.T += 1;
        des = (*this)[node];
    }
    return routes;
}

std::vector<MRRGRoute> MRRG::findPath(MRRGNode* src, MRRGNode* des, bool occupy) {
    std::vector<MRRGRoute> paths;
    if (src->T >= des->T)
        return paths;
    // depth-first-search-like algorithm (stack based)
    MRRGRoute curPath;
    std::vector<MRRGNode*> notVstd;
    notVstd.push_back(src);
    while(!notVstd.empty()) {
        // maintain notVstd
        MRRGNode* cur = notVstd.back();
        notVstd.pop_back();
        // maintain curPath
        while (!curPath.empty() && !this->getEdge(curPath.back(), cur))
            curPath.pop_back();
        curPath.push_back(cur);
        // main operation
        if (cur->T == des->T && cur->type == FUNode) { // leaf node operation:
            if (cur == des)
                paths.push_back(curPath);
        } else if (cur->T <= des->T) { // push children into stack
            std::vector<MRRGNode*> children = this->childrenOf(cur);
            notVstd.insert(notVstd.end(), children.begin(), children.end());
        } else {
            std::cout<<fg::red<<"[ERROR] "<<fg::reset<<"There is no des node like: ";
            cur->serialize(std::cout);
            std::cout<<"\n";
            return paths;
        }
    }
    return paths;
}

bool MRRG::tryOccupyPath(MRRGRoute path) {
    return true;
    MRRGNode* lastNode = NULL;
    for (MRRGNode* node : path) {
        if (node->occupied)
            return false;
        if (node->type == MuxNode) {
            int inEdge = 0;
            for (MRRGEdge* edge : this->edges)
                if (edge->occupied)
                    inEdge += 1;
            assert(inEdge < node->mux_allow);
            if (inEdge+1 == node->mux_allow)
                node->occupied = true;
        } else
            node->occupied = true;
        if (lastNode) {
            this->getEdge(lastNode, node)->occupied = true;
        }
        lastNode = node;
    }
    return true;
}

std::vector<MRRGNode*> MRRG::childrenOf(MRRGNode* node) {
    std::vector<MRRGNode*> children;
    for (MRRGEdge* edge : this->edges) {
        if (edge->src == node) {
            children.push_back(edge->des);
        }
    }
    return children;
}

void MRRG::extendGraphUntil(int T) {
    if (T <= 0) 
        LOG_WARNING<<"extendGraphUntil(int T): T must be > 0!\n";
    if (T > MAXT) {
        std::cout<<fg::red<<"[ERROR] "<<fg::reset
                 <<"T="<<T<<" exceeds the max supported value "
                 <<MAXT<<"\n";
        exit(1);
    }
    int startT = 0;
    for (MRRGNode* node : this->nodes) 
        startT = std::max(startT, node->T+1);
    MRRGNode* rootnode;
    if (startT == 0) { // create root node
        rootnode = new MRRGNode(-1, ROOTNode, PE(), -1);
        this->nodes.push_back(rootnode);
    }
    /* --- nodes and connections within the same T --- */
    for (int t=startT; t<=T; t++) { // every cycle t
        for (PE pe : this->cgra.PEs) {
            MRRGNode* node = new MRRGNode(this->node_counter++, FUNode, pe, t);
            this->nodes.push_back(node);
            /* --- only when t != 0 --- */
            this->edges.push_back(new MRRGEdge(
                    this->edge_counter++,
                    rootnode,
                    node
                ));
            if (t != 0) 
                this->edges.push_back(new MRRGEdge(
                    this->edge_counter++,
                    this->getNodeByDetial(FUNode, pe, t-1), // the last fu
                    node
                ));
        }
    }
    /* --- connections cross different Ts --- */
    for (auto & [srcPE, desPE] : this->cgra.connections) {
        for (int t=startT; t<=T; t++) {
            if (t == 0)
                continue;
            this->edges.push_back(new MRRGEdge(
                this->edge_counter++,
                this->getNodeByDetial(FUNode, srcPE, t-1),
                this->getNodeByDetial(FUNode, desPE, t)
            ));
        }
    }
    /* --- connections from node(T-1) -> node(0) --- */
    for (PE& pe : this->cgra.PEs) {
        this->edges.push_back(new MRRGEdge(
            this->edge_counter++,
            this->getNodeByDetial(FUNode, pe, T-1),
            this->getNodeByDetial(FUNode, pe, 0)
        ));
    }
    for (auto & [srcPE, desPE] : this->cgra.connections) {
        this->edges.push_back(new MRRGEdge(
            this->edge_counter++,
            this->getNodeByDetial(FUNode, srcPE, T-1),
            this->getNodeByDetial(FUNode, desPE, 0)
        ));
    }
    this->TT = T;
}

MRRGNode* MRRG::operator[](MRRGNode node) {
    if (node.T > this->TT) {
        this->extendGraphUntil(node.T);
    }
    for (MRRGNode* n : this->nodes) {
        if (*n == node)
            return n;
    }
    LOG_WARNING<<"no such node: ";
    node.serialize(std::cout);
    std::cout<<"\n";
    return NULL;
}

MRRGNode* MRRG::operator[](int ID) {
    for (MRRGNode* n : this->nodes) {
        if (n->ID == ID)
            return n;
    }
    LOG_WARNING<<"no such node with ID: "<<ID<<"\n";
    return NULL;
}

MRRGNode* MRRG::getNodeByDetial(MRRGNodeType type, PE pe, int t) {
    if (t > this->TT) {
        this->extendGraphUntil(t);
    }
    for (MRRGNode* n : this->nodes) {
        if (n->type == type && n->pe == pe && n->T == t)
            return n;
    }
    LOG_WARNING<<"no such node: ";
    MRRGNode tmpNode(-1, type, pe, t);
    tmpNode.serialize(std::cout);
    std::cout<<"\n";
    return NULL;
}

void MRRG::generateDot(std::ostream& f) {
    f << "digraph G {\n";
    /* --- nodes --- */
    for (int t=0; t<this->II; t++) {
        f<<"    subgraph cluster_T"<<t<<" {\n";
        f<<"        color=black;\n";
        f<<"        label=\"T"<<t<<"\";\n";
        for (MRRGNode* node : this->nodes) {
            if (node->T != t)
                continue;
            f << "        ";
            node->serialize(f, true);
            f << ";\n";
        }
        f<<"    }\n";
    }
    /* --- edges --- */
    // f << "    edge [color=\"black\" style=\"dotted\"];\n";
    for (MRRGEdge* edge : this->edges) {
        // if (edge->src->T >= this->II || edge->des->T >= this->II)
        if (edge->des->T >= this->II 
         || edge->src->T == -1
         || edge->src->T == this->II-1)
            continue;
        f << "    ";
        edge->src->serialize(f);
        f << " -> ";
        edge->des->serialize(f);
        f << ";\n";
    }
    f << "}\n";
}

MRRG::~MRRG() {
    // std::cout<<"MRRG destruction happened!\n";
    for (MRRGNode* node : this->nodes) {
        delete node;
    }
    for (MRRGEdge* edge : this->edges) {
        delete edge;
    }
}

MRRG& MRRG::operator=(const MRRG& other) {
    this->cgra = other.cgra;
    this->II = other.II;
    this->TT = other.TT;
    for (MRRGNode* node : this->nodes) {
        delete node;
    }
    for (MRRGEdge* edge : this->edges) {
        delete edge;
    }
    this->nodes.clear();
    this->edges.clear();
    for (MRRGNode* node : other.nodes) {
        MRRGNode* newNode = new MRRGNode();
        *newNode = *node;
        this->nodes.push_back(newNode);
    }
    for (MRRGEdge* edge : other.edges) {
        MRRGEdge* e = new MRRGEdge(*edge);
        e->src = (*this)[*edge->src];
        e->des = (*this)[*edge->des];
        this->edges.push_back(e);
    }
    return *this;
}

// bool MRRG::operator==(const MRRG& other) {
//     if (this->II != other.II)
//         return false;
//     for (int i=0; i<other.nodes.size(); i++) {
//         if (*(this->nodes[i]) != *(other.nodes[i]))
//             return false;
//     }
//     return true;
// }
