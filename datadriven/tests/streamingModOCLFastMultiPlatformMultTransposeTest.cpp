/*
 * multiEvalPerformance.cpp
 *
 *  Created on: Mar 12, 2015
 *      Author: pfandedd
 */

#if USE_OCL==1

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <random>
#include <fstream>
#include <iostream>

#include "test_datadrivenCommon.hpp"

#include <sgpp/globaldef.hpp>

#include <sgpp/base/operation/hash/OperationMultipleEval.hpp>
#include <sgpp/datadriven/DatadrivenOpFactory.hpp>
#include <sgpp/base/operation/BaseOpFactory.hpp>
#include <sgpp/datadriven/tools/ARFFTools.hpp>
#include <sgpp/base/tools/ConfigurationParameters.hpp>
#include <sgpp/base/opencl/OCLConfigurationParameters.hpp>

BOOST_AUTO_TEST_SUITE(TestStreamingModOCLFastMultiPlatformMultTranspose)

SGPP::base::OCLConfigurationParameters getConfigurationDefaults() {
    SGPP::base::OCLConfigurationParameters parameters;
    parameters.set("OCL_MANAGER_VERBOSE", "false");
    parameters.set("VERBOSE", "false");
    parameters.set("ENABLE_OPTIMIZATIONS", "true");
    parameters.set("PLATFORM", "first");
    parameters.set("SELECT_SPECIFIC_DEVICE", "0");
    return parameters;
}

BOOST_AUTO_TEST_CASE(Simple) {

    std::vector<std::string> fileNames = { "datadriven/tests/data/friedman_4d.arff.gz",
            "datadriven/tests/data/friedman_10d.arff.gz" };
    std::vector<double> errors = { 10E-14, 10E-14 };

    uint32_t level = 4;

    SGPP::base::OCLConfigurationParameters parameters = getConfigurationDefaults();
    parameters.set("KERNEL_USE_LOCAL_MEMORY", "true");
    parameters.set("KERNEL_DATA_BLOCKING_SIZE", "1");
    parameters.set("KERNEL_TRANS_GRID_BLOCK_SIZE", "1");
    parameters.set("KERNEL_TRANS_DATA_BLOCK_SIZE", "1");
    parameters.set("KERNEL_MAX_DIM_UNROLL", "1");
    parameters.set("KERNEL_STORE_DATA", "array");

    SGPP::datadriven::OperationMultipleEvalConfiguration configuration(
    SGPP::datadriven::OperationMultipleEvalType::STREAMING,
    SGPP::datadriven::OperationMultipleEvalSubType::OCLFASTMULTIPLATFORM, parameters);

    for (size_t i = 0; i < fileNames.size(); i++) {
        double mse = compareToReferenceTranspose(SGPP::base::ModLinear, fileNames[i], level, configuration);
        BOOST_CHECK(mse < errors[i]);
    }
}

BOOST_AUTO_TEST_CASE(Blocking) {

    std::vector<std::string> fileNames = { "datadriven/tests/data/friedman_4d.arff.gz",
            "datadriven/tests/data/friedman_10d.arff.gz" };
    std::vector<double> errors = { 10E-14, 10E-14 };

    uint32_t level = 4;

    SGPP::base::OCLConfigurationParameters parameters = getConfigurationDefaults();
    parameters.set("KERNEL_USE_LOCAL_MEMORY", "false");
    parameters.set("KERNEL_DATA_BLOCKING_SIZE", "2");
    parameters.set("KERNEL_TRANS_GRID_BLOCK_SIZE", "2");
    parameters.set("KERNEL_TRANS_DATA_BLOCK_SIZE", "2");
    parameters.set("KERNEL_MAX_DIM_UNROLL", "10");
    parameters.set("KERNEL_STORE_DATA", "register");

    SGPP::datadriven::OperationMultipleEvalConfiguration configuration(
    SGPP::datadriven::OperationMultipleEvalType::STREAMING,
    SGPP::datadriven::OperationMultipleEvalSubType::OCLFASTMULTIPLATFORM, parameters);

    for (size_t i = 0; i < fileNames.size(); i++) {
        double mse = compareToReferenceTranspose(SGPP::base::ModLinear, fileNames[i], level, configuration);
        BOOST_CHECK(mse < errors[i]);
    }
}

