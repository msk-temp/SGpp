// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#ifndef SWEEP_HPP
#define SWEEP_HPP

#include <sgpp/base/grid/GridStorage.hpp>
#include <sgpp/base/datatypes/DataVector.hpp>

#include <sgpp/globaldef.hpp>

#include <vector>
#include <utility>
#include <iostream>


namespace sgpp {
namespace base {

/**
 * Standard sweep operation
 * FUNC should be a class with overwritten operator(). For an example see laplace_up_functor in laplace.hpp.
 * It must be default constructable or copyable.
 * STORAGE must provide a grid_iterator supporting left_child, step_right, up, hint and seq.
 */
template<class FUNC>
class sweep {
 protected:
  typedef GridStorage::grid_iterator grid_iterator;

  /// Object of FUNC, this is executed by sweep
  FUNC functor;
  /// Pointer to the grid's storage object
  GridStorage& storage;
  /// algorithmic dimensions, operator is applied in this dimensions
  const std::vector<size_t> algoDims;
  /// number of algorithmic dimensions
  const size_t numAlgoDims_;

 public:
  /**
   * Create a new sweep object with a default constructed functor
   *
   * @param storage the storage that contains the grid points
   */
  explicit sweep(GridStorage& storage) : functor(), storage(storage),
    algoDims(storage.getAlgorithmicDimensions()),
    numAlgoDims_(storage.getAlgorithmicDimensions().size()) {
  }

  /**
   * Create a new sweep object with a copied functor
   *
   * @param functor the functor that is executed on the grid
   * @param storage the storage that contains the grid points
   */
  sweep(FUNC& functor, GridStorage& storage) :
    functor(functor), storage(storage),
    algoDims(storage.getAlgorithmicDimensions()),
    numAlgoDims_(storage.getAlgorithmicDimensions().size()) {
  }

  /**
   * Destructor
   */
  ~sweep() {
  }

  /**
   * Descends on all dimensions beside dim_sweep. Class functor for dim_sweep
   * Boundaries are not regarded
   *
   * @param source a DataVector containing the source coefficients of the grid points
   * @param result a DataVector containing the result coefficients of the grid points
   * @param dim_sweep the dimension in which the functor is executed
   */
  void sweep1D(DataVector& source, DataVector& result, size_t dim_sweep) {
    // generate a list of all dimension (-dim_sweep)
    // from dimension recursion unrolling
    std::vector<size_t> dim_list;

    for (size_t i = 0; i < storage.getDimension(); i++) {
      if (i != dim_sweep) {
        dim_list.push_back(i);
      }
    }

    grid_iterator index(storage);

    sweep_rec(source, result, index, dim_list, storage.getDimension() - 1, dim_sweep);
  }

  /**
   * Descends on all dimensions beside dim_sweep. Class functor for dim_sweep
   * Boundaries are not regarded
   *
   * @param source a DataMatrix containing the source coefficients of the grid points
   * @param result a DataMatrix containing the result coefficients of the grid points
   * @param dim_sweep the dimension in which the functor is executed
   */
  void sweep1D(DataMatrix& source, DataMatrix& result, size_t dim_sweep) {
    // generate a list of all dimension (-dim_sweep)
    // from dimension recursion unrolling
    std::vector<size_t> dim_list;

    for (size_t i = 0; i < this->numAlgoDims_; i++) {
      if (i != dim_sweep) {
        dim_list.push_back(i);
      }
    }

    grid_iterator index(storage);

    sweep_rec(source, result, index, dim_list, this->numAlgoDims_ - 1,
              dim_sweep);
  }

  /**
   * Descends on all dimensions beside dim_sweep. Class functor for dim_sweep
   * Boundaries are regarded
   *
   * @param source a DataVector containing the source coefficients of the grid points
   * @param result a DataVector containing the result coefficients of the grid points
   * @param dim_sweep the dimension in which the functor is executed
   */
  void sweep1D_Boundary(DataVector& source, DataVector& result,
                        size_t dim_sweep) {
    // generate a list of all dimension (-dim_sweep) from
    // dimension recursion unrolling
    std::vector<size_t> dim_list;

    for (size_t i = 0; i < storage.getDimension(); i++) {
      if (i != dim_sweep) {
        dim_list.push_back(i);
      }
    }

    grid_iterator index(storage);
    index.resetToLevelZero();

    sweep_Boundary_rec(source, result, index, dim_list, storage.getDimension() - 1,
                       dim_sweep);
  }


