/******************************************************************************
* Copyright (C) 2009 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
******************************************************************************/

#include "grid/common/DirichletUpdateVector.hpp"
#include "solver/ode/VarTimestep.hpp"
#include "operation/common/OperationEval.hpp"
#include "tools/common/GridPrinter.hpp"
#include "exception/solver_exception.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

namespace sg
{

VarTimestep::VarTimestep(size_t imax, double timestepSize, double eps, ScreenOutput* screen) : ODESolver(imax, timestepSize), myScreen(screen)
{
	this->residuum = 0.0;
	this->myEps = eps;

}

VarTimestep::~VarTimestep()
{
}

void VarTimestep::solve(SLESolver& LinearSystemSolver, OperationParabolicPDESolverSystem& System, bool bIdentifyLastStep, bool verbose)
{
	size_t allIter = 0;
    DataVector* rhs;
    DataVector YkAdBas(System.getGridCoefficients()->getSize());
    DataVector YkImEul(System.getGridCoefficients()->getSize());

    double tmp_timestepsize = this->myEpsilon;
    double tmp_timestepsize_old = tmp_timestepsize;
    double tmp_timestepsize_new = tmp_timestepsize;
    double epsilon = this->myEps;

    double maxTimestep = this->nMaxIterations*tmp_timestepsize;

    size_t maxIter = this->nMaxIterations*1000;

    double time = 0.0;

    std::stringstream fnsstream;

    fnsstream << "Time_" << "VaTim" << this->myEps << ".gnuplot";

    std::string filename = fnsstream.str();

    std::ofstream fileout;

    fileout.open(filename.c_str());

	for (size_t i = 0; i < maxIter && time < maxTimestep; i++)
	{

		System.setTimestepSize(tmp_timestepsize);

		System.setODESolver("AdBas");

		// generate right hand side
		rhs = System.generateRHS();

		// solve the system of the current timestep
		LinearSystemSolver.solve(System, *System.getGridCoefficientsForCG(), *rhs, true, false, -1.0);

		System.finishTimestep();

		System.getGridCoefficientsForSC(YkAdBas);

		System.abortTimestep();

		System.setODESolver("CrNic");

		// generate right hand side
		rhs = System.generateRHS();

		// solve the system of the current timestep
		LinearSystemSolver.solve(System, *System.getGridCoefficientsForCG(), *rhs, true, false, -1.0);

		System.finishTimestep();

		System.getGridCoefficientsForSC(YkImEul);

	    YkImEul.sub(YkAdBas);

	    double tmp  = sqrt(YkImEul.dotProduct(YkImEul));

	    double deltaY = tmp/(3.0*(1.0+tmp_timestepsize/tmp_timestepsize_old));

	    tmp_timestepsize_new = tmp_timestepsize * pow(epsilon/deltaY,(double)1.0/(double)3.0);

	    if (0.8*tmp_timestepsize > tmp_timestepsize_new) {
	    	tmp_timestepsize = tmp_timestepsize_new;
	    	System.abortTimestep();
	    	allIter += LinearSystemSolver.getNumberIterations();

	    }
	    else {
	    	fileout << i << " " << (tmp_timestepsize_new-tmp_timestepsize) << " " << time << " " << tmp_timestepsize << std::endl;
	    	time += tmp_timestepsize;
	    	allIter += LinearSystemSolver.getNumberIterations();
	    	if (verbose == true)
	    	{
	    		if (myScreen == NULL)
	    		{
	    			std::cout << "Final residuum " << LinearSystemSolver.residuum << "; with " << LinearSystemSolver.getNumberIterations() << " Iterations (Total Iter.: " << allIter << ")" << std::endl;
	    		}
	    	}
	    	if (myScreen != NULL)
	    	{
	    		std::stringstream soutput;

	    		soutput << " Final residuum " << LinearSystemSolver.residuum << "; with " << LinearSystemSolver.getNumberIterations() << " Iterations (Total Iter.: " << allIter << ")";

	    		if (i < this->nMaxIterations-1)
	    		{
	    			myScreen->update((size_t)(((double)(time)*100.0)/((double)maxTimestep)), soutput.str());
	    		}
	    		else
	    		{
	    			myScreen->update(100, soutput.str());
	    		}
	    	}


	    	System.saveAlpha();




			tmp_timestepsize_old = tmp_timestepsize;
			if(tmp_timestepsize_new > tmp_timestepsize)
			{
				tmp_timestepsize = tmp_timestepsize_new;
			}

			tmp_timestepsize = std::min(tmp_timestepsize,maxTimestep-time);

	    }

	}
	fileout.close();



	// write some empty lines to console
    if (myScreen != NULL)
	{
    	myScreen->writeEmptyLines(2);
	}

}

}