BOOST_AUTO_TEST_CASE(MultiDevice) {

    std::vector<std::string> fileNames = { "datadriven/tests/data/friedman_4d.arff.gz",
            "datadriven/tests/data/friedman_10d.arff.gz" };
    std::vector<double> errors = { 10E-14, 10E-14 };

    uint32_t level = 4;

    SGPP::base::OCLConfigurationParameters parameters = getConfigurationDefaults();
    parameters.set("KERNEL_USE_LOCAL_MEMORY", "false");
    parameters.set("KERNEL_DATA_BLOCKING_SIZE", "2");
    parameters.set("KERNEL_TRANS_GRID_BLOCK_SIZE", "2");
    parameters.set("KERNEL_TRANS_DATA_BLOCK_SIZE", "2");
    parameters.set("KERNEL_MAX_DIM_UNROLL", "10");
    parameters.set("KERNEL_STORE_DATA", "register");
    parameters.set("PLATFORM", "first");
    parameters.set("SELECT_SPECIFIC_DEVICE", "DISABLED");

    SGPP::datadriven::OperationMultipleEvalConfiguration configuration(
    SGPP::datadriven::OperationMultipleEvalType::STREAMING,
    SGPP::datadriven::OperationMultipleEvalSubType::OCLFASTMULTIPLATFORM, parameters);

    for (size_t i = 0; i < fileNames.size(); i++) {
        double mse = compareToReferenceTranspose(SGPP::base::ModLinear, fileNames[i], level, configuration);
        BOOST_CHECK(mse < errors[i]);
    }
}

BOOST_AUTO_TEST_CASE(MultiPlatform) {

    std::vector<std::string> fileNames = { "datadriven/tests/data/friedman_4d.arff.gz",
            "datadriven/tests/data/friedman_10d.arff.gz" };
    std::vector<double> errors = { 10E-14, 10E-14 };

    uint32_t level = 4;

    SGPP::base::OCLConfigurationParameters parameters = getConfigurationDefaults();
    parameters.set("KERNEL_USE_LOCAL_MEMORY", "false");
    parameters.set("KERNEL_DATA_BLOCKING_SIZE", "2");
    parameters.set("KERNEL_TRANS_GRID_BLOCK_SIZE", "2");
    parameters.set("KERNEL_TRANS_DATA_BLOCK_SIZE", "2");
    parameters.set("KERNEL_MAX_DIM_UNROLL", "10");
    parameters.set("KERNEL_STORE_DATA", "register");
    parameters.set("PLATFORM", "all");
    parameters.set("SELECT_SPECIFIC_DEVICE", "DISABLED");

    SGPP::datadriven::OperationMultipleEvalConfiguration configuration(
    SGPP::datadriven::OperationMultipleEvalType::STREAMING,
    SGPP::datadriven::OperationMultipleEvalSubType::OCLFASTMULTIPLATFORM, parameters);

    for (size_t i = 0; i < fileNames.size(); i++) {
        double mse = compareToReferenceTranspose(SGPP::base::ModLinear, fileNames[i], level, configuration);
        BOOST_CHECK(mse < errors[i]);
    }
}

BOOST_AUTO_TEST_CASE(SimpleSinglePrecision) {

    std::vector<std::string> fileNames = { "datadriven/tests/data/friedman_4d.arff.gz",
            "datadriven/tests/data/friedman_10d.arff.gz" };
    std::vector<double> errors = { 2.0E+04, 1.0E+4 };

    uint32_t level = 4;

    SGPP::base::OCLConfigurationParameters parameters = getConfigurationDefaults();
    parameters.set("INTERNAL_PRECISION", "float");
    parameters.set("KERNEL_USE_LOCAL_MEMORY", "false");
    parameters.set("KERNEL_DATA_BLOCKING_SIZE", "1");
    parameters.set("KERNEL_TRANS_GRID_BLOCK_SIZE", "1");
    parameters.set("KERNEL_TRANS_DATA_BLOCK_SIZE", "1");
    parameters.set("KERNEL_MAX_DIM_UNROLL", "1");
    parameters.set("KERNEL_STORE_DATA", "array");

    SGPP::datadriven::OperationMultipleEvalConfiguration configuration(
    SGPP::datadriven::OperationMultipleEvalType::STREAMING,
    SGPP::datadriven::OperationMultipleEvalSubType::OCLFASTMULTIPLATFORM, parameters);

    for (size_t i = 0; i < fileNames.size(); i++) {
        double mse = compareToReferenceTranspose(SGPP::base::ModLinear, fileNames[i], level, configuration);
        BOOST_CHECK(mse < errors[i]);
    }
}

BOOST_AUTO_TEST_CASE(BlockingSinglePrecision) {

    std::vector<std::string> fileNames = { "datadriven/tests/data/friedman_4d.arff.gz",
            "datadriven/tests/data/friedman_10d.arff.gz" };
    std::vector<double> errors = { 2.0E+04, 1.0E+4 };

    uint32_t level = 4;

    SGPP::base::OCLConfigurationParameters parameters = getConfigurationDefaults();
    parameters.set("INTERNAL_PRECISION", "float");
    parameters.set("KERNEL_USE_LOCAL_MEMORY", "false");
    parameters.set("KERNEL_DATA_BLOCKING_SIZE", "2");
    parameters.set("KERNEL_TRANS_GRID_BLOCK_SIZE", "2");
    parameters.set("KERNEL_TRANS_DATA_BLOCK_SIZE", "2");
    parameters.set("KERNEL_MAX_DIM_UNROLL", "10");
    parameters.set("KERNEL_STORE_DATA", "register");

    SGPP::datadriven::OperationMultipleEvalConfiguration configuration(
    SGPP::datadriven::OperationMultipleEvalType::STREAMING,
    SGPP::datadriven::OperationMultipleEvalSubType::OCLFASTMULTIPLATFORM, parameters);

    for (size_t i = 0; i < fileNames.size(); i++) {
        double mse = compareToReferenceTranspose(SGPP::base::ModLinear, fileNames[i], level, configuration);
        BOOST_CHECK(mse < errors[i]);
    }
}

BOOST_AUTO_TEST_CASE(MultiDeviceSinglePrecision) {

    std::vector<std::string> fileNames = { "datadriven/tests/data/friedman_4d.arff.gz",
            "datadriven/tests/data/friedman_10d.arff.gz" };
    std::vector<double> errors = { 2.0E+04, 1.0E+4 };

    uint32_t level = 4;

    SGPP::base::OCLConfigurationParameters parameters = getConfigurationDefaults();
    parameters.set("INTERNAL_PRECISION", "float");
    parameters.set("KERNEL_USE_LOCAL_MEMORY", "false");
    parameters.set("KERNEL_DATA_BLOCKING_SIZE", "2");
    parameters.set("KERNEL_TRANS_GRID_BLOCK_SIZE", "2");
    parameters.set("KERNEL_TRANS_DATA_BLOCK_SIZE", "2");
    parameters.set("KERNEL_MAX_DIM_UNROLL", "10");
    parameters.set("KERNEL_STORE_DATA", "register");
    parameters.set("PLATFORM", "first");
    parameters.set("SELECT_SPECIFIC_DEVICE", "DISABLED");

    SGPP::datadriven::OperationMultipleEvalConfiguration configuration(
    SGPP::datadriven::OperationMultipleEvalType::STREAMING,
    SGPP::datadriven::OperationMultipleEvalSubType::OCLFASTMULTIPLATFORM, parameters);

    for (size_t i = 0; i < fileNames.size(); i++) {
        double mse = compareToReferenceTranspose(SGPP::base::ModLinear, fileNames[i], level, configuration);
        BOOST_CHECK(mse < errors[i]);
    }
}

BOOST_AUTO_TEST_CASE(MultiPlatformSinglePrecision) {

    std::vector<std::string> fileNames = { "datadriven/tests/data/friedman_4d.arff.gz",
            "datadriven/tests/data/friedman_10d.arff.gz" };
    std::vector<double> errors = { 2.0E+04, 1.0E+4 };

    uint32_t level = 4;

    SGPP::base::OCLConfigurationParameters parameters = getConfigurationDefaults();
    parameters.set("INTERNAL_PRECISION", "float");
    parameters.set("KERNEL_USE_LOCAL_MEMORY", "false");
    parameters.set("KERNEL_DATA_BLOCKING_SIZE", "2");
    parameters.set("KERNEL_TRANS_GRID_BLOCK_SIZE", "2");
    parameters.set("KERNEL_TRANS_DATA_BLOCK_SIZE", "2");
    parameters.set("KERNEL_MAX_DIM_UNROLL", "10");
    parameters.set("KERNEL_STORE_DATA", "register");
    parameters.set("PLATFORM", "all");
    parameters.set("SELECT_SPECIFIC_DEVICE", "DISABLED");

    SGPP::datadriven::OperationMultipleEvalConfiguration configuration(
    SGPP::datadriven::OperationMultipleEvalType::STREAMING,
    SGPP::datadriven::OperationMultipleEvalSubType::OCLFASTMULTIPLATFORM, parameters);

    for (size_t i = 0; i < fileNames.size(); i++) {
        double mse = compareToReferenceTranspose(SGPP::base::ModLinear, fileNames[i], level, configuration);
        BOOST_CHECK(mse < errors[i]);
    }
}

BOOST_AUTO_TEST_SUITE_END()

#endif
