// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <numeric>

#ifdef _OPENMP
#include <omp.h>
#endif

#include <sgpp/globaldef.hpp>
#include <sgpp/optimization/gridgen/IterativeGridGenerator.hpp>

namespace SGPP {
  namespace optimization {

    IterativeGridGenerator::IterativeGridGenerator(
      ObjectiveFunction& f, base::Grid& grid, size_t N) :
      f(f),
      grid(grid),
      N(N),
      functionValues(0) {
    }

    IterativeGridGenerator::~IterativeGridGenerator() {
    }

    base::Grid& IterativeGridGenerator::getGrid() const {
      return grid;
    }

    const base::DataVector& IterativeGridGenerator::getFunctionValues() const {
      return functionValues;
    }

    void IterativeGridGenerator::undoRefinement(size_t oldGridSize) {
      base::GridStorage& gridStorage = *grid.getStorage();
      std::list<size_t> indicesToRemove(gridStorage.size() - oldGridSize);
      std::iota(indicesToRemove.begin(), indicesToRemove.end(), oldGridSize);
      gridStorage.deletePoints(indicesToRemove);
    }

    void IterativeGridGenerator::evalFunction(size_t oldGridSize) {
      const size_t d = f.getDimension();
      base::GridStorage& gridStorage = *grid.getStorage();
      const size_t curGridSize = gridStorage.size();
      base::DataVector& fX = functionValues;

      #pragma omp parallel shared(fX, oldGridSize, gridStorage) \
      default(none)
      {
        base::DataVector x(d);
        ObjectiveFunction* curFPtr = &f;
#ifdef _OPENMP
        std::unique_ptr<ObjectiveFunction> curF;

        if (omp_get_max_threads() > 1) {
          f.clone(curF);
          curFPtr = curF.get();
        }

#endif /* _OPENMP */

        #pragma omp for

        for (size_t i = oldGridSize; i < curGridSize; i++) {
          // convert grid point to coordinate vector
          #pragma omp critical
          {
            base::GridIndex& gp = *gridStorage.get(i);

            for (size_t t = 0; t < d; t++) {
              x[t] = gp.getCoord(t);
            }
          }

          float_t fx = curFPtr->eval(x);

          #pragma omp critical
          fX[i] = fx;
        }
      }
    }

  }
}
