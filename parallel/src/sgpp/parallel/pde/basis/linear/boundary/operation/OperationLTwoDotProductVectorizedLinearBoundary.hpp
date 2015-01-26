// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at 
// sgpp.sparsegrids.org

#ifndef OPERATIONLTWODOTPRODUCTVECTORIZEDLINEARBOUNDARY_HPP
#define OPERATIONLTWODOTPRODUCTVECTORIZEDLINEARBOUNDARY_HPP

#include <vector>

#include <sgpp/base/operation/OperationMatrix.hpp>
#include <sgpp/base/datatypes/DataMatrix.hpp>
#include <sgpp/base/grid/Grid.hpp>

#include <sgpp/base/tools/SGppStopwatch.hpp>

#include <sgpp/parallel/tools/TypesParallel.hpp>

#if defined(__SSE4_2__) || defined(__AVX__)
#include <immintrin.h>
#endif
#if defined(__FMA4__)
#include <x86intrin.h>
#endif

#ifdef __USEAVX128__
#undef __AVX__
#endif

#include <sgpp/globaldef.hpp>


namespace SGPP {
  namespace parallel {

    /**
     * Implementation for linear functions of LTwoDotProduct Operation, linear grids with boundaries
     *
     * @version $HEAD$
     */
    class OperationLTwoDotProductVectorizedLinearBoundary: public SGPP::base::OperationMatrix {
      private:

        SGPP::base::GridStorage* storage;
        SGPP::base::DataMatrix* level_;
        SGPP::base::DataMatrix* level_int_;
        SGPP::base::DataMatrix* index_;
        SGPP::base::DataVector* lcl_q_;
        SGPP::base::DataVector* constants_;
        SGPP::base::DataVector* alpha_padded_;

        SGPP::base::DataVector* result_boundary_filtered_;

        SGPP::base::DataMatrix* level_boundary_filtered_;
        SGPP::base::DataMatrix* level_int_boundary_filtered_;
        SGPP::base::DataMatrix* index_boundary_filtered_;


        SGPP::base::DataVector** gradient_temp;
        SGPP::base::DataVector** l2dot_temp;

#if defined(STORE_PDE_MATRIX_BOUNDARY)
        SGPP::base::DataMatrix* operation_result_matrix_;
        bool operation_result_generated_;
#endif
        std::vector<std::size_t> i_boundary_filtered;


        int process_count;
        int process_index;

        std::vector<int> all_i_start;
        std::vector<int> all_i_size;

        std::vector<int> send_start;
        std::vector<int> send_size;

        std::vector<int> recv_start;
        std::vector<int> recv_size;


        void init_constants();
        void init_grid_storage();

        double l2dot_dirichlet(size_t i, size_t j, size_t dim);

        void mult_dirichlet(SGPP::base::DataVector& alpha, SGPP::base::DataVector& result);
        double all_time;
        double all_iterations;
        SGPP::base::SGppStopwatch stopWatch;

      public:
        /**
         * Construtor of OperationLTwoProductLinear
         *
         * @param storage Pointer to the grid's gridstorage obejct
         */
        OperationLTwoDotProductVectorizedLinearBoundary(SGPP::base::GridStorage* storage);

        /**
         * Destructor
         */
        virtual ~OperationLTwoDotProductVectorizedLinearBoundary();

        virtual void mult(SGPP::base::DataVector& alpha, SGPP::base::DataVector& result);
        virtual void reset();
    };

  }

}

#endif /* OPERATIONLAPLACEVECTORIZEDLINEARBOUNDARY_HPP */