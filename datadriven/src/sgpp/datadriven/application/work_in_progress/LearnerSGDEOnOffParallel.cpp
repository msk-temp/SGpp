// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

//TODO: Delete this Definition after Refactoring
#define USE_GSL


#ifdef USE_GSL

#include <sgpp/base/exception/data_exception.hpp>
#include <sgpp/base/grid/generation/functors/SurplusCoarseningFunctor.hpp>
#include <sgpp/base/grid/generation/functors/SurplusRefinementFunctor.hpp>
#include <sgpp/base/grid/generation/hashmap/HashCoarsening.hpp>
#include <sgpp/base/operation/BaseOpFactory.hpp>
#include <sgpp/datadriven/algorithm/ConvergenceMonitor.hpp>
#include <sgpp/datadriven/algorithm/DBMatOfflineChol.hpp>
#include <sgpp/datadriven/algorithm/DBMatOfflineFactory.hpp>
#include <sgpp/datadriven/algorithm/DBMatOnlineDE.hpp>
#include <sgpp/datadriven/algorithm/DBMatOnlineDEFactory.hpp>
#include <sgpp/datadriven/functors/MultiGridRefinementFunctor.hpp>
#include <sgpp/datadriven/functors/classification/DataBasedRefinementFunctor.hpp>
#include <sgpp/datadriven/functors/classification/ZeroCrossingRefinementFunctor.hpp>
#include <sgpp/datadriven/application/work_in_progress/MPIMethods.hpp>


#include <chrono>

using sgpp::base::Grid;
using sgpp::base::GridStorage;
using sgpp::base::GridGenerator;
using sgpp::base::OperationEval;
using sgpp::base::data_exception;
using sgpp::base::SurplusRefinementFunctor;


namespace sgpp {
    namespace datadriven {



        LearnerSGDEOnOffParallel::LearnerSGDEOnOffParallel(
                sgpp::datadriven::DBMatDensityConfiguration &dconf,
                Dataset &trainData,
                Dataset &testData,
                Dataset *validationData,
                sgpp::base::DataVector &classLabels,
                size_t numClassesInit,
                bool usePrior, double beta, double lambda)
                : LearnerSGDEOnOff(dconf, trainData, testData, validationData, classLabels, numClassesInit, usePrior,
                                   beta, lambda) {

            init(lambda);


        }

        void LearnerSGDEOnOffParallel::init(double lambda) {
            MPIMethods::initMPI();
        }

