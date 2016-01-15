/*
 * regressionByInterpolation.cpp
 *
 *  Created on: Dec 17, 2015
 *      Author: pfandedd
 */

#include <fstream>
#include <random>

#include <sgpp/globaldef.hpp>

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/grid/Grid.hpp>
#include <sgpp/base/grid/GridStorage.hpp>
#include <sgpp/base/grid/generation/GridGenerator.hpp>
#include <sgpp/base/operation/hash/OperationEval.hpp>
#include <sgpp/base/operation/BaseOpFactory.hpp>
#include <sgpp/datadriven/application/DensityRegressionMetaLearner.hpp>
#include <sgpp/datadriven/tools/ARFFTools.hpp>

using namespace SGPP::base;

// function to reconstruct
SGPP::float_t f(std::vector<SGPP::float_t> point) {
    return 16.0 * (point[0] - 1) * point[0] * (point[1] - 1) * point[1];
}

// function to reconstruct
SGPP::float_t f(SGPP::base::DataVector point) {
    return 16.0 * (point[0] - 1) * point[0] * (point[1] - 1) * point[1];
}

// function to reconstruct
SGPP::float_t f1D(SGPP::base::DataVector point) {
    return -4.0 * (point[0] - 1) * point[0];
}

//// function to reconstruct
//SGPP::SGPP::float_t f1D(SGPP::base::DataVector point) {
////    return point[0];
//    if (point[0] < 0.5) {
//        return 2.0 * point[0];
//    } else {
//        return -2.0 * (point[0] - 1.0);
//    }
//}

//// function to reconstruct
//SGPP::SGPP::float_t f1D(SGPP::base::DataVector point) {
//    return 0.0;
//}