  /**
   * Descends on all dimensions beside dim_sweep. Class functor for dim_sweep
   * Boundaries are regarded
   *
   * @param source a DataMatrix containing the source coefficients of the grid points
   * @param result a DataMatrix containing the result coefficients of the grid points
   * @param dim_sweep the dimension in which the functor is executed
   */
  void sweep1D_Boundary(DataMatrix& source, DataMatrix& result,
                        size_t dim_sweep) {
    // generate a list of all dimension (-dim_sweep) from
    // dimension recursion unrolling
    std::vector<size_t> dim_list;

    for (size_t i = 0; i < storage.getDimension(); i++) {
      if (i != dim_sweep) {
        dim_list.push_back(i);
      }
    }

    grid_iterator index(storage);
    index.resetToLevelZero();

    sweep_Boundary_rec(source, result, index, dim_list, storage.getDimension() - 1,
                       dim_sweep);
  }

 protected:
  /**
   * Descends on all dimensions beside dim_sweep. Class functor for dim_sweep.
   * Boundaries are not regarded
   *
   * @param source coefficients of the sparse grid
   * @param result coefficients of the function computed by sweep
   * @param index current grid position
   * @param dim_list list of dimensions, that should be handled
   * @param dim_rem number of remaining dims
   * @param dim_sweep static dimension, in this dimension the functor is executed
   */
  void sweep_rec(DataVector& source, DataVector& result, grid_iterator& index,
                 std::vector<size_t>& dim_list, size_t dim_rem,
                 size_t dim_sweep) {
    functor(source, result, index, dim_sweep);

    // dimension recursion unrolled
    for (size_t d = 0; d < dim_rem; d++) {
      size_t current_dim = dim_list[d];

      if (index.hint()) {
        continue;
      }

      index.leftChild(current_dim);

      if (!storage.isInvalidSequenceNumber(index.seq())) {
        sweep_rec(source, result, index, dim_list, d + 1, dim_sweep);
      }

      index.stepRight(current_dim);

      if (!storage.isInvalidSequenceNumber(index.seq())) {
        sweep_rec(source, result, index, dim_list, d + 1, dim_sweep);
      }

      index.up(current_dim);
    }
  }

  /**
   * Descends on all dimensions beside dim_sweep. Class functor for dim_sweep.
   * Boundaries are not regarded
   *
   * @param source coefficients of the sparse grid
   * @param result coefficients of the function computed by sweep
   * @param index current grid position
   * @param dim_list list of dimensions, that should be handled
   * @param dim_rem number of remaining dims
   * @param dim_sweep static dimension, in this dimension the functor is executed
  */
  void sweep_rec(DataMatrix& source, DataMatrix& result, grid_iterator& index,
                 std::vector<size_t>& dim_list, size_t dim_rem,
                 size_t dim_sweep) {
    functor(source, result, index, dim_sweep);

    // dimension recursion unrolled
    for (size_t d = 0; d < dim_rem; d++) {
      size_t current_dim = dim_list[d];

      if (index.hint()) {
        continue;
      }

      index.leftChild(current_dim);

      if (!storage.isInvalidSequenceNumber(index.seq())) {
        sweep_rec(source, result, index, dim_list, d + 1, dim_sweep);
      }

      index.stepRight(current_dim);

      if (!storage.isInvalidSequenceNumber(index.seq())) {
        sweep_rec(source, result, index, dim_list, d + 1, dim_sweep);
      }

      index.up(current_dim);
    }
  }

  /**
   * Descends on all dimensions beside dim_sweep. Class functor for dim_sweep.
   * Boundaries are regarded
   *
   * @param source coefficients of the sparse grid
   * @param result coefficients of the function computed by sweep
   * @param index current grid position
   * @param dim_list list of dimensions, that should be handled
   * @param dim_rem number of remaining dims
   * @param dim_sweep static dimension, in this dimension the functor is executed
   */
  void sweep_Boundary_rec(DataVector& source, DataVector& result,
                          grid_iterator& index,
                          std::vector<size_t>& dim_list, size_t dim_rem,
                          size_t dim_sweep) {
    if (dim_rem == 0) {
      functor(source, result, index, dim_sweep);
    } else {
      typedef level_t level_type;
      typedef index_t index_type;

      level_type current_level;
      index_type current_index;

      index.get(dim_list[dim_rem - 1], current_level, current_index);

      // handle level greater zero
      if (current_level > 0) {
        // given current point to next dim
        sweep_Boundary_rec(source, result, index, dim_list, dim_rem - 1,
                           dim_sweep);

        if (!index.hint()) {
          index.leftChild(dim_list[dim_rem - 1]);

          if (!storage.isInvalidSequenceNumber(index.seq())) {
            sweep_Boundary_rec(source, result, index, dim_list, dim_rem,
                               dim_sweep);
          }

          index.stepRight(dim_list[dim_rem - 1]);

          if (!storage.isInvalidSequenceNumber(index.seq())) {
            sweep_Boundary_rec(source, result, index, dim_list, dim_rem,
                               dim_sweep);
          }

          index.up(dim_list[dim_rem - 1]);
        }
      } else {  // handle level zero
        sweep_Boundary_rec(source, result, index, dim_list, dim_rem - 1,
                           dim_sweep);

        index.resetToRightLevelZero(dim_list[dim_rem - 1]);
        sweep_Boundary_rec(source, result, index, dim_list, dim_rem - 1,
                           dim_sweep);

        if (!index.hint()) {
          index.resetToLevelOne(dim_list[dim_rem - 1]);

          if (!storage.isInvalidSequenceNumber(index.seq())) {
            sweep_Boundary_rec(source, result, index, dim_list, dim_rem,
                               dim_sweep);
          }
        }

        index.resetToLeftLevelZero(dim_list[dim_rem - 1]);
      }
    }
  }

  /**
   * Descends on all dimensions beside dim_sweep. Class functor for dim_sweep.
   * Boundaries are regarded
   *
   * @param source coefficients of the sparse grid
   * @param result coefficients of the function computed by sweep
   * @param index current grid position
   * @param dim_list list of dimensions, that should be handled
   * @param dim_rem number of remaining dims
   * @param dim_sweep static dimension, in this dimension the functor is executed
   */
  void sweep_Boundary_rec(DataMatrix& source, DataMatrix& result,
                          grid_iterator& index,
                          std::vector<size_t>& dim_list, size_t dim_rem,
                          size_t dim_sweep) {
    if (dim_rem == 0) {
      functor(source, result, index, dim_sweep);
    } else {
      typedef level_t level_type;
      typedef index_t index_type;

      level_type current_level;
      index_type current_index;

      index.get(dim_list[dim_rem - 1], current_level, current_index);

      // handle level greater zero
      if (current_level > 0) {
        // given current point to next dim
        sweep_Boundary_rec(source, result, index, dim_list, dim_rem - 1,
                           dim_sweep);

        if (!index.hint()) {
          index.leftChild(dim_list[dim_rem - 1]);

          if (!storage.isInvalidSequenceNumber(index.seq())) {
            sweep_Boundary_rec(source, result, index, dim_list, dim_rem,
                               dim_sweep);
          }

          index.stepRight(dim_list[dim_rem - 1]);

          if (!storage.isInvalidSequenceNumber(index.seq())) {
            sweep_Boundary_rec(source, result, index, dim_list, dim_rem,
                               dim_sweep);
          }

          index.up(dim_list[dim_rem - 1]);
        }
      } else {  // handle level zero
        sweep_Boundary_rec(source, result, index, dim_list, dim_rem - 1,
                           dim_sweep);

        index.resetToRightLevelZero(dim_list[dim_rem - 1]);
        sweep_Boundary_rec(source, result, index, dim_list, dim_rem - 1,
                           dim_sweep);

        if (!index.hint()) {
          index.resetToLevelOne(dim_list[dim_rem - 1]);

          if (!storage.isInvalidSequenceNumber(index.seq())) {
            sweep_Boundary_rec(source, result, index, dim_list, dim_rem,
                               dim_sweep);
          }
        }

        index.resetToLeftLevelZero(dim_list[dim_rem - 1]);
      }
    }
  }
};

}  // namespace base
}  // namespace sgpp

#endif /* SWEEP_HPP */
