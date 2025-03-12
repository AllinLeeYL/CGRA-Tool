#include"Mapping.hpp"
#include"rang.hpp"
#include<cmath>
#include<ctime>
#include<random>
#include<sstream>

// using namespace llvm;
using namespace cgratool;
using fg = rang::fg;       // console font fontgraound color
using style = rang::style; // console font style

Mapping::Mapping() {
    this->dfg = NULL;
}

// Mapping::Mapping(vector<int> placement) {
//     this->placement = placement;
// }

Mapping::Mapping(DFG* dfg, MRRG mrrg) {
    this->dfg = dfg;
    this->mrrg = mrrg;
}

Mapping::Mapping(const Mapping& other) {
    *this = other;
}

Mapping& Mapping::operator=(const Mapping& other) {
    this->dfg = other.dfg;
    this->mrrg = other.mrrg;
    this->mapping.clear();
    for (auto & [d, m] : other.mapping) {
        DFGNode& dfgnode = (*this->dfg)[*d];
        MRRGNode* mrrgnode = this->mrrg[*m];
        this->mapping[&dfgnode] = mrrgnode;
    }
    this->route_map.clear();
    for (auto & [e, medges] : other.route_map) {
        this->route_map[e] = std::vector<MRRGEdge>();
        for (MRRGEdge me : medges) {
            this->route_map[e].push_back(this->mrrg.getEdge(me.src->ID, me.des->ID));
        }
    }
    return *this;
}

bool Mapping::operator==(const Mapping& other) {
    return this->mapping == other.mapping
        && this->route_map == other.route_map
        && this->dfg == other.dfg
        && this->mrrg.II == other.mrrg.II;
}

// TODO: modify implmentation
MRRGNode*& Mapping::operator[](DFGNode* dfgnode) {
    return this->mapping[dfgnode];
}

const DFGNode* Mapping::operator[](MRRGNode* mrrgnode) {
    for (const auto & [d, ms] : this->route_map) {
        for (MRRGEdge m : ms) 
            if (m.des == mrrgnode)
                return d.des;
    }
    return NULL;
}

std::vector<MRRGEdge>& Mapping::operator[](DFGEdge edge) {
    return this->route_map[edge];
}

void Mapping::clear() {
    this->mapping.clear();
    this->mrrg.reset();
}

void Mapping::generateDot(std::ostream& f) {
    int maxT = this->mrrg.II;
    // for (auto & [d, m] : this->mapping) {
    //     maxT = std::max(m->T, maxT);
    // }
    f << "digraph G {\n";
    /* --- nodes --- */
    for (int t=0; t<maxT; t++) {
        f<<"    subgraph cluster_T"<<t<<" {\n";
        f<<"        color=black;\n";
        f<<"        label=\"T"<<t<<"\";\n";
        for (MRRGNode* node : this->mrrg.nodes) {
            if (node->T != t)
                continue;
            const DFGNode* dfgnode = (*this)[node];
            std::ostringstream label;
            if (dfgnode != NULL) {
                dfgnode->serialize(label);
            }
            f << "        ";
            bool emphasise = label.str() != "";
            node->serialize(f, true, label.str(), emphasise);
            f << ";\n";
        }
        f<<"    }\n";
    }
    /* --- used edges --- */
    std::set<MRRGEdge> usedEdges;
    f << "    edge [color=\"blue\" style=\"bold\"];\n";
    for (const auto& [de, mes] : this->route_map) {
        if (de.src->nodeType == NULLNode)
            continue;
        for (MRRGEdge me : mes) {
            f << "    ";
            me.src->serialize(f);
            f << " -> ";
            me.des->serialize(f);
            f << ";\n";
            usedEdges.insert(me);
        }
    }
    
    /* --- ununsed edges --- */
    f << "    edge [color=\"black\" style=\"dotted\"];\n";
    for (MRRGEdge* edge : this->mrrg.edges) {
        if (edge->des->T >= maxT || usedEdges.count(*edge)==1 || edge->src->T == -1)
            continue;
        f << "    ";
        edge->src->serialize(f);
        f << " -> ";
        edge->des->serialize(f);
        f << ";\n";
    }
    f << "}\n";
}

