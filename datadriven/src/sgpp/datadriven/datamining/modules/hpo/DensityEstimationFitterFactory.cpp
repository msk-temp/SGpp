/*
 * Copyright (C) 2008-today The SG++ project
 * This file is part of the SG++ project. For conditions of distribution and
 * use, please see the copyright notice provided with SG++ or at
 * sgpp.sparsegrids.org
 *
 * DensityEstimationFitterFactory.cpp
 *
 *  Created on:	17.12.2017
 *      Author: Eric Koepke
 */

#include <sgpp/datadriven/datamining/modules/hpo/DensityEstimationFitterFactory.hpp>
#include <sgpp/datadriven/datamining/modules/fitting/FitterConfiguration.hpp>
#include <sgpp/datadriven/datamining/modules/fitting/FitterConfigurationLeastSquares.hpp>
#include <sgpp/datadriven/datamining/modules/fitting/ModelFittingLeastSquares.hpp>
#include <sgpp/datadriven/datamining/modules/fitting/ModelFittingBase.hpp>

namespace sgpp {
namespace datadriven {

DensityEstimationFitterFactory::DensityEstimationFitterFactory(DataMiningConfigParser& parser)
  :baseConfig(){

	baseConfig.readParams(parser);

	//EDIT: new hier ohne pointer?

  dispar["level"] = DiscreteParameter("level", 1, 4);

  catpar["basisFunction"] = DiscreteParameter("basisFunction",0,1);

  conpar["lambda"] = ContinuousParameter(6, "lambda", -8, 0);


}



ModelFittingBase* DensityEstimationFitterFactory::buildFitter()  {

  // build config
  FitterConfigurationLeastSquares* config = new FitterConfigurationLeastSquares(baseConfig);
  //EDIT: make lambda exponential
  base::GridType basisFunction[] = {base::GridType::Linear,base::GridType::ModLinear};

  config->getGridConfig().level_ = dispar["level"].getValue();
  config->getGridConfig().type_ = basisFunction[catpar["basisFunction"].getValue()];
  config->getRegularizationConfig().lambda_ = pow(10, conpar["lambda"].getValue());

  return new ModelFittingLeastSquares(*config);
}

void DensityEstimationFitterFactory::printConfig(){

	std::string basisFunction[] = {"Linear","ModLinear"};
	std::cout<<"Level: "<< dispar["level"].getValue()
					 <<", Basis: "<<basisFunction[catpar["basisFunction"].getValue()]
					 <<", Lambda: "<< pow(10, conpar["lambda"].getValue())
					 <<std::endl;
}

} /* namespace datadriven */
} /* namespace sgpp */