        void LearnerSGDEOnOffParallel::train(size_t batchSize, size_t maxDataPasses,
                                             std::string refinementFunctorType, std::string refMonitor,
                                             size_t refPeriod, double accDeclineThreshold,
                                             size_t accDeclineBufferSize, size_t minRefInterval,
                                             bool enableCv, size_t nextCvStep) {
            // counts total number of processed data points
            size_t numProcessedDataPoints = 0;

            // contains list of removed grid points and number of added grid points
            // (is updated in each refinement/coarsening step)
            std::vector<RefinementResult> *vectorRefinementResults =
                    new std::vector<RefinementResult>(numClasses);

            // initialize counter for dataset passes
            size_t completedDataPasses = 0;

            // initialize refinement variables
            double currentValidError = 0.0;
            double currentTrainError = 0.0;
            // create convergence monitor object
            ConvergenceMonitor monitor{accDeclineThreshold, accDeclineBufferSize, minRefInterval};

            // counts number of performed refinements
            size_t numberOfCompletedRefinements = 0;

            // coarsening
            // size_t coarseCnt = 0;
            // size_t maxCoarseNum = 5;
            // size_t coarsePeriod = 50;
            // size_t coarseNumPoints = 1;
            // double coarseThreshold = 1.0;

            auto &onlineObjects = getDensityFunctions();

            size_t dim = trainData.getDimension();

            // determine number of batches to process
            size_t numBatch = trainData.getNumberInstances() / batchSize;

            // pointer to the next batch (data points + class labels) to be processed
            Dataset dataBatch(batchSize, dim);

            // print initial grid size
            printGridSizeStatistics(onlineObjects, "#Initial grid size of grid ");

            // auxiliary variable for accuracy (error) measurement
            double acc = getAccuracy();

            avgErrors.append(1.0 - acc);

            // main loop which performs the training process
            while (completedDataPasses < maxDataPasses) {
                std::cout << "#batch-size: " << batchSize << std::endl;
                std::cout << "#batches to process: " << numBatch << std::endl;

                // data point counter - determines offset when selecting next batch
                size_t cnt = 0;

                // iterate over total number of batches
                for (size_t currentBatchNum = 1; currentBatchNum <= numBatch; currentBatchNum++) {
                    std::cout << "#processing batch: " << currentBatchNum << "\n";

                    auto begin = std::chrono::high_resolution_clock::now();

                    // check if cross-validation should be performed
                    bool doCrossValidation = false;
                    if (enableCv) {
                        if (nextCvStep == currentBatchNum) {
                            doCrossValidation = true;
                            nextCvStep *= 5;
                        }
                    }
                    // assemble next batch
                    assembleNextBatchData(batchSize, &dataBatch, dim, &cnt);

                    // train the model with current batch
                    train(dataBatch, doCrossValidation, vectorRefinementResults);

                    numProcessedDataPoints += dataBatch.getData().getNrows();

                    // access DBMatOnlineDE-objects of all classes in order
                    // to apply adaptivity to the specific sparse grids later on
                    //TODO: check if this is necessary
//                    onlineObjects = getDensityFunctions();

                    // Refinement only occurs on the Master Node
                    if (MPIMethods::isMaster()) {

                        // check if refinement should be performed
                        if (checkRefinementNecessary(refMonitor, refPeriod, numProcessedDataPoints, currentValidError,
                                                     currentTrainError, numberOfCompletedRefinements, monitor)) {
                            // if the Cholesky decomposition is chosen as factorization method
                            // refinement
                            // and coarsening methods can be applied

                            std::cout << "refinement at iteration: " << numProcessedDataPoints << std::endl;
                            doRefinementForAll(refinementFunctorType, refMonitor, vectorRefinementResults,
                                               onlineObjects, monitor);
                            numberOfCompletedRefinements += 1;

                            //TODO If not master, the grid needs to be adjusted here
                            if (MPIMethods::isMaster()) {
                                //TODO Send and Receive Delta
                                //TODO Adjust Grid
                                MPIMethods::sendGridComponentsUpdate(vectorRefinementResults);
                            }
                        }


                    } else {
                        //TODO: Otherwise, merge grids if any are available
                    }

                    // save current error
                    if (numProcessedDataPoints % 10 == 0) {
                        acc = getAccuracy();
                        avgErrors.append(1.0 - acc);
                    }

                    auto end = std::chrono::high_resolution_clock::now();
                    std::cout << "Processing batch in "
                              << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
                              << "ms" << std::endl;

                }

                // Synchronize end of Data Pass
                MPIMethods::synchronizeEndOfDataPass();

                completedDataPasses++;
                processedPoints = 0;
            }  // end while

            std::cout << "#Training finished" << std::endl;

            error = 1.0 - getAccuracy();

            // delete offline;
            delete vectorRefinementResults;
        }

        void LearnerSGDEOnOffParallel::printGridSizeStatistics(
                ClassDensityContainer &onlineObjects,
                const char *messageString) {
            // print initial grid size
            for (auto &onlineObject : onlineObjects) {
                auto densEst = onlineObject.first.get();
                Grid &grid = densEst->getOfflineObject().getGrid();
                printf(messageString, onlineObject.second, grid.getSize());
            }
        }

        void LearnerSGDEOnOffParallel::doRefinementForAll(const std::string &refinementFunctorType,
                                                          const std::string &refinementMonitorType,
                                                          std::vector<RefinementResult> *vectorRefinementResults,
                                                          const ClassDensityContainer &onlineObjects,
                                                          ConvergenceMonitor &monitor) {
            // acc = getAccuracy();
            // avgErrors.append(1.0 - acc);

            // bundle grids and surplus vector pointer needed for refinement
            // (for zero-crossings refinement, data-based refinement)
            std::vector<Grid *> grids;
            std::vector<DataVector *> alphas;
            for (size_t i = 0; i < getNumClasses(); i++) {
                auto densEst = onlineObjects[i].first.get();
                grids.push_back(&(densEst->getOfflineObject().getGrid()));
                alphas.push_back(&(densEst->getAlpha()));
            }
            bool levelPenalize = false;  // Multiplies penalzing term for fine levels
            bool preCompute = true;      // Precomputes and caches evals for zrcr
            MultiGridRefinementFunctor *func = nullptr;

            // Zero-crossing-based refinement
            ZeroCrossingRefinementFunctor funcZrcr{grids, alphas, offline->getConfig().ref_noPoints_,
                                                   levelPenalize, preCompute};

            // Data-based refinement. Needs a problem dependent coeffA. The values
            // can be determined by testing (aim at ~10 % of the training data is
            // to be marked relevant). Cross-validation or similar can/should be
            // employed
            // to determine this value.
            //TODO: Investigate this
            std::vector<double> coeffA;
            coeffA.push_back(1.2);  // ripley 1.2
            coeffA.push_back(1.2);  // ripley 1.2
            DataMatrix *trainDataRef = &(trainData.getData());
            DataVector *trainLabelsRef = &(trainData.getTargets());
            DataBasedRefinementFunctor funcData = DataBasedRefinementFunctor{
                    grids, alphas, trainDataRef, trainLabelsRef, offline->getConfig().ref_noPoints_,
                    levelPenalize, coeffA};

            if (refinementFunctorType == "zero") {
                func = &funcZrcr;
            } else if (refinementFunctorType == "data") {
                func = &funcData;
            }

            // perform refinement/coarsening for each grid
            for (size_t idx = 0; idx < this->getNumClasses(); idx++) {
                this->doRefinementForClass(refinementFunctorType,
                                           &((*vectorRefinementResults)[idx]),
                                           onlineObjects,
                                           preCompute, func, idx);
            }
            if (refinementMonitorType == "convergence") {
                monitor.nextRefCnt = monitor.minRefInterval;
            }
        }