void Mapping::serialize(std::ostream& f) {
    for (const auto& [dfgnode, mrrgnode] : this->mapping) {
        dfgnode->serialize(f);
        f<<"->";
        mrrgnode->serialize(f);
        f<<"\n";
    }
}

std::map<DFGNode*, MRRGNode*>::iterator Mapping::begin() {
    return this->mapping.begin();
}

std::map<DFGNode*, MRRGNode*>::iterator Mapping::end() {
    return this->mapping.end();
}

Mapping& Mapping::shuffle(int upper_bound) {
    this->mapping.clear();
    upper_bound = upper_bound == -1 ? this->mrrg.II * 2 : upper_bound;
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(this->mrrg.II, upper_bound); // distribution in range [1, 6]
    // TODO: optimize the shuffle algorithm below.
    /* {OP0.idx,   -1, OP1.idx,    -1  }
       {PE0t0,  PE1t0,   PE0t1, PE1t1  } */
    std::vector<int> placement;
    std::vector<MRRGNode*> fus = this->mrrg.getFUs(dist6(rng));
    std::vector<DFGNode*> ops = this->dfg->getOps();
    assert(ops.size() <= fus.size());
    for (int i=0; i<fus.size(); i++)
        placement.push_back(-1);
    for (int i=0; i<ops.size(); i++)
        placement[i] = i;

    unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(placement.begin(), placement.end(), std::default_random_engine(seed));
    
    for (int i=0; i<fus.size(); i++) {
        if (placement[i] != -1)
            this->mapping[ops[placement[i]]] = fus[i];
    }
    return *this;
}

bool Mapping::isLegal() {
    using RoutePair = std::pair<DFGEdge, std::vector<MRRGRoute>>;

    std::vector<RoutePair> path_pairs;
    // 1. Check the whether the FU support this op.
    for (auto const& [dfgnode, mrrgnode] : this->mapping) {
        if (!mrrgnode->supportOP(dfgnode->opCode))
            return false;
    }
    // 2. Check whether the FUs used conflict
    std::vector<MRRGNode> occupied;
    for (auto const& [dfgnode, mrrgnode] : this->mapping) {
        MRRGNode node_modded = *mrrgnode;
        node_modded.T = node_modded.T % this->mrrg.II;
        if (std::find(occupied.begin(), occupied.end(), node_modded) != occupied.end())
            return false;
        occupied.push_back(node_modded);
    }
    // 3. Check whether there is a path from src to des
    for (DFGEdge edge : this->dfg->getEdges()) {
        MRRGNode* src = this->mapping[edge.src];
        MRRGNode* des = this->mapping[edge.des];
        if (edge.isAnti) {
            MRRGNode next_des = *des;
            next_des.T += this->mrrg.II;
            des = this->mrrg.getNodeByDetial(des->type, des->pe, des->T+this->mrrg.II);
            assert(des != *this->mrrg.nodes.end());
        } 
        std::vector<MRRGRoute> paths = this->mrrg.findPath(src, des);
        if (paths.size() == 0)
            return false;
        path_pairs.push_back(RoutePair(edge, paths));
    }
    // 4. Sort the pairs in order of route numbers
    std::sort(path_pairs.begin(), path_pairs.end(), [ ] (const RoutePair& lhs, const RoutePair& rhs) {
        return lhs.second.size() < rhs.second.size();
    });
    // 5. Route and try occupying paths to check if there is a route
    this->route_map.clear();
    for (const auto & [edge, paths] : path_pairs) {
        bool one_of_them_succeed = false;
        for (MRRGRoute path : paths) {
            if (this->mrrg.tryOccupyPath(path)) {
                one_of_them_succeed = true;
                // this->route_map[edge] = path;
                break;
            }
        }
        if (!one_of_them_succeed) {
            // std::cout<<"None of them succeeded.\n";
            this->route_map.clear();
            return false;
        }
    }
    // 6. Check whether these routes fulfill the requirement of II
    // for (auto & [edge, route] : this->route_map) {
    //     edge.serialize(std::cout);
    //     std::cout<<":";
    //     for (MRRGNode* cur : route) {
    //         cur->serialize(std::cout);
    //         std::cout<<"->";
    //     }
    //     std::cout<<"\n";
    // }
    
    return true;
}

bool Mapping::isNull() {
    return this->route_map.empty();
}
