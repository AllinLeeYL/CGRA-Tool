#include "Common.hpp"
#include "Mapper.hpp"
#include "ortools/base/logging.h"
#include "ortools/linear_solver/linear_solver.h"

using namespace cgratool;
using namespace operations_research;

ILPMapper::ILPMapper(DFG* dfg, CGRA cgra) : Mapper(dfg, cgra) { ; }

Mapping ILPMapper::map(int II, int time_limit) {
    Mapping mapping;
    time_t start = time(NULL);
    for (int ii = II; mapping.isNull(); ii++) {
        time_t now = time(NULL);
        if (now - start > time_limit)
            return mapping;
        mapping = this->mapII(ii);
    }
    return mapping;
}

Mapping ILPMapper::mapII(int II, int time_limit) {
    Mapping mapping(this->dfg, MRRG(this->cgra, II));
    MRRG &mrrg = mapping.mrrg;
    std::vector<DFGNode*> ops = this->dfg->getOps();
    std::vector<MRRGNode*> fus = mrrg.getFUs(II);
    
    std::unique_ptr<MPSolver> solver(MPSolver::CreateSolver("GLOP"));
    if (!solver) {
        LOG_WARNING<<"Could not create solver GLOP\n";
    }
    /* FU -- OP Mapping */
    std::vector<DFGEdge> dfgEdges = dfg->getEdges();
    for (DFGEdge dfgEdge : dfgEdges) {
        for (MRRGNode* fu : fus) {
            std::string name = std::to_string(dfgEdge.ID)+"_"+fu->getLabel();
            MPVariable* const t = solver->MakeBoolVar(name);
        }
    }

    std::vector<std::vector<MPVariable*>> fuop;
    for (int i=0; i<fus.size(); i++) {
        fuop.push_back(std::vector<MPVariable*>());
        for (int j=0; j<ops.size(); j++) {
            std::string name = "fuop"+std::to_string(i)+std::to_string(j);
            MPVariable* const t = solver->MakeBoolVar(name);
            fuop[i].push_back(t);
        }
    }

    /* FU -- OP Route Resource Mapping*/
    std::vector<std::vector<MPVariable*>> Rfuop;
    for (int i=0; i<fus.size(); i++) {
        fuop.push_back(std::vector<MPVariable*>());
        for (int j=0; j<ops.size(); j++) {
            std::string name = "Rfuop"+std::to_string(i)+std::to_string(j);
            MPVariable* const t = solver->MakeBoolVar(name);
            fuop[i].push_back(t);
        }
    }

    LOG_INFO<<"Number of variables = "<<solver->NumVariables()<<"\n";
    /* Constrains: Every OP is placed on exactly one FU. */

    
    // /* FU -- OP */
    // MPConstraint* const ct = solver->MakeRowConstraint(-MPSolver::infinity(), 2.0, "ct");
    // ct->SetCoefficient(x, 1);
    // ct->SetCoefficient(y, 1);
    // LOG_INFO<<"Number of constraints = "<<solver->NumConstraints()<<"\n";

    // MPObjective* const obj = solver->MutableObjective();
    // obj->SetCoefficient(x, 3);
    // obj->SetCoefficient(y, 1);
    // obj->SetMaximization();

    // LOG_INFO<<"Solving with "<<solver->SolverVersion()<<"\n";
    // const MPSolver::ResultStatus result_status = solver->Solve();
    // LOG_INFO<<"Status: "<<result_status<<"\n";
    // if (result_status != MPSolver::OPTIMAL) {
    //     LOG_WARNING<<"The problem does not have an optimal solution.\n";
    //     if (result_status == MPSolver::FEASIBLE) {
    //         LOG_INFO<<"A potentially suboptimal solution was found.\n";
    //     } else {
    //         LOG_WARNING<<"The solver could not solve the problem!\n";
    //         return Mapping();
    //     }
    // }
    
    // LOG_INFO<<"Solution:\n";
    // LOG_INFO<<"Objective value = "<<obj->Value()<<"\n";
    // LOG_INFO<<"x = " <<x->solution_value()<<"\n";
    // LOG_INFO<<"y = " <<y->solution_value()<<"\n";
    return Mapping();
}