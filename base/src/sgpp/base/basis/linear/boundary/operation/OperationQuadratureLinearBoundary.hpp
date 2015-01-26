// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at 
// sgpp.sparsegrids.org

#ifndef OPERATIONQUADRATURELINEARBOUND_HPP
#define OPERATIONQUADRATURELINEARBOUND_HPP

#include <sgpp/base/operation/OperationQuadrature.hpp>
#include <sgpp/base/grid/Grid.hpp>

#include <sgpp/globaldef.hpp>


namespace SGPP {
  namespace base {

    /**
     * Quadrature on sparse grid, linear grid without boundaries
     */
    class OperationQuadratureLinearBoundary : public OperationQuadrature {
      public:
        /**
         * Constructor of OperationQuadratureLinear
         *
         * @param storage Pointer to the grid's GridStorage object
         */
        OperationQuadratureLinearBoundary(GridStorage* storage) : storage(storage) {}

        virtual ~OperationQuadratureLinearBoundary() {}

        /**
         * Quadrature for piecewise linear hat basis functions. Computes
         * @f[ \sum_{\vec{l}} 2^{-|\vec{l}|}\alpha_{\vec{l}}. @f]
         *
         * @param alpha Coefficient vector for current grid
         */
        virtual double doQuadrature(DataVector& alpha);

      protected:
        // Pointer to the grid's GridStorage object
        GridStorage* storage;
    };

  }
}

#endif /* OPERATIONQUADRATURELINEARBOUND_HPP */