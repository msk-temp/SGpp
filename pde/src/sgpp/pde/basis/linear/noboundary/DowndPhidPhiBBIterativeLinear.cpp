/******************************************************************************
* Copyright (C) 2010 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
******************************************************************************/
// @author Alexander Heinecke (Alexander.Heinecke@mytum.de

#include <sgpp/pde/basis/linear/noboundary/DowndPhidPhiBBIterativeLinear.hpp>
#include <sgpp/base/grid/common/BoundingBox.hpp>

namespace sg {
  namespace pde {

    DowndPhidPhiBBIterativeLinear::DowndPhidPhiBBIterativeLinear(sg::base::GridStorage* storage) : storage(storage) {
    }

    DowndPhidPhiBBIterativeLinear::~DowndPhidPhiBBIterativeLinear() {
    }

    void DowndPhidPhiBBIterativeLinear::operator()(sg::base::DataVector& alpha, sg::base::DataVector& result, size_t dim) {
      // Bounding Box handling
      sg::base::BoundingBox* boundingBox = this->storage->getBoundingBox();
      double q = boundingBox->getIntervalWidth(dim);
      double Qqout = 1.0 / q;

      // init the coefficients of the ansatz functions with boundary
      result.setAll(0.0);

      if (q != 1.0) {
        // traverse all basis function by sequence number
        for (size_t i = 0; i < storage->size(); i++) {
          sg::base::GridStorage::index_type::level_type level;
          sg::base::GridStorage::index_type::index_type index;
          (*storage)[i]->get(dim, level, index);
          //only affects the diagonal of the stiffness matrix
          result[i] = alpha[i] * (Qqout * (static_cast<double>(1 << (level + 1))));
        }
      } else {
        // traverse all basis function by sequence number
        for (size_t i = 0; i < storage->size(); i++) {
          sg::base::GridStorage::index_type::level_type level;
          sg::base::GridStorage::index_type::index_type index;
          (*storage)[i]->get(dim, level, index);
          //only affects the diagonal of the stiffness matrix
          result[i] = alpha[i] * static_cast<double>(1 << (level + 1));
        }
      }
    }

  }
}