        void LearnerSGDEOnOffParallel::doRefinementForClass(const std::string &refType,
                                                            RefinementResult *refinementResult,
                                                            const ClassDensityContainer &onlineObjects,
                                                            bool preCompute,
                                                            MultiGridRefinementFunctor *refinementFunctor,
                                                            size_t classIndex) {
            // perform refinement/coarsening for grid which corresponds to current
            // index
            std::cout << "Refinement and coarsening for class: " << classIndex
                      << std::endl;
            auto densEst = onlineObjects[classIndex].first.get();
            Grid &grid = densEst->getOfflineObject().getGrid();
            std::cout << "Size before adaptivity: " << grid.getSize()
                      << std::endl;

            base::GridGenerator &gridGen = grid.getGenerator();

            size_t oldGridSize = grid.getSize();
            size_t numberOfNewPoints = 0;

            if (refType == "surplus") {
                numberOfNewPoints = handleSurplusBasedRefinement(densEst, grid, gridGen);

            } else if ((refType == "data") || (refType == "zero")) {
                numberOfNewPoints = handleDataAndZeroBasedRefinement(preCompute, refinementFunctor, classIndex, grid,
                                                                     gridGen);
            }

            std::cout << "grid size after adaptivity: " << grid.getSize()
                      << std::endl;


            //Collect new grid points into the refinement result for shipping
            for (int i = 0; i < numberOfNewPoints; i++) {
                sgpp::base::DataVector dataVector;
                //TODO: Replace absolute coordinate with level-index vectors
                grid.getStorage()[oldGridSize + i].getStandardCoordinates(dataVector);
                refinementResult->addedGridPoints.push_back(dataVector);
            }

            updateVariablesAfterRefinement(refinementResult, classIndex, densEst);
        }

        void LearnerSGDEOnOffParallel::updateVariablesAfterRefinement(
                RefinementResult *refinementResult,
                size_t classIndex,
                DBMatOnlineDE *densEst) {

            base::Grid &grid = densEst->getOfflineObject().getGrid();

            if (!MPIMethods::isMaster()) {
                //TODO: Modify the actual grid
                //Delete the grid points removed on master thread
                grid.getStorage().deletePoints(refinementResult->deletedGridPointsIndexes);

                //Add grid points added on master thread
                for (sgpp::base::DataVector dataVector : refinementResult->addedGridPoints) {
                    sgpp::base::GridPoint gridPoint;
                    //TODO: What happens when other points are changed (ie Leaf boolean etc)
//                    gridPoint.;

                    grid.getStorage().insert(gridPoint);
                }
            }

            // apply grid changes to the Cholesky factorization
            if (offline->isRefineable()) {
                dynamic_cast<DBMatOfflineChol &>(densEst->getOfflineObject())
                        .choleskyModification(refinementResult->addedGridPoints.size(),
                                              refinementResult->deletedGridPointsIndexes, densEst->getBestLambda());
            }
            // update alpha vector
            densEst->updateAlpha(&(refinementResult->deletedGridPointsIndexes),
                                 refinementResult->addedGridPoints.size());
        }

        void LearnerSGDEOnOffParallel::assembleNextBatchData(size_t batchSize,
                                                             Dataset *dataBatch,
                                                             size_t dataDimensionality,
                                                             size_t *batchOffset) const {
            printf("Assembling batch of size %zu at offset %zu\n", batchSize, *batchOffset);

            for (size_t j = 0; j < batchSize; j++) {
                base::DataVector dataPoint(dataDimensionality);
                trainData.getData().getRow(j + *batchOffset, dataPoint);
                double y = trainData.getTargets().get(j + *batchOffset);
                dataBatch->getData().setRow(j, dataPoint);
                dataBatch->getTargets().set(j, y);
            }
            *batchOffset += batchSize;

            printf("Assembled batch\n");
        }

        bool LearnerSGDEOnOffParallel::checkRefinementNecessary(const std::string &refMonitor, size_t refPeriod,
                                                                size_t totalInstances, double currentValidError,
                                                                double currentTrainError,
                                                                size_t numberOfCompletedRefinements,
                                                                ConvergenceMonitor &monitor) {

            // access DBMatOnlineDE-objects of all classes in order
            // to apply adaptivity to the specific sparse grids later on

            // check if refinement should be performed
            if (refMonitor == "periodic") {
                // check periodic monitor
                if (offline->isRefineable() && (totalInstances > 0) && (totalInstances % refPeriod == 0) &&
                    (numberOfCompletedRefinements < offline->getConfig().numRefinements_)) {
                    return true;
                }
            } else if (refMonitor == "convergence") {
                // check convergence monitor
                if (validationData == nullptr) {
                    throw data_exception("No validation data for checking convergence provided!");
                }
                if (offline->isRefineable() && (numberOfCompletedRefinements < offline->getConfig().numRefinements_)) {
                    currentValidError = getError(*validationData);
                    currentTrainError = getError(trainData);  // if train dataset is large
                    // use a subset for error
                    // evaluation
                    monitor.pushToBuffer(currentValidError, currentTrainError);
                    if (monitor.nextRefCnt > 0) {
                        monitor.nextRefCnt--;
                    }
                    if (monitor.nextRefCnt == 0) {
                        return monitor.checkConvergence();
                    }
                }
            }
            return false;
        }

        size_t
        LearnerSGDEOnOffParallel::handleDataAndZeroBasedRefinement(bool preCompute, MultiGridRefinementFunctor *func,
                                                                   size_t idx, base::Grid &grid,
                                                                   base::GridGenerator &gridGen) const {
            if (preCompute) {
                // precompute the evals (needs to be done once per step, before
                // any refinement is done
                func->preComputeEvaluations();
            }
            func->setGridIndex(idx);
            // perform refinement (zero-crossings-based / data-based)
            size_t gridSizeBeforeRefine = grid.getSize();
            gridGen.refine(*func);
            size_t gridSizeAfterRefine = grid.getSize();
            return gridSizeAfterRefine - gridSizeBeforeRefine;
        }

        size_t LearnerSGDEOnOffParallel::handleSurplusBasedRefinement(DBMatOnlineDE *densEst, base::Grid &grid,
                                                                      base::GridGenerator &gridGen) const {
            DataVector *alphaWork;  // required for surplus refinement
            // auxiliary variables
            DataVector p(trainData.getDimension());


            std::unique_ptr<OperationEval> opEval(op_factory::createOperationEval(grid));
            GridStorage &gridStorage = grid.getStorage();
            alphaWork = &(densEst->getAlpha());
            DataVector alphaWeight(alphaWork->getSize());
            // determine surpluses
            for (size_t k = 0; k < gridStorage.getSize(); k++) {
                // sets values of p to the coordinates of the given GridPoint gp
                gridStorage.getPoint(k).getStandardCoordinates(p);
                // multiply k-th alpha with the evaluated function at grind-point
                // k
                alphaWeight[k] = alphaWork->get(k) * opEval->eval(*alphaWork, p);
            }

            // Perform Coarsening (surplus based)
            /*if (coarseCnt < maxCoarseNum) {
              HashCoarsening coarse_;
              //std::cout << "\n" << "Start coarsening\n";

              // Coarsening based on surpluses
              SurplusCoarseningFunctor scf(
                alphaWeight, coarseNumPoints, coarseThreshold);

              //std::cout << "Size before coarsening:" << grid->getSize() <<
            "\n";
              //int old_size = grid->getSize();
              coarse_.free_coarsen_NFirstOnly(
                grid->getStorage(), scf, alphaWeight, grid->getSize());

              std::cout << "Size after coarsening:" << grid->getSize() <<
            "\n\n";
              //int new_size = grid->getSize();

              deletedGridPoints.clear();
              deletedGridPoints = coarse_.getDeletedPoints();

              (*refineCoarse)[idx].first = deletedGridPoints;

              coarseCnt++;
            }*/

            // perform refinement (surplus based)
            size_t sizeBeforeRefine = grid.getSize();
            // simple refinement based on surpluses
            SurplusRefinementFunctor srf(alphaWeight, offline->getConfig().ref_noPoints_);
            gridGen.refine(srf);
            size_t sizeAfterRefine = grid.getSize();
            return sizeAfterRefine - sizeBeforeRefine;
        }

        // Train from an entire Batch
        void LearnerSGDEOnOffParallel::train(Dataset &dataset, bool doCrossValidation,
                                             std::vector<RefinementResult> *vectorRefinementResults) {

            size_t dim = dataset.getDimension();


            // create an empty matrix for every class:
            std::vector<std::unique_ptr<DataMatrix>> classData;
            classData.reserve(classLabels.getSize());
            std::vector<std::pair<DataMatrix *, double>> trainDataClasses;
            trainDataClasses.reserve(classLabels.getSize());

            std::map<double, int> classIndices;  // maps class labels to indices

            allocateClassMatrices(dim, trainDataClasses, classIndices);

            // split the data into the different classes:
            splitBatchIntoClasses(dataset, dim, trainDataClasses, classIndices);

            // compute density functions
            train(trainDataClasses, doCrossValidation, vectorRefinementResults);

        }

        void
        LearnerSGDEOnOffParallel::splitBatchIntoClasses(const Dataset &dataset,
                                                        size_t dim,
                                                        const std::vector<std::pair<DataMatrix *, double>> &trainDataClasses,
                                                        std::map<double, int> &classIndices) const {
            // split the data into the different classes:
            for (size_t i = 0; i < dataset.getNumberInstances(); i++) {
                double classLabel = dataset.getTargets()[i];
                DataVector vec(dim);
                dataset.getData().getRow(i, vec);
                auto &p = trainDataClasses[classIndices[classLabel]];
                p.first->appendRow(vec);
            }
        }

        void LearnerSGDEOnOffParallel::allocateClassMatrices(size_t dim,
                                                             std::vector<std::pair<base::DataMatrix *, double>> &trainDataClasses,
                                                             std::map<double, int> &classIndices) const {
            int index = 0;
            for (size_t i = 0; i < classLabels.getSize(); i++) {
                base::DataMatrix *m = new base::DataMatrix(0, dim);
                std::pair<base::DataMatrix *, double> p(m, classLabels[i]);
                trainDataClasses.push_back(p);
                classIndices.insert(std::pair<double, int>(classLabels[i], index));
                index++;
            }
        }

        //Train from a Batch already split up into its classes
        void LearnerSGDEOnOffParallel::train(
                std::vector<std::pair<sgpp::base::DataMatrix *, double> > &trainDataClasses,
                bool doCrossValidation,
                std::vector<RefinementResult> *vectorRefinementResults) {

            //Calculate the total number of data points
            size_t numberOfDataPoints = 0;
            for (size_t i = 0; i < trainDataClasses.size(); i++) {
                numberOfDataPoints += trainDataClasses[i].first->getSize();
            }

            // Learn from each Class
            for (size_t i = 0; i < trainDataClasses.size(); i++) {
                std::pair<sgpp::base::DataMatrix *, double> p = trainDataClasses[i];

                if ((*p.first).getNrows() > 0) {
                    // update density function for current class
                    densityFunctions[i].first->computeDensityFunction(
                            *p.first, true, doCrossValidation, &(*vectorRefinementResults)[i].deletedGridPointsIndexes,
                            (*vectorRefinementResults)[i].addedGridPoints.size());
                    (*vectorRefinementResults)[i].deletedGridPointsIndexes.clear();
                    (*vectorRefinementResults)[i].addedGridPoints.clear();

                    if (usePrior) {
                        this->prior[p.second] =
                                ((this->prior[p.second] * static_cast<double>(processedPoints)) +
                                 static_cast<double>(p.first->getSize())) /
                                (static_cast<double>(numberOfDataPoints) +
                                 static_cast<double>(processedPoints));
                    } else {
                        this->prior[p.second] = 1.;
                    }
                }
            }

            this->processedPoints += numberOfDataPoints;
            trained = true;
        }

    }  // namespace datadriven
}  // namespace sgpp

#endif /* USE_GSL */
