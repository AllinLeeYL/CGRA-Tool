#include "CGRA.hpp"

using namespace std;
using namespace cgratool;

CGRA::CGRA() {}

CGRA::CGRA(int row, int col) {
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