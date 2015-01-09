/* ****************************************************************************
 * Copyright (C) 2013 Technische Universitaet Muenchen                         *
 * This file is part of the SG++ project. For conditions of distribution and   *
 * use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
 **************************************************************************** */
// @author Alexander Heinecke (Alexander.Heinecke@mytum.de)
// @author Roman Karlstetter (karlstetter@mytum.de)
#ifndef ABSTRACTOPERATIONMULTIPLEEVALSUBSPACE_HPP
#define ABSTRACTOPERATIONMULTIPLEEVALSUBSPACE_HPP

#include "../OperationMultipleEvalSubspace/CommonParameters.hpp"
#include "base/grid/Grid.hpp"
#include "base/grid/GridStorage.hpp"
#include "base/tools/SGppStopwatch.hpp"
#include "datadriven/tools/PartitioningTool.hpp"
#include "base/operation/OperationMultipleEval.hpp"

namespace sg {
namespace datadriven {

class AbstractOperationMultipleEvalSubspace: public base::OperationMultipleEval {
protected:
	//should be set to true to avoid called "prepare()" with every "mult()" or "multTranspose()"
	bool isPrepared;
	base::GridStorage *storage;
private:
	base::SGppStopwatch timer;
	double duration;

public:
	AbstractOperationMultipleEvalSubspace(base::Grid &grid, base::DataMatrix &dataset) :
			base::OperationMultipleEval(grid, dataset), isPrepared(false), storage(grid.getStorage()), duration(-1.0) {

	}

	~AbstractOperationMultipleEvalSubspace() {
	}

	virtual void prepareExecution(size_t gridFrom = 0, size_t gridTo = std::numeric_limits<size_t>::max()) {

		this->prepare();

		if (gridTo == std::numeric_limits<size_t>::max()) {
			gridTo = this->storage->size();
		}
	}

	//TODO is this function redundant?
	virtual void prepare() = 0;

	//TODO where and when to do the padding? (currently in multTranspose done and reverted)

	virtual void multImpl(base::DataVector &alpha, base::DataVector &result, const size_t start_index_data,
			const size_t end_index_data) = 0;

	virtual void multTransposeImpl(sg::base::DataVector &source, sg::base::DataVector &result,
			const size_t start_index_data, const size_t end_index_data) = 0;

	void multTranspose(sg::base::DataVector& alpha, sg::base::DataVector& result) override {
		if (!this->isPrepared) {
			this->prepareExecution();
		}

		size_t originalAlphaSize = alpha.getSize();

		const size_t start_index_data = 0;
		//TODO handle transposed matrix? or use transposed matrix?
		const size_t end_index_data = this->getPaddedDatasetSize();

		//pad the alpha vector to the padded size of the dataset
		alpha.resizeZero(this->getPaddedDatasetSize());

		this->timer.start();
		result.setAll(0.0);

#pragma omp parallel
		{
			size_t start;
			size_t end;
			PartitioningTool::getOpenMPPartitionSegment(start_index_data, end_index_data, &start, &end,
					this->getAlignment());
			this->multTransposeImpl(alpha, result, start, end);
		}

		alpha.resize(originalAlphaSize);
		this->duration = this->timer.stop();
	}

	//TODO assumes padded "result" vector -> document somewhere or improve eval interface
	void mult(sg::base::DataVector& source, sg::base::DataVector& result) override {

		if (!this->isPrepared) {
			this->prepareExecution();
		}

		size_t originalResultSize = result.getSize();
		result.resizeZero(this->getPaddedDatasetSize());

		const size_t start_index_data = 0;
		const size_t end_index_data = this->getPaddedDatasetSize();

		this->timer.start();
		result.setAll(0.0);

#pragma omp parallel
		{
			size_t start;
			size_t end;
			PartitioningTool::getOpenMPPartitionSegment(start_index_data, end_index_data, &start, &end,
					this->getAlignment());
			this->multImpl(source, result, start, end);
		}

		result.resize(originalResultSize);

		this->duration = this->timer.stop();
	}

	virtual size_t getPaddedDatasetSize() {
		return this->dataset.getNrows();
	}

	virtual size_t getAlignment() = 0;

	static inline size_t getChunkGridPoints() {
		return 12;
	}
	static inline size_t getChunkDataPoints() {
		return 24; // must be divisible by 24
	}

};

}
}

#endif // ABSTRACTOPERATIONMULTIPLEEVALSUBSPACE_HPP