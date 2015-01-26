#pragma once

#include <iostream>
#include <string>

#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/datatypes/DataMatrix.hpp>
#include <sgpp/base/grid/Grid.hpp>
#include <sgpp/base/grid/GridStorage.hpp>
#include <sgpp/base/grid/generation/GridGenerator.hpp>
#include <sgpp/datadriven/tools/ARFFTools.hpp>
#include <sgpp/base/operation/OperationMultipleEval.hpp>
#include <sgpp/datadriven/tools/TypesDatadriven.hpp>
#include <sgpp/datadriven/application/LearnerLeastSquaresIdentity.hpp>
#include <sgpp/solver/SLESolver.hpp>

#include "../operation/OperationMultipleEvalSubspace/CommonParameters.hpp"
#include <sgpp/datadriven/operation/DatadrivenOperationCommon.hpp>

namespace sg {
namespace datadriven {

class MetaLearner {
private:

    sg::datadriven::ARFFTools arffTools;
    size_t dim;
    size_t instances;
    size_t baseLevel;
    double lambda;

    std::string csvSep;

    bool verbose;

    LearnerBase *myLearner = nullptr;
    LearnerBase *referenceLearner = nullptr;

    sg::base::RegularGridConfiguration gridConfig;
    sg::solver::SLESolverConfiguration solverConfig;
    sg::solver::SLESolverConfiguration solverFinalStep;
    sg::base::AdpativityConfiguration adaptivityConfiguration;

    LearnerTiming myTiming;
    LearnerTiming referenceTiming;

    std::vector<std::pair<size_t, double> > ExecTimesOnStep;
    std::vector<std::pair<size_t, double> > ExecTimesOnStepReference;

    void writeRefinementResults(std::string fileName, std::string fileHeader,
                                std::vector<std::pair<std::string, std::vector<std::pair<size_t, double> > > > datasetDetails,
                                std::vector<std::pair<std::string, std::vector<std::pair<size_t, double> > > > datasetDetailsReference,
                                bool referenceComparison);

public:
    MetaLearner() = delete;

    MetaLearner(sg::solver::SLESolverConfiguration solverConfig, sg::solver::SLESolverConfiguration solverFinalStep,
                sg::base::AdpativityConfiguration adaptivityConfiguration, size_t baseLevel, double lambda, bool verbose =
                    false);

    ~MetaLearner() {
        if (this->myLearner != nullptr) {
            delete this->myLearner;
        }

        if (this->referenceLearner != nullptr) {
            delete this->referenceLearner;
        }
    }

    void learn(sg::datadriven::OperationMultipleEvalConfiguration &operationConfiguration, std::string datasetFileName);

    void learnReference(std::string fileName);

    //learn and test against test dataset and measure hits/mse
    void learnAndTest(sg::datadriven::OperationMultipleEvalConfiguration &operationConfiguration,
                      std::string datasetFileName, std::string testFileName, bool isBinaryClassification);

    //learn and test against the streaming implementation
    double learnAndCompare(sg::datadriven::OperationMultipleEvalConfiguration &operationConfiguration,
                           std::string datasetFileName, size_t gridGranularity, double tolerance);

    void refinementAndOverallPerformance(
        std::vector<sg::datadriven::OperationMultipleEvalConfiguration *> operationConfigurations,
        std::vector<std::string> datasets, std::vector<std::string> experimentHeaders, std::string metaInformation,
        std::string fileName, bool referenceComparison = false);

    void regularGridSpeedup(sg::datadriven::OperationMultipleEvalConfiguration &operationConfiguration,
                            std::vector<size_t> dimList, std::vector<size_t> levelList, size_t instances, std::string metaInformation,
                            std::string experimentName);

    void appendToPerformanceRun(std::string fileName, std::string changingRowName, std::string currentValues,
                                std::vector<sg::datadriven::OperationMultipleEvalConfiguration *> operationConfigurations,
                                std::vector<std::string> datasets, std::vector<std::string> datasetNames, std::string metaInformation,
                                bool removeOld);

    void testRegular(sg::datadriven::OperationMultipleEvalConfiguration &operationConfiguration, size_t dim,
                     size_t level, size_t instances, double &duration, double &durationReference);
};

}
}
