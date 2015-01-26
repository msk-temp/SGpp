/******************************************************************************
* Copyright (C) 2009 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
******************************************************************************/

#include <sgpp/base/grid/Grid.hpp>
#include <sgpp/base/grid/type/PolyGrid.hpp>

#include <sgpp/base/grid/generation/StandardGridGenerator.hpp>

#include <sgpp/base/exception/factory_exception.hpp>

#include <iostream>

namespace sg {
  namespace base {

    PolyGrid::PolyGrid(std::istream& istr) : Grid(istr), degree(1 << 16), basis_(NULL)  {
      istr >> degree;
    }

    PolyGrid::PolyGrid(size_t dim, size_t degree) : degree(degree), basis_(NULL)  {
      this->storage = new GridStorage(dim);
    }

    PolyGrid::~PolyGrid() {
    	if (basis_ != NULL){
    	    		delete basis_;
    	    	}
    }

    const char* PolyGrid::getType() {
      return "poly";
    }

    const SBasis& PolyGrid::getBasis(){
    	if (basis_ == NULL){
			basis_ = new SPolyBase(degree);
		}
		return *basis_;
	}

    size_t PolyGrid::getDegree() const {
      return this->degree;
    }

    Grid* PolyGrid::unserialize(std::istream& istr) {
      return new PolyGrid(istr);
    }

    void PolyGrid::serialize(std::ostream& ostr) {
      this->Grid::serialize(ostr);
      ostr << degree << std::endl;
    }

    /**
     * Creates new GridGenerator
     * This must be changed if we add other storage types
     */
    GridGenerator* PolyGrid::createGridGenerator() {
      return new StandardGridGenerator(this->storage);
    }

  }
}
