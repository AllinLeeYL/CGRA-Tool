#ifndef INCLUDE_MAPPER_H_
#define INCLUDE_MAPPER_H_

#include "DFG.hpp"
#include "CGRA.hpp"
#include "MRRG.hpp"
#include "Mapping.hpp"
#include "ortools/linear_solver/linear_solver.h"

namespace cgratool
{

    class Mapper
    {
    protected:
        DFG *dfg;
        CGRA cgra;

    public:
        Mapper(DFG *dfg, CGRA cgra);
        ~Mapper();
        int getResMII();
        int getRecMII();

        /** @brief The entry point for mapping.
         *
         * This function need to be implemented by subclasses and will return a Mapping.
         */
        Mapping map();
    };

    class RandomMapper : public Mapper
    {
    public:
        RandomMapper(DFG *dfg, CGRA cgra);
        ~RandomMapper();

        /** @brief Try mapping the DFG onto the CGRA.
         *
         * @param II The mapping starts from II and will increase II until
         *           exceeding time limit or finding a legal mapping.
         * @param time_limit The time limit (second).
         */
        Mapping map(int II, int time_limit = 1200);

        /** @brief Try mapping the DFG onto the CGRA with a given II.
         *
         * @param II Try finding all possible mapping for II until
         *           exceeding time limit or finding a legal mapping.
         *           This function will not try to increase the number of II.
         * @param time_limit The time limit (second).
         */
        Mapping mapII(int II, int time_limit = 120);
        void genRandomMap();
    };

    class ExhaustiveMapper : public Mapper
    {
    public:
        ExhaustiveMapper(DFG *dfg, CGRA cgra);

        /** @brief Try mapping the DFG onto the CGRA.
         *
         * @param II The mapping starts from II and will increase II until
         *           exceeding time limit or finding a legal mapping.
         * @param time_limit The time limit (second).
         */
        Mapping map(int II, int time_limit = 1200);

        /** @brief Try mapping the DFG onto the CGRA with a given II.
         *
         * @param II Try finding all possible mapping for II until
         *           exceeding time limit or finding a legal mapping.
         *           This function will not try to increase the number of II.
         * @param time_limit The time limit (second).
         */
        Mapping mapII(int II, int time_limit = 1200);
    };

    class HeuristicMapper : public Mapper
    {
    public:
        HeuristicMapper(DFG *dfg, CGRA cgra);

        /** @brief Try mapping the DFG onto the CGRA.
         *
         * @param II The mapping starts from II and will increase II until
         *           exceeding time limit or finding a legal mapping.
         * @param time_limit The time limit (second).
         */
        Mapping map(int II, int time_limit = 1200);

        /** @brief Try mapping the DFG onto the CGRA with a given II.
         *
         * @param II Try finding all possible mapping for II until
         *           exceeding time limit or finding a legal mapping.
         *           This function will not try to increase the number of II.
         * @param time_limit The time limit (second).
         */
        Mapping mapII(int II, int time_limit = 120);
    };

    /** @brief This mapper will find a feasible mapping as soon as possible.
     *  But the mapping found is probably not optimal.
     */
    class FeasibleMapper : public Mapper
    {
    public:
        FeasibleMapper(DFG *dfg, CGRA cgra);

        /** @brief Try mapping the DFG onto the CGRA.
         *
         * @param II The minimium II.
         * @param time_limit The time limit (second).
         */
        Mapping map(int II, int time_limit = 1200);
    };

    /** @brief ILP Mapper
     */
    class ILPMapper : public Mapper
    {
    public:
        std::string solverName;
    public:
        ILPMapper(DFG *dfg, CGRA cgra, std::string solver="CP-SAT");

        /** @brief Try mapping the DFG onto the CGRA.
         *
         * @param II The minimium II.
         * @param time_limit The time limit (second).
         */
        Mapping map(int II, int time_limit = 1200);

        /** @brief Try mapping the DFG onto the CGRA with a given II.
         *
         * @param II Try finding all possible mapping for II until
         *           exceeding time limit or finding a legal mapping.
         *           This function will not try to increase the number of II.
         * @param time_limit The time limit (second).
         */
        std::tuple<Mapping, 
                   std::chrono::milliseconds, 
                   std::chrono::milliseconds, 
                   operations_research::MPSolver::ResultStatus> mapII(int II, int time_limit = 120);
    };

}

#endif