int main(int argc, char **argv) {

//    size_t samplePoints = 1000;

//  DataMatrix dataset(0, dim);
//  DataVector values(samplePoints);

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<SGPP::float_t> dist(0.0, 1.0);

//  std::ofstream sampleFile;
//  sampleFile.open("sampleFile.csv");
//
//  for (size_t sample = 0; sample < samplePoints; sample++) {
//    DataVector point(dim);
//    for (size_t d = 0; d < dim; d++) {
//      point[d] = dist(mt);
//      sampleFile << point[d] << ", ";
//    }
//    dataset.appendRow(point);
//    if (dim == 1) {
//      values[sample] = f1D(point);
//    } else if (dim == 2) {
//      values[sample] = f(point);
//    } else {
//      throw;
//    }
//    sampleFile << values[sample] << std::endl;
//  }
//  sampleFile.close();

//    std::string fileName("parabel2d.arff");
//    std::string fileName("chess_02D_tr.dat.arff");
//    std::string fileName("friedman_4d.arff");
//    std::string fileName("chess_05D_3fields_tr.dat.arff");
    std::string fileName("chess_3d.arff");

    SGPP::datadriven::ARFFTools arffTools;
    SGPP::datadriven::Dataset arffDataset = arffTools.readARFF(fileName);

    SGPP::base::DataMatrix &dataset = arffDataset.getTrainingData();
    SGPP::base::DataVector &values = arffDataset.getClasses();

    int maxLevel = 6;

    SGPP::base::RegularGridConfiguration gridConfig;
    SGPP::solver::SLESolverConfiguration solverConfig;
    SGPP::base::AdpativityConfiguration adaptConfig;

    // setup grid
    gridConfig.dim_ = arffDataset.getDimension();
    gridConfig.level_ = maxLevel;
    gridConfig.type_ = SGPP::base::GridType::LinearBoundary;

    // Set Adaptivity
    adaptConfig.maxLevelType_ = false;
    adaptConfig.noPoints_ = 80;
    adaptConfig.numRefinements_ = 15;
    adaptConfig.percent_ = 200.0;
    adaptConfig.threshold_ = 0.0;

    // Set solver during refinement
    solverConfig.eps_ = 0;
    solverConfig.maxIterations_ = 50;
    solverConfig.threshold_ = -1.0;
    solverConfig.type_ = SGPP::solver::SLESolverType::CG;

    SGPP::pde::RegularizationConfiguration regularizationConfig;
    regularizationConfig.regType_ = SGPP::pde::RegularizationType::Laplace;

//  SGPP::float_t lambda = 0.1;
//  SGPP::float_t lambda = 0.0005;

    bool verbose = true;

    SGPP::datadriven::DensityRegressionMetaLearner learner(verbose, dataset, values, gridConfig, adaptConfig,
            solverConfig, regularizationConfig);

//  learner.optimizeLambdaCV(10, 0.0001, 0.001, 10);

//    SGPP::float_t lambdaOpt = learner.optimizeLambdaCVGreedy(10, 15, 0.25, 0.125);
    SGPP::float_t lambdaOpt = learner.optimizeLambdaCVGreedy(3, 10, 1E-3, 10.0);
//    SGPP::float_t lambdaOpt = learner.optimizeLambdaCVGreedy(3, 0, 1.7425e-05, 10.0);

    auto grid = std::shared_ptr<SGPP::base::Grid>(SGPP::base::Grid::createLinearGrid(arffDataset.getDimension()));

    auto generator = std::shared_ptr<SGPP::base::GridGenerator>(grid->createGridGenerator());
    generator->regular(maxLevel);

//    SGPP::base::DataVector alpha(grid->getSize());
    DataVector alpha = learner.train(*grid, dataset, values, lambdaOpt);

    if (arffDataset.getDimension() == 2) {

        SGPP::base::OperationEval *linearEval = SGPP::op_factory::createOperationEval(*grid);

#if USE_DOUBLE_PRECISION == 1
        SGPP::float_t pointIncrement = 0.01;
#else
        SGPP::float_t pointIncrement = 0.05f;
#endif
        std::ofstream resultFileLinear;
        resultFileLinear.open("resultFileLinear.csv");

        for (SGPP::float_t testX = 0; testX <= 1.0; testX += pointIncrement) {
            for (SGPP::float_t testY = 0; testY <= 1.0; testY += pointIncrement) {
//        std::vector<SGPP::float_t> point = { testX, testY };
                DataVector point(arffDataset.getDimension());
                point[0] = testX;
                point[1] = testY;
                for (size_t d = 0; d < arffDataset.getDimension(); d++) {
                    resultFileLinear << point[d] << ", ";
                }
                SGPP::float_t eval = linearEval->eval(alpha, point);
                resultFileLinear << eval << std::endl;
            }
            resultFileLinear << std::endl;
        }
        resultFileLinear.close();
    } else if (arffDataset.getDimension() == 3) {

        SGPP::base::OperationEval *linearEval = SGPP::op_factory::createOperationEval(*grid);

#if USE_DOUBLE_PRECISION == 1
        SGPP::float_t pointIncrement = 0.05;
#else
        SGPP::float_t pointIncrement = 0.05f;
#endif
        std::ofstream resultFileLinear;
        resultFileLinear.open("resultFileLinear3d.csv");

        for (SGPP::float_t testX = 0; testX <= 1.0; testX += pointIncrement) {
            for (SGPP::float_t testY = 0; testY <= 1.0; testY += pointIncrement) {
                for (SGPP::float_t testZ = 0; testZ <= 1.0; testZ += pointIncrement) {
//        std::vector<SGPP::float_t> point = { testX, testY };
                    DataVector point(arffDataset.getDimension());
                    point[0] = testX;
                    point[1] = testY;
                    point[2] = testZ;
                    for (size_t d = 0; d < arffDataset.getDimension(); d++) {
                        resultFileLinear << point[d] << ", ";
                    }
                    SGPP::float_t eval = linearEval->eval(alpha, point);
                    SGPP::float_t unscaledValue = eval;
                    //scale for easier printing
                    eval = eval + 1.0;
                    eval *= 255.0;
                    if (eval < 0.0)
                        eval = 0.0;
                    if (eval > 255.0)
                        eval = 255.0;

                    uint64_t asInteger = static_cast<uint64_t>(eval);
                    std::stringstream valueStream;
                    resultFileLinear << "0x";
                    if (asInteger < 16)
                        valueStream << "0";
                    valueStream << std::hex << asInteger << std::dec << "0000";
//                    std::cout << "valueStream: " << valueStream.str() << std::endl;
//                    if (valueStream.str().size() != 6) {
//                        std::cout << "original value: " << unscaledValue << std::endl;
//                        std::cout << "scaled value: " << eval << std::endl;
//                    }
                    resultFileLinear << valueStream.str() << "," << unscaledValue << std::endl;
//                    resultFileLinear << eval << std::endl;
//                    if (eval >= 0.0) {
//                        resultFileLinear << "0x0000FF" << std::endl;
//                    } else {
//                        resultFileLinear << "0x000000" << std::endl;
//                    }
                }
            }
//            resultFileLinear << std::endl;
        }
        resultFileLinear.close();
    }

    std::cout << "all done!" << std::endl;
    return 0;
}

