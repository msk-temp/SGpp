// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at 
// sgpp.sparsegrids.org

#ifndef POISSONEQUATIONELLIPTICPDESOLVERSYSTEMDIRICHLETPARALLELMPI_HPP
#define POISSONEQUATIONELLIPTICPDESOLVERSYSTEMDIRICHLETPARALLELMPI_HPP

#include <sgpp/pde/operation/OperationEllipticPDESolverSystemDirichlet.hpp>

#include <sgpp/globaldef.hpp>


namespace SGPP {
  namespace parallel {

    /**
     * This class uses SGPP::pde::OperationEllipticPDESolverSystemDirichlet
     * to define a solver system for the Poission Equation.
     *
     * For the mult-routine only the Laplace-Operator is required
     *
     * There is a parallelization over all operators, required
     * to solve the poisson equation.
     */
    class PoissonEquationEllipticPDESolverSystemDirichletParallelMPI : public SGPP::pde::OperationEllipticPDESolverSystemDirichlet {
      protected:
        SGPP::base::OperationMatrix* Laplace_Inner;
        SGPP::base::OperationMatrix* Laplace_Complete;

        void applyLOperatorComplete(SGPP::base::DataVector& alpha, SGPP::base::DataVector& result);

        void applyLOperatorInner(SGPP::base::DataVector& alpha, SGPP::base::DataVector& result);

      public:
        /**
         * Constructor
         *
         * @param SparseGrid reference to a sparse grid on which the Poisson Equation should be solved
         * @param rhs the right hand side for solving the elliptic PDE
         */
        PoissonEquationEllipticPDESolverSystemDirichletParallelMPI(SGPP::base::Grid& SparseGrid, SGPP::base::DataVector& rhs);

        /**
         * Destructor
         */
        virtual ~PoissonEquationEllipticPDESolverSystemDirichletParallelMPI();

        void mult(SGPP::base::DataVector& alpha, SGPP::base::DataVector& result);

        SGPP::base::DataVector* generateRHS();
    };

  }
}

#endif /* POISSONEQUATIONELLIPTICPDESOLVERSYSTEMDIRICHLETPARALLELMPI_HPP */