#include"PE.hpp"
#include<map>

using namespace std;
using namespace cgratool;

static vector<OPCode> FULL_INSTS{Br, Add, FAdd, FMul, Load, Store, GetElementPtr, ICmp, PHI, Call};
static std::map<OPCode, std::string> opExprs = {
    {Br, "//br"},
    {Add, "src1 + src2"},
    {FAdd, "fsrc1 + fsrc2"},
    {FMul, "fsrc1 * fsrc2"},
    {Load, "//load"},
    {Store, "//store"},
    {GetElementPtr, "//getelementptr"},
    {ICmp, "//icmp"},
    {PHI, "//phi"},
    {Call, "//call"},
};

/* ------------- FU ------------- */

FU::FU() {
    this->supportedInsts = FULL_INSTS;
}

FU::FU(const FU& other) {
    *this = other;
}

bool FU::supportOP(OPCode op) {
    return std::find(this->supportedInsts.begin(),
                     this->supportedInsts.end(),
                     op) != this->supportedInsts.end();
}

FU& FU::operator=(const FU& other) {
    this->supportedInsts = other.supportedInsts;
    return *this;
}

bool FU::operator==(const FU& other) const {
    return this->supportedInsts == other.supportedInsts;
}

/* ------------- PE ------------- */

PE::PE() {
    this->ID = -1;
    this->fu = FU();
}

PE::PE(int ID) {
    this->ID = ID;
    this->fu = FU();
}

PE::PE(const PE& other) {
    *this = other;
}

bool PE::supportOP(OPCode op) {
    return this->fu.supportOP(op);
}

PE& PE::operator=(const PE& other) {
    this->ID = other.ID;
    this->fu = other.fu;
    return *this;
}

bool PE::operator==(const PE& other) const {
    return this->ID == other.ID
        && this->fu == other.fu;
}

void PE::generateVerilog(ostream& f) {
    f << "module PE (\n";
    f << "  input [31:0] src1,\n";
    f << "  input [31:0] src2,\n";
    f << "  input [31:0] inst,\n";
    f << "  output reg [31:0] out1\n";
    f << ");\n";

    for (OPCode opcode : this->fu.supportedInsts) {
        f << "  " << opExprs[opcode] << "\n";
    }

    f << "endmodule\n";
}
