// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <sgpp/finance/basis/linear/boundary/algorithm_sweep/DPhiPhiUpBBLinearBoundary.hpp>

#include <sgpp/globaldef.hpp>


namespace SGPP {
  namespace finance {



    DPhiPhiUpBBLinearBoundary::DPhiPhiUpBBLinearBoundary(SGPP::base::GridStorage* storage) : DPhiPhiUpBBLinear(storage) {
    }

    DPhiPhiUpBBLinearBoundary::~DPhiPhiUpBBLinearBoundary() {
    }

    void DPhiPhiUpBBLinearBoundary::operator()(SGPP::base::DataVector& source, SGPP::base::DataVector& result, grid_iterator& index, size_t dim) {
      // get boundary values
      float_t fl = 0.0;
      float_t fr = 0.0;

      if (!index.hint()) {
        index.top(dim);

        if (!this->storage->end(index.seq())) {
          rec(source, result, index, dim, fl, fr);
        }

        index.left_levelzero(dim);
      }

      size_t seq_left;
      size_t seq_right;

      // left boundary
      seq_left = index.seq();

      // right boundary
      index.right_levelzero(dim);
      seq_right = index.seq();

      // check boundary conditions
      if (this->boundingBox->hasDirichletBoundaryLeft(dim)) {
        result[seq_left] = 0.0; // source[seq_left];
      } else {
        // up
        //////////////////////////////////////
        result[seq_left] = fl;

        result[seq_left] += source[seq_right] * (0.5);
      }

      if (this->boundingBox->hasDirichletBoundaryRight(dim)) {
        result[seq_right] = 0.0; //source[seq_right];
      } else {
        result[seq_right] = fr;
      }

      index.left_levelzero(dim);
    }

    // namespace detail

  } // namespace SGPP
}