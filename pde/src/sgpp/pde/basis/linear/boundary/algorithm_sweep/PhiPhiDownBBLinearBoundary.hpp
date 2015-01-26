/* ****************************************************************************
* Copyright (C) 2009 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
**************************************************************************** */
// @author Alexander Heinecke (Alexander.Heinecke@mytum.de)

#ifndef PHIPHIDOWNBBLINEARBOUNDARY_HPP
#define PHIPHIDOWNBBLINEARBOUNDARY_HPP

#include <sgpp/base/grid/GridStorage.hpp>
#include <sgpp/base/datatypes/DataVector.hpp>

#include <sgpp/pde/basis/linear/noboundary/algorithm_sweep/PhiPhiDownBBLinear.hpp>

namespace sg {
  namespace pde {



    /**
     * Implementation of sweep operator (): 1D Down for
     * Bilinearform \f$\int_{x} \phi(x) \phi(x) dx\f$
     * on linear boundary grids
     */
    class PhiPhiDownBBLinearBoundary : public PhiPhiDownBBLinear {
      public:
        /**
         * Constructor
         *
         * @param storage the grid's sg::base::GridStorage object
         */
        PhiPhiDownBBLinearBoundary(sg::base::GridStorage* storage);

        /**
         * Destructor
         */
        virtual ~PhiPhiDownBBLinearBoundary();

        /**
         * This operations performs the calculation of down in the direction of dimension <i>dim</i>
         *
         * For level zero it's assumed, that both ansatz-functions do exist: 0,0 and 0,1
         * If one is missing this code might produce some bad errors (segmentation fault, wrong calculation
         * result)
         * So please assure that both functions do exist!
         *
         * On level zero the getfixDirechletBoundaries of the storage object evaluated
         *
         * @param source sg::base::DataVector that contains the gridpoint's coefficients (values from the vector of the laplace operation)
         * @param result sg::base::DataVector that contains the result of the down operation
         * @param index a iterator object of the grid
         * @param dim current fixed dimension of the 'execution direction'
         */
        virtual void operator()(sg::base::DataVector& source, sg::base::DataVector& result, grid_iterator& index, size_t dim);
    };

    // namespace detail

  } // namespace sg
}

#endif /* PHIPHIDOWNBBLINEARBOUNDARY_HPP */
