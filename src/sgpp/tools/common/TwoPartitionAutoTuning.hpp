/******************************************************************************
* Copyright (C) 2011 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
******************************************************************************/
// @author Alexander Heinecke (Alexander.Heinecke@mytum.de)

#ifndef TWOPARTIONAUTOTUNING_HPP
#define TWOPARTIONAUTOTUNING_HPP

#include <cstddef>

namespace sg
{
namespace base
{

/**
 * This class provides functionality to split a workload range
 * into two parts based on timing results from previous executions. The
 * second partition is forced to be dividable by a given divider.
 *
 * For instance this class is used in datadriven application for solving
 * systems of linear equations concurrently on hybrid platforms using standard
 * CPUs and accelerators.
 */
class TwoPartitionAutoTuning
{
public:
	/**
	 * Constructor
	 *
	 * @param problemSize contains the overall size which should be partitioned
	 * @param partition2Divider the second partition
	 * @param retune_cycles number of iteration after which the problem's separation is re-considered
	 * @oaram damping damping factor to shade OS issues which might influence time measurements
	 * @param maxPercent max. percentage range into which the partition size should be changed during rescheduling
	 */
	TwoPartitionAutoTuning(size_t problemSize, size_t partition2Divider, size_t retune_cycles, double damping, double maxPercent);

	/**
	 * Destructor
	 */
	virtual ~TwoPartitionAutoTuning();

	/**
	 * get problem size
	 *
	 * @return problem size to should partitioned
	 */
	size_t getProblemSize();

	/**
	 * gets size of partition 1 based on the currently stored
	 * runtimes for partition 1 and 2
	 *
	 * @return size of partition 1
	 */
	virtual size_t getPartition1Size();

	/**
	 * Update execution times in order to allow
	 * a new calculation of the partition sizes
	 *
	 * @param timePartition1 time needed for partition 1
	 * @param timePartition2 time needed for partition 2
	 */
	void setExecutionTimes(double timePartition1, double timePartition2);

	/**
	 * sets the problem size
	 *
	 * @param problemSize problem size
	 */
	void setProblemSize(size_t problemSize);

	/**
	 * set the possible divider of partition 2
	 *
	 * @param partition2Divider the divider of partition 2
	 */
	void setPartition2Divider(size_t partition2Divider);

	void resetAutoTuning();

	void softResetAutoTuning();

protected:
	/// store problemsize
	size_t _problemSize;

	/// store required divider of partition 2
	size_t _partition2Divider;

	/// time needed to execute partition 1
	double _timePartition1;

	/// time needed to execute partition 2
	double _timePartition2;

	/// (old) size of partition1
	size_t _oldSizePartition1;

	bool _testPartition1;
	bool _testPartition2;
	bool _isFirstTuning;

	size_t _tuneCounter;

	size_t _retune;

	double _damping;

	double _maxPercent;

	void rescaleAutoTuning(size_t newProblemSize);
};

}

}

#endif /* TWOPARTIONAUTOTUNING_HPP */