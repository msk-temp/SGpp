/*
 * Copyright (C) 2008-today The SG++ project
 * This file is part of the SG++ project. For conditions of distribution and
 * use, please see the copyright notice provided with SG++ or at
 * sgpp.sparsegrids.org
 *
 * FitterFactory.hpp
 *
 * Created on: Dec 17, 2017
 *     Author: Eric Koepke
 */

#pragma once

#include <sgpp/datadriven/datamining/configuration/DataMiningConfigParser.hpp>
#include <sgpp/datadriven/datamining/modules/fitting/ModelFittingBase.hpp>
#include <sgpp/datadriven/datamining/modules/fitting/FitterConfiguration.hpp>
#include <sgpp/datadriven/datamining/modules/hpo/ConfigurationBit.hpp>
#include <sgpp/datadriven/datamining/modules/hpo/ContinuousParameter.hpp>
#include <sgpp/datadriven/datamining/modules/hpo/DiscreteParameter.hpp>


namespace sgpp {
namespace datadriven {

/**
 * Abstract factory to build all kinds of fitters/models based on a given configuration.
 */
class FitterFactory {
 public:
  /**
   * Default constructor
   */
  FitterFactory():conpar(), dispar(), catpar(){} //= default;

  /**
   * Virtual destructor
   */
  virtual ~FitterFactory() = default;

  /**
   * Assemble a #sgpp::datadriven::ModelFittingBase object based on the configuration
   * @param parser Instance of #sgpp::datadriven::DataMiningConfigParser that reads the required
   * data from the config file.
   * @return Fully configured instance of a  #sgpp::datadriven::ModelFittingBase object.
   */

  virtual ModelFittingBase* buildFitter() = 0;


  virtual void printConfig() = 0;

  void setHarmonica();

  void getBOspace(int* nCont, std::vector<int>& nOptions); //EDIT: add categorical parameters

  void setBO(base::DataVector& cont, std::vector<int>& disc);

  void getConfigBits(std::vector<ConfigurationBit*>& configBits);

protected:
  std::map<std::string,ContinuousParameter> conpar;
  std::map<std::string,DiscreteParameter> dispar;
  std::map<std::string,DiscreteParameter> catpar;



  };
} /* namespace datadriven */
} /* namespace sgpp */
