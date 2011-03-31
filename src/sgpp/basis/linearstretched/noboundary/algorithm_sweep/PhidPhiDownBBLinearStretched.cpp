/******************************************************************************
* Copyright (C) 2011 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
******************************************************************************/
// @author Alexander Heinecke (Alexander.Heinecke@mytum.de), Stefanie Schraufstetter (schraufs@in.tum.de), Sarpkan Selcuk (Sarpkan.Selcuk@mytum.de)


#include "basis/linearstretched/noboundary/algorithm_sweep/PhidPhiDownBBLinearStretched.hpp"

namespace sg
{

namespace detail
{

PhidPhiDownBBLinearStretched::PhidPhiDownBBLinearStretched(GridStorage* storage) : storage(storage), stretching(storage->getStretching())
{
}

PhidPhiDownBBLinearStretched::~PhidPhiDownBBLinearStretched()
{
}

void PhidPhiDownBBLinearStretched::operator()(DataVector& source, DataVector& result, grid_iterator& index, size_t dim)
{
	rec(source, result, index, dim, 0.0, 0.0);
}

void PhidPhiDownBBLinearStretched::rec(DataVector& source, DataVector& result, grid_iterator& index, size_t dim, double fl, double fr)
{
	//Fix the eqn (Selcuk)
	size_t seq = index.seq();

	double alpha_value = source[seq];

	GridStorage::index_type::level_type l;
	GridStorage::index_type::index_type i;

	index.get(dim, l, i);

	double posl=0, posr=0, posc=0;
	this->stretching->getAdjacentPositions(static_cast<int>(l), static_cast<int>(i), dim, posc, posl, posr );

	// integration
	result[seq] = (  0.5*(fl -fr) );    // diagonal entry = 0.0

	// dehierarchisation
	double fm  = (fr-fl)*(posc-posl)/(posr-posl)+fl + alpha_value;

	if(!index.hint())
	{
		index.left_child(dim);
		if(!storage->end(index.seq()))
		{
			rec(source, result, index, dim, fl, fm);
		}

		index.step_right(dim);
		if(!storage->end(index.seq()))
		{
			rec(source, result, index, dim, fm, fr);
		}

		index.up(dim);
	}
}

} // namespace detail

} // namespace sg