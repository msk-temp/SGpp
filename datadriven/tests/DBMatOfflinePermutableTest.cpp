// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <sgpp/base/grid/Grid.hpp>
#include <sgpp/datadriven/algorithm/DBMatOfflinePermutable.hpp>
#include <sgpp/datadriven/algorithm/GridFactory.hpp>

BOOST_AUTO_TEST_SUITE(DBMatOfflinePermutableTest)

class DBMatOfflineTest : public sgpp::datadriven::DBMatOfflinePermutable {
 public:
  DBMatOfflineTest() {}

  void permuteDecomposition(
      const sgpp::base::GeneralGridConfiguration& baseGridConfig,
      const sgpp::base::GeneralGridConfiguration& desiredGridConfig) override {}

  void decomposeMatrix(
      const sgpp::datadriven::RegularizationConfiguration& regularizationConfig,
      const sgpp::datadriven::DensityEstimationConfiguration& densityEstimationConfig) override {}

  DBMatOffline* clone() const { return new DBMatOfflineTest(); }

  bool isRefineable() override { return false; }

  sgpp::datadriven::MatrixDecompositionType getDecompositionType() override {
    return sgpp::datadriven::MatrixDecompositionType::Chol;
  }

  size_t test_getMatrixIndexForPoint(std::vector<size_t> level, std::vector<size_t> index,
                                     std::vector<size_t> gridLevel,
                                     const std::vector<size_t>& preComputations) {
    return this->getMatrixIndexForPoint(level, index, gridLevel, preComputations);
  }

  std::vector<size_t> test_preComputeMatrixIndexForPoint(std::vector<size_t> level) {
    return this->preComputeMatrixIndexForPoint(level);
  }
};

BOOST_AUTO_TEST_CASE(GetMatrixIndexForPointTest) {
  DBMatOfflineTest testObject;
  std::vector<size_t> pre =
      testObject.test_preComputeMatrixIndexForPoint(std::vector<size_t>{3, 2});
  BOOST_CHECK(testObject.test_getMatrixIndexForPoint(std::vector<size_t>{1, 1},
                                                     std::vector<size_t>{1, 1},
                                                     std::vector<size_t>{3, 2}, pre) == 1);
  BOOST_CHECK(testObject.test_getMatrixIndexForPoint(std::vector<size_t>{2, 1},
                                                     std::vector<size_t>{1, 1},
                                                     std::vector<size_t>{3, 2}, pre) == 2);
  BOOST_CHECK(testObject.test_getMatrixIndexForPoint(std::vector<size_t>{2, 2},
                                                     std::vector<size_t>{1, 3},
                                                     std::vector<size_t>{3, 2}, pre) == 11);
  BOOST_CHECK(testObject.test_getMatrixIndexForPoint(std::vector<size_t>{3, 2},
                                                     std::vector<size_t>{5, 1},
                                                     std::vector<size_t>{3, 2}, pre) == 18);
  BOOST_CHECK(testObject.test_getMatrixIndexForPoint(std::vector<size_t>{3, 2},
                                                     std::vector<size_t>{7, 3},
                                                     std::vector<size_t>{3, 2}, pre) == 21);
  pre = testObject.test_preComputeMatrixIndexForPoint(std::vector<size_t>{3, 2, 2});
  BOOST_CHECK(testObject.test_getMatrixIndexForPoint(std::vector<size_t>{2, 1, 1},
                                                     std::vector<size_t>{1, 1, 1},
                                                     std::vector<size_t>{3, 2, 2}, pre) == 2);
  BOOST_CHECK(testObject.test_getMatrixIndexForPoint(std::vector<size_t>{2, 2, 1},
                                                     std::vector<size_t>{1, 3, 1},
                                                     std::vector<size_t>{3, 2, 2}, pre) == 11);
  BOOST_CHECK(testObject.test_getMatrixIndexForPoint(std::vector<size_t>{1, 1, 2},
                                                     std::vector<size_t>{1, 1, 1},
                                                     std::vector<size_t>{3, 2, 2}, pre) == 22);
  BOOST_CHECK(testObject.test_getMatrixIndexForPoint(std::vector<size_t>{1, 2, 2},
                                                     std::vector<size_t>{1, 1, 1},
                                                     std::vector<size_t>{3, 2, 2}, pre) == 36);
  BOOST_CHECK(testObject.test_getMatrixIndexForPoint(std::vector<size_t>{3, 2, 2},
                                                     std::vector<size_t>{7, 3, 3},
                                                     std::vector<size_t>{3, 2, 2}, pre) == 63);
}

BOOST_AUTO_TEST_CASE(LhsMatrixPermutationTest) {
  DBMatOfflineTest baseTestObject;
  DBMatOfflineTest desiredTestObject;

  sgpp::datadriven::RegularizationConfiguration regConfig;
  regConfig.lambda_ = 0;

  sgpp::base::GeneralGridConfiguration baseConfig;
  baseConfig.generalType_ = sgpp::base::GeneralGridType::ComponentGrid;
  baseConfig.type_ = sgpp::base::GridType::Linear;
  baseConfig.dim_ = 3;
  baseConfig.levelVector_ = std::vector<size_t>{3, 2, 2};

  sgpp::base::GeneralGridConfiguration desiredConfig;
  desiredConfig.generalType_ = sgpp::base::GeneralGridType::ComponentGrid;
  desiredConfig.type_ = sgpp::base::GridType::Linear;
  desiredConfig.dim_ = 5;
  desiredConfig.levelVector_ = std::vector<size_t>{3, 2, 1, 1, 2};

  sgpp::datadriven::GridFactory gridFactory;
  std::vector<std::vector<size_t>> interactions = std::vector<std::vector<size_t>>();
  std::unique_ptr<sgpp::base::Grid> baseGrid{gridFactory.createGrid(baseConfig, interactions)};
  std::unique_ptr<sgpp::base::Grid> desiredGrid{
      gridFactory.createGrid(desiredConfig, interactions)};

  baseTestObject.buildMatrix(baseGrid.get(), regConfig);
  desiredTestObject.buildMatrix(desiredGrid.get(), regConfig);

  baseTestObject.permuteLhsMatrix(baseConfig, desiredConfig);

  for (size_t i = 0; i < baseTestObject.getLhsMatrix_ONLY_FOR_TESTING().getNrows(); i++) {
    for (size_t j = 0; j < baseTestObject.getLhsMatrix_ONLY_FOR_TESTING().getNcols(); j++) {
      BOOST_CHECK(std::abs(baseTestObject.getLhsMatrix_ONLY_FOR_TESTING().get(i, j) -
                           desiredTestObject.getLhsMatrix_ONLY_FOR_TESTING().get(i, j)) < 0.001);
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()