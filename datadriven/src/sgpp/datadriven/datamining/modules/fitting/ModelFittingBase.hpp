// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#pragma once

#include <sgpp/base/datatypes/DataMatrix.hpp>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/grid/Grid.hpp>
#include <sgpp/base/operation/hash/OperationMatrix.hpp>
#include <sgpp/datadriven/application/RegularizationConfiguration.hpp>
#include <sgpp/datadriven/tools/Dataset.hpp>
#include <sgpp/globaldef.hpp>

namespace sgpp {

using sgpp::base::OperationMatrix;
using sgpp::base::Grid;
using sgpp::base::DataMatrix;
using sgpp::base::DataVector;
using sgpp::base::RegularGridConfiguration;

namespace datadriven {

enum class FittingSolverState { refine, solve };

class ModelFittingBase {
 public:
  ModelFittingBase();

  virtual ~ModelFittingBase();

  /// new grid and new data set
  virtual void fit(Dataset& dataset) = 0;

  /// reuse the grid and assume old data set
  virtual void refine() = 0;

  /// reuse grid and new data set
  virtual void update(Dataset& dataset) = 0;

  /**
   *
   * @param sample
   * @return
   */
  virtual double evaluate(DataVector& sample);

  /**
   *
   * @param samples
   * @param result
   * @return
   */
  virtual void evaluate(DataMatrix& samples, DataVector& results) const;

  virtual const Grid& getGrid() const;
  virtual const DataVector& getSurpluses() const;

 protected:
  OperationMatrix* getRegularizationMatrix(RegularizationType regType);
  void initializeGrid(RegularGridConfiguration gridConfig);

  std::shared_ptr<Grid> grid;
  std::shared_ptr<DataVector> alpha;
};

} /* namespace datadriven */
} /* namespace sgpp */
