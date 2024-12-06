#include"Mapper.hpp"
#include"rang.hpp"
#include<cmath>
#include<ctime>
#include<random>

using namespace std;
using namespace cgratool;
using fg = rang::fg;       // console font fontgraound color
using style = rang::style; // console font style

/* ------------- Mapper ------------- */

Mapper::Mapper(DFG* dfg, CGRA cgra) {
    this->dfg = dfg;
    this->cgra = cgra;
}

Mapper::~Mapper() {;}

int Mapper::getResMII() {
    // TODO: A more accurate one descripted in "Iterative Modulo Scheduling".
    return ceil(float(this->dfg->getOpCount()) / this->cgra.getPECount());
}

int Mapper::getRecMII() {
    return this->dfg->getRecMII();
}

Mapping Mapper::map() {
    std::cout<<"This need to be implemented!\n";
    return Mapping();
}

/* ------------- RandomMapper ------------- */

RandomMapper::RandomMapper(DFG* dfg, CGRA cgra) : Mapper(dfg, cgra) {;}

RandomMapper::~RandomMapper() {
    ;
}

/* params: II: initial II
 * params: time_limit: seconds
 * return: II if succeed, else -1 */
Mapping RandomMapper::map(int II, int time_limit) {
    time_t start = time(NULL);
    Mapping mapping;
    for (; mapping.isNull(); II++) {
        time_t now = time(NULL);
        if (now - start > time_limit) {
            return Mapping();
        }
        mapping = this->mapII(II);
    }
    return mapping;
}

/* params: II: current II
 * params: time_limit: seconds
 * return: success or not */
Mapping RandomMapper::mapII(int II, int time_limit) {
    cout<<fg::cyan<<"[INFO] "<<fg::reset<<"mapping for II="<<II<<" starts.\n";
    MRRG mrrg(this->cgra, II);
    std::vector<Mapping> history;

    /* --- Debugging --- */
    string fname = "mrrg-";
    fname.append(std::to_string(II));
    fname.append(".dot");
    std::ofstream fmrrg(fname, std::ios::out);
    mrrg.generateDot(fmrrg);
    fmrrg.close();

    /* --- Mapping --- */
    time_t start = time(NULL);
    Mapping mapping(this->dfg, mrrg);
    while (true) {
        time_t now = time(NULL);
        if (now - start > time_limit)
            break;
        mapping.shuffle();
        if (mapping.isLegal())
            return mapping;
        continue;
    }
    return Mapping();
}
