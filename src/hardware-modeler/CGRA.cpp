#include "CGRA.hpp"
#include "toml++/toml.hpp"

using namespace std;
using namespace cgratool;

CGRA::CGRA() {}

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

bool CGRA::loadFromToml(istream& f) {
    toml::table tbl;
    try {
        tbl = toml::parse(f);
        std::cout << tbl << "\n";
    } catch (const toml::parse_error& err) {
        std::cerr << "Parsing failed:\n" << err << "\n";
        return false;
    }
    return true;
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