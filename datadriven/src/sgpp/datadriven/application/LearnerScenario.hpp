// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#pragma once

#include <string>

#include "sgpp/globaldef.hpp"
#include "sgpp/base/grid/Grid.hpp"
#include "sgpp/solver/TypesSolver.hpp"

namespace SGPP {
namespace datadriven {

class LearnerScenario {
 private:
  bool isInitialized;

  // variables for the scenario
  double lambda;
  std::string datasetFileName;
  SGPP::base::RegularGridConfiguration gridConfig;
  SGPP::solver::SLESolverConfiguration solverConfigRefine;
  SGPP::solver::SLESolverConfiguration solverConfigFinal;
  SGPP::base::AdpativityConfiguration adaptConfig;

 public:
  LearnerScenario();

  explicit LearnerScenario(std::string scenarioFileName);

  LearnerScenario(std::string datasetFileName, double lambda,
                  SGPP::base::RegularGridConfiguration gridConfig,
                  SGPP::solver::SLESolverConfiguration SLESolverConfigRefine,
                  SGPP::solver::SLESolverConfiguration SLESolverConfigFinal,
                  SGPP::base::AdpativityConfiguration adaptConfig);

  std::string getDatasetFileName();

  double getLambda();

  SGPP::base::RegularGridConfiguration getGridConfig();

  SGPP::solver::SLESolverConfiguration getSolverConfigurationRefine();
  SGPP::solver::SLESolverConfiguration getSolverConfigurationFinal();

  SGPP::base::AdpativityConfiguration getAdaptivityConfiguration();

  void writeToFile(std::string fileName);

  void readFromFile(std::string fileName);

 private:
  template <class T>
  T fromString(const std::string& s);
};
}  // namespace datadriven
}  // namespace SGPP
