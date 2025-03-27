#include <chrono>
#include "Common.hpp"
#include "Mapper.hpp"
#include "ortools/base/logging.h"
#include "ortools/linear_solver/linear_solver.h"

using namespace cgratool;
using namespace operations_research;

/* helper functions */

inline std::vector<std::string> split(std::string s, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    tokens.push_back(s);
    return tokens;
}

/* MPVariable name is "DFGSrcID-DFGDesID:MRRGSrcID-MRRGDesID" for abbreviation "ds-dd:ms:md"
 * e.g. "S-0:S-0", "0-2:1-3"*/
inline std::vector<MPVariable*> getMPVarOfWhich(std::vector<MPVariable*> vars, 
                                                std::string ds="", 
                                                std::string dd="", 
                                                std::string ms="", 
                                                std::string md="") {
    std::vector<MPVariable*> vec;
    for (MPVariable* v : vars) {
        std::vector<std::string> ss = split(v->name(), ":");
        auto [ld, lm] = std::tuple<std::string, std::string>(ss[0], ss[1]);
        std::vector<std::string> s0 = split(ld, ">");
        std::vector<std::string> s1 = split(lm, ">");
        auto [lds, ldd] = std::tuple<std::string, std::string>(s0[0], s0[1]);
        auto [lms, lmd] = std::tuple<std::string, std::string>(s1[0], s1[1]);
        assert(ss.size() == 2);
        assert(s0.size() == 2);
        assert(s1.size() == 2);
        if (ds != "" && lds != ds)
            continue;
        if (dd != "" && ldd != dd)
            continue;
        if (ms != "" && lms != ms)
            continue;
        if (md != "" && lmd != md)
            continue;
        vec.push_back(v);
    }
    return vec;
}

inline std::tuple<int, int, int, int> getDFGAndMRRGFromMPVar(MPVariable* v) {
    std::vector<std::string> ss = split(v->name(), ":");
    auto [ld, lm] = std::tuple<std::string, std::string>(ss[0], ss[1]);
    std::vector<std::string> s0 = split(ld, ">");
    std::vector<std::string> s1 = split(lm, ">");
    auto [lds, ldd] = std::tuple<std::string, std::string>(s0[0], s0[1]);
    auto [lms, lmd] = std::tuple<std::string, std::string>(s1[0], s1[1]);
    assert(ss.size() == 2);
    assert(s0.size() == 2);
    assert(s1.size() == 2);
    // return std::tuple<int, int>(std::stoi(ldd), std::stoi(lmd));
    // lds = lds == "S" ? "-1" : lds;
    // lms = lms == "S" ? "-1" : lms;
    return std::tuple<int, int, int, int>(
                      std::stoi(lds), std::stoi(ldd), 
                      std::stoi(lms), std::stoi(lmd));
}

/* class functions */

ILPMapper::ILPMapper(DFG* dfg, CGRA cgra, std::string solver) : Mapper(dfg, cgra) { 
    this->solverName = solver;
}

Mapping ILPMapper::map(int II, int time_limit) {
    Mapping mapping;
    time_t start = time(NULL);
    for (int ii = II; mapping.isNull(); ii++) {
        time_t now = time(NULL);
        if (now - start > time_limit)
            break;
        mapping = this->mapII(ii);
        // break; //! delete this after debugging
    }
    return mapping;
}

Mapping ILPMapper::mapII(int II, int time_limit) {
    Mapping mapping(this->dfg, MRRG(this->cgra, II));
    MRRG &mrrg = mapping.mrrg;
    /* --- Debugging --- */
    // std::string fname = "mrrg-";
    // fname.append(std::to_string(II));
    // fname.append(".dot");
    // std::ofstream fmrrg(fname, std::ios::out);
    // mrrg.generateDot(fmrrg);
    // fmrrg.close();

    auto start = std::chrono::high_resolution_clock::now();

    std::unique_ptr<MPSolver> solver(MPSolver::CreateSolver(this->solverName));
    if (!solver) {
        LOG_WARNING<<"Could not create solver "<<this->solverName<<"\n";
    }

    { /* VARIABLE: mappable place for each node */
        // std::vector<DFGNode*> nodes = dfg->getOpsOfCycle(0, II);
        std::vector<DFGNode*> nodes = dfg->getOps();
        for (DFGNode* node : nodes) {
            std::vector<DFGEdge> ies = dfg->getEdgesTo(node);
            std::vector<DFGEdge> anties = dfg->getAntiEdgesTo(node);
            if (ies.size() == 0) 
                ies.push_back(dfg->getEdge(-1, node->ID));
            for (DFGEdge ie : ies) { // edges
                for (int i=ie.src->cycle; i!=node->cycle; i++) {
                    if (ie.src->ID == -1) // special cituation for root node
                        i = node->cycle - 1;
                    std::vector<MPVariable*> vec;
                    std::vector<MRRGEdge*> mrrgEdges = mrrg.getEdgesOfT(
                        ie.src->ID == -1 ? -1 : i%II, 
                        (i+1)%II);
                    for (MRRGEdge* me : mrrgEdges) {
                        std::string dfgName = std::to_string(ie.src->ID)+">"+std::to_string(ie.des->ID);
                        std::string mrrgName = std::to_string(me->src->ID)+">"+std::to_string(me->des->ID);
                        MPVariable* const v = solver->MakeBoolVar(dfgName+":"+mrrgName);
                        vec.push_back(v);
                    }
                    /* CONSTRAINT: Being mapped and only mapped to one place*/
                    MPConstraint* const c = solver->MakeRowConstraint(1, 1);
                    for (MPVariable* v : vec) {
                        c->SetCoefficient(v, 1);
                    }
                }
            }
            for (DFGEdge e : anties) { // anti edges
                int descycle = node->cycle + II;
                for (int i=e.src->cycle; i!=descycle; i++) {
                    std::vector<MPVariable*> vec;
                    std::vector<MRRGEdge*> mrrgEdges = mrrg.getEdgesOfT(
                        e.src->ID == -1 ? -1 : i%II, 
                        (i+1)%II);
                    for (MRRGEdge* me : mrrgEdges) {
                        std::string dfgName = std::to_string(e.src->ID)+">"+std::to_string(e.des->ID);
                        std::string mrrgName = std::to_string(me->src->ID)+">"+std::to_string(me->des->ID);
                        MPVariable* const v = solver->MakeBoolVar(dfgName+":"+mrrgName);
                        vec.push_back(v);
                    }
                    /* CONSTRAINT: Being mapped and only mapped to one place*/
                    MPConstraint* const c = solver->MakeRowConstraint(1, 1);
                    for (MPVariable* v : vec) {
                        c->SetCoefficient(v, 1);
                    }
                }
            }
        }
    }

    /* Get unique edge variables for ILP from DFG and MRRG */
    std::vector<std::string> dECls; // Class names of DFG edges
    std::vector<std::string> mECls; // Class names of MRRG edges
    {
        for (MPVariable* const var : solver->variables()) {
            std::vector<std::string> ss = split(var->name(), ":");
            assert(ss.size() == 2);
            auto [d, m] = std::tuple<std::string, std::string>(ss[0], ss[1]);
            dECls.push_back(d);
            mECls.push_back(m);
        }
        // remove duplicates
        std::set<std::string> s1(dECls.begin(), dECls.end() );
        dECls.assign(s1.begin(), s1.end());
        std::set<std::string> s2(mECls.begin(), mECls.end() );
        mECls.assign(s2.begin(), s2.end());

        // LOG_INFO<<"(ILP) DFG Variable classses:\n";
        // for (auto s : dECls)
        //     LOG_IDT<<s;
        // std::cout<<"\n";
        // LOG_INFO<<"(ILP) MRRG Variable classses:\n";
        // for (auto s : mECls)
        //     LOG_IDT<<s;
        // std::cout<<"\n";
    }

    /* CONSTRAINT: One MRRG Node can only be mapped to one DFGNode. a-b and c-d to different node */
    // LOG_INFO<<"(ILP) Exclusive Constraints:\n";
    std::set<std::string> mdCls;
    for (std::string cls : mECls) {
        std::vector<std::string> ss = split(cls, ">");
        assert(ss.size() == 2);
        mdCls.insert(ss[1]);
    }
    for (std::string cls : mdCls) {
        std::vector<MPVariable*> vec = getMPVarOfWhich(solver->variables(), "", "", "", cls);
        std::map<int, std::map<int, std::vector<MPVariable*>>> ddClsMap; // {'dd': {'ds': <MPVar>}}
        for (MPVariable* v : vec) {
            auto [ds, dd, ms, md] = getDFGAndMRRGFromMPVar(v);
            if (ddClsMap.count(dd) == 0) {
                ddClsMap[dd] = std::map<int, std::vector<MPVariable*>>();
                ddClsMap[dd][ds] = std::vector<MPVariable*>{v};
            }
            else {
                if (ddClsMap[dd].count(ds) == 0)
                    ddClsMap[dd][ds] = std::vector<MPVariable*>{v};
                else
                    ddClsMap[dd][ds].push_back(v);
            }
        }
        /* CONSTRAINT: One MRRG Node can only be mapped to one DFGNode. a-b and c-d to different node */
        // LOG_IDT<<"0 <= ";
        MPConstraint* const c = solver->MakeRowConstraint(0, 1);
        for (const auto [key, val] : ddClsMap) {
            for (MPVariable* v : val.begin()->second) {
                c->SetCoefficient(v, 1); // LOG_IDT<<v->name()<<" + ";
            } 
        }
        // LOG_IDT<<" <= 1\n";
        /* CONSTRAINT: Consistency. DFG b->c and DFG a->c Mapped to the same place on MRRG */
        for (const auto [key, val] : ddClsMap) {
            auto iter=val.begin();
            iter++;
            for (; iter!=val.end(); iter++) {
                MPConstraint* const c = solver->MakeRowConstraint(0, 0);
                for (const auto v : iter->second)
                    c->SetCoefficient(v, 1);
                iter--;
                for (const auto v : iter->second)
                    c->SetCoefficient(v, -1);
                iter++;
            }
        }
    }

    /* CONSTRAINT: Continuation */
    // LOG_INFO<<"(ILP) Continuation Constraints:\n";
    for (DFGNode* d : dfg->getOps()) {
        for (DFGEdge oe : dfg->getEdgesFrom(d, true, false)) {
            // if (oe.isAnti)
            //     continue;
            for (MRRGNode* m : mrrg.getFUsOfT(d->cycle%II)) { // a(0) -> c(1)
                std::vector<MPVariable*> vec1 = getMPVarOfWhich(solver->variables(), "", std::to_string(d->ID), "", std::to_string(m->ID));
                std::vector<MPVariable*> vec2 = getMPVarOfWhich(solver->variables(), std::to_string(d->ID), std::to_string(oe.des->ID), std::to_string(m->ID), "");
                for (MPVariable* v1 : vec1) {
                    MPConstraint* const c = solver->MakeRowConstraint(-solver->infinity(), 0);
                    c->SetCoefficient(v1, 1); // LOG_IDT<<v1->name();
                    for (MPVariable* v2 : vec2) {
                        c->SetCoefficient(v2, -1); // LOG_IDT<<"-  "<<v2->name();
                    }
                    // LOG_IDT<<"<=  0\n";
                }
            }
            int descycle = oe.isAnti ? oe.des->cycle + II : oe.des->cycle;
            for (int i=1; d->cycle+i!=descycle; i++) { // a(0) -> c(3)
                // [x]1. a-c cross multiple cycle;
                for (MRRGNode* m : mrrg.getFUsOfT((d->cycle+i)%II)) {
                    std::vector<MPVariable*> vec1 = getMPVarOfWhich(solver->variables(), std::to_string(d->ID), std::to_string(oe.des->ID), "", std::to_string(m->ID));
                    std::vector<MPVariable*> vec2 = getMPVarOfWhich(solver->variables(), std::to_string(d->ID), std::to_string(oe.des->ID), std::to_string(m->ID), "");
                    for (MPVariable* v1 : vec1) {
                        MPConstraint* const c = solver->MakeRowConstraint(-solver->infinity(), 0);
                        c->SetCoefficient(v1, 1); // LOG_IDT<<v1->name();
                        for (MPVariable* v2 : vec2) {
                            c->SetCoefficient(v2, -1); // LOG_IDT<<"-  "<<v2->name();
                        }
                        // LOG_IDT<<"<=  0\n";
                    }
                }
            }
        }
    }

    /* CONSTRAINT: Consistency. DFG b->c and DFG a->c Mapped to the same place on MRRG */
    // for (MRRGNode * m : mrrg.getFUsOfT(0, II)) {
    //     for (DFGNode* d : dfg->getOpsOfCycleModII(m->T, II)) {
    //         std::vector<DFGEdge> vec = dfg->getEdgesTo(d);
    //         for (int i=1; i<vec.size(); i++) {
    //             DFGNode* ds = vec[i-1].src;
    //             std::vector<MPVariable*> vec1 = getMPVarOfWhich(solver->variables(), std::to_string(ds->ID), std::to_string(d->ID), "", std::to_string(m->ID));
    //             ds = vec[i].src;
    //             std::vector<MPVariable*> vec2 = getMPVarOfWhich(solver->variables(), std::to_string(ds->ID), std::to_string(d->ID), "", std::to_string(m->ID));
    //             MPConstraint* const c = solver->MakeRowConstraint(0, 0);
    //             for (MPVariable* v : vec1)
    //                 c->SetCoefficient(v, 1);
    //             for (MPVariable* v : vec2)
    //                 c->SetCoefficient(v, -1);
    //         }
    //     }
    // }

    // LOG_INFO<<"Number of variables = "<<solver->NumVariables()<<"\n";
    // for (MPVariable* const var : solver->variables()) {
    //     LOG_IDT<<var->name()<<"\n";
    // }
    

    // LOG_INFO<<"Number of constraints = "<<solver->NumConstraints()<<"\n";
    // for (MPConstraint const* c : solver->constraints()) {
    //     LOG_IDT<<*c<<"\n";
    // }

    MPObjective* const obj = solver->MutableObjective();
    for (MPVariable* var : solver->variables()) {
        obj->SetCoefficient(var, 1);
    }
    obj->SetMinimization();

    LOG_INFO<<"Solving II="<<II<<" with "<<solver->SolverVersion()<<"";
    auto stop = std::chrono::high_resolution_clock::now();
    const MPSolver::ResultStatus result_status = solver->Solve();
    auto end = std::chrono::high_resolution_clock::now();

    // LOG_INFO<<"Solution:";
    // LOG_IDT<<"Objective value = "<<obj->Value()<<"\n";
    // for (MPVariable* var : solver->variables()) {
    //     LOG_IDT<<var->name()<<" = " <<var->solution_value()<<"\n";
    // }
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - stop);
    std::cout<<"; Solving time: "<<duration.count()<<" ms. ";

    std::cout<<"Result status: ";
    if (result_status != MPSolver::OPTIMAL) {
        std::cout<<YELLOW(result_status)<<".\n";
        if (result_status == MPSolver::FEASIBLE)
            ;
            // std::cout<<"A potentially suboptimal solution was found.\n";
        else {
            // std::cout<<"The solver could not solve the problem!\n";
            return Mapping();
        }
    } else 
        std::cout<<GREEN(result_status)<<"\n";

    /* Construct Mapping */
    for (MPVariable* var : solver->variables()) {
        if (var->solution_value() != 1)
            continue;
        auto [ds, dd, ms, md] = getDFGAndMRRGFromMPVar(var);
        DFGEdge& de = dfg->getEdge(ds, dd);
        MRRGEdge& mr = mrrg.getEdge(ms, md);
        mapping[de].push_back(mr);
    }

    return mapping;
}