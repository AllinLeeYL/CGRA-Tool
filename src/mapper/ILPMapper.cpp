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

/* class functions */

ILPMapper::ILPMapper(DFG* dfg, CGRA cgra) : Mapper(dfg, cgra) { ; }

Mapping ILPMapper::map(int II, int time_limit) {
    Mapping mapping;
    time_t start = time(NULL);
    for (int ii = II; mapping.isNull(); ii++) {
        time_t now = time(NULL);
        if (now - start > time_limit)
            return mapping;
        mapping = this->mapII(ii);
        break; //! delete this after debugging
    }
    return mapping;
}

Mapping ILPMapper::mapII(int II, int time_limit) {
    Mapping mapping(this->dfg, MRRG(this->cgra, II));
    MRRG &mrrg = mapping.mrrg;
    // std::vector<DFGNode*> ops = this->dfg->getOps();
    // std::vector<MRRGNode*> fus = mrrg.getFUs(II);
    
    std::unique_ptr<MPSolver> solver(MPSolver::CreateSolver("GLOP"));
    if (!solver) {
        LOG_WARNING<<"Could not create solver GLOP\n";
    }

    { /* mappable place for root node */
        std::vector<DFGNode *> rootNodes = dfg->getOpsOfLatestCycle(0);
        for (DFGNode* node : rootNodes) {
            for (MRRGNode* fu : mrrg.getFUsOfT(0)) {
                std::string dfgName = "S-"+std::to_string(node->ID);
                std::string mrrgName = "S-"+std::to_string(fu->ID);
                MPVariable* const v = solver->MakeBoolVar(dfgName+":"+mrrgName);
            }
        }
    }

    /* unique edge variables for ILP from DFG and MRRG */
    std::vector<std::string> dECls; // Class names of DFG edges
    std::vector<std::string> mECls; // Class names of MRRG edges
    {
        for (MPVariable* const var : solver->variables()) {
            std::vector<std::string> ss = split(var->name(), ":");
            assert(ss.size() == 2);
            dECls.push_back(ss[0]);
            mECls.push_back(ss[1]);
        }
        // remove duplicates
        std::set<std::string> s1(dECls.begin(), dECls.end() );
        dECls.assign(s1.begin(), s1.end());
        std::set<std::string> s2(mECls.begin(), mECls.end() );
        mECls.assign(s2.begin(), s2.end());

        LOG_INFO<<"(ILP) DFG Variable classses:\n  ";
        for (auto s : dECls)
            LOG_IDT<<s;
        std::cout<<"\n";
        LOG_INFO<<"(ILP) MRRG Variable classses:\n  ";
        for (auto s : mECls)
            LOG_IDT<<s;
        std::cout<<"\n";
    }

    /* CONSTRAIN: Mapped */
    for (std::string cls : dECls) {
        std::vector<MPVariable*> vec;
        for (MPVariable* const var : solver->variables()) {
            std::vector<std::string> ss = split(var->name(), ":");
            assert(ss.size() == 2);
            if (ss[0] == cls)
                vec.push_back(var);
        }
        MPConstraint* const c = solver->MakeRowConstraint(1, solver->infinity());
        for (MPVariable* v : vec) {
            c->SetCoefficient(v, 1);
        }
    }

    // LOG_INFO<<"Number of variables = "<<solver->NumVariables()<<"\n";
    // for (MPVariable* const var : solver->variables()) {
    //     LOG_IDT<<var->name()<<"\n";
    // }
    

    
    // /* FU -- OP */
    // MPConstraint* const ct = solver->MakeRowConstraint(-MPSolver::infinity(), 2.0, "ct");
    // ct->SetCoefficient(x, 1);
    // ct->SetCoefficient(y, 1);
    // LOG_INFO<<"Number of constraints = "<<solver->NumConstraints()<<"\n";

    MPObjective* const obj = solver->MutableObjective();
    for (MPVariable* var : solver->variables()) {
        obj->SetCoefficient(var, 1);
    }
    obj->SetMinimization();

    LOG_INFO<<"Solving with "<<solver->SolverVersion()<<"\n";
    const MPSolver::ResultStatus result_status = solver->Solve();
    LOG_INFO<<"Status: "<<result_status<<"\n";
    if (result_status != MPSolver::OPTIMAL) {
        LOG_WARNING<<"The problem does not have an optimal solution.\n";
        if (result_status == MPSolver::FEASIBLE)
            LOG_INFO<<"A potentially suboptimal solution was found.\n";
        else {
            LOG_WARNING<<"The solver could not solve the problem!\n";
            return Mapping();
        }
    }
    
    LOG_INFO<<"Solution:\n";
    LOG_INFO<<"Objective value = "<<obj->Value()<<"\n";
    for (MPVariable* var : solver->variables()) {
        LOG_IDT<<var->name()<<" = " <<var->solution_value()<<"\n";
    }
    return Mapping();
}