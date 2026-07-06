#include "CGRA.hpp"
#include "toml++/toml.hpp"

#include <stdexcept>
#include <sstream>

using namespace std;
using namespace cgratool;

CGRA::CGRA(int row, int col, std::string arch) {
    try {
        if (arch.compare("2dmesh") == 0)
            init2dmesh(row, col);
        else 
            throw std::invalid_argument("not supported argument:"+arch);
    } catch (const exception& e) {
        LOG_ERROR<<e.what();
        exit(1);
    }
}

void CGRA::init2dmesh(int row, int col) {
    this->nodeID = 0;
    this->row = row;
    this->col = col;
    for (int i=0; i<row; i++) {
        for (int j=0; j<col; j++) {
            this->PEs.push_back(PE(this->nodeID++));
            /* default connections */
            if (i>0) {
                this->connections.push_back(pair<PE, PE>(this->PEs[(i-1)*col+j], this->PEs[i*col+j]));
                this->connections.push_back(pair<PE, PE>(this->PEs[i*col+j], this->PEs[(i-1)*col+j]));
            }
            if (j>0) {
                this->connections.push_back(pair<PE, PE>(this->PEs[i*col+j-1], this->PEs[i*col+j]));
                this->connections.push_back(pair<PE, PE>(this->PEs[i*col+j], this->PEs[i*col+j-1]));
            }
        }
    }
}

CGRA::CGRA(const CGRA& other) {
    *this = other;
}

CGRA& CGRA::operator=(const CGRA& other) {
    this->nodeID = other.nodeID;
    this->row = other.row;
    this->col = other.col;
    this->PEs = other.PEs;
    this->connections = other.connections;
    return *this;
}

CGRA::~CGRA() {
    ;
}

int CGRA::getPECount() {
    return this->row * this->col;
}

CGRA CGRA::loadFromToml(istream& f) {
    CGRA arch;
    toml::table tbl;

    tbl = toml::parse(f);

    /* peArr */
    toml::array& arr = *(tbl["topology"]["peList"].as_array());
    for (auto& t : arr) {
        auto& e = *t.as_table();
        PE pe;
        pe.ID = e["id"].as_integer()->get();
        pe.regNum = e["regNum"].as_integer()->get();
        pe.cfgNum = e["cfgNum"].as_integer()->get();
        pe.hasMemPort = e["hasMemPort"].as_boolean()->get();
        arch.PEs.push_back(pe);
        arch.adjList.push_back(std::vector<int>());
    }

    /* adjList */
    toml::array& conns = *(tbl["topology"]["connections"].as_array());
    for (auto& t : conns) {
        auto& e = *t.as_table();
        int id = e["node"].as_integer()->get();
        toml::array& neighbors = *(e["neighbors"].as_array());
        for (auto& t : neighbors) {
            arch.adjList[id].push_back(t.as_integer()->get());
        }
    } 

    /* daisyChains */
    toml::array& daisyChains = *(tbl["topology"]["daisyChains"].as_array());
    for (auto& e : daisyChains) {
        arch.daisyChains.push_back(std::vector<int>());
        toml::array& chain = *(e.as_array());
        for (auto& t : chain) {
            int id = t.as_integer()->get();
            arch.daisyChains.back().push_back(id);
        }
    } 

    return arch;
}

void CGRA::generateDot(ostream& f) {
    f << "digraph G {\n";
    for (PE pe : this->PEs) {
        f<<"    pe"<<pe.ID<<";\n";
    }
    for (pair<PE, PE> pp : this->connections) {
        f<<"    pe"<<pp.first.ID<<" -> pe"<<pp.second.ID<<";\n";
    }
    f << "}\n";
}

void CGRA::generateVerilog(ostream& f) {
    this->PEs[0].generateVerilog(f);
}