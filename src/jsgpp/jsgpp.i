/******************************************************************************
* Copyright (C) 2009 Technische Universitaet Muenchen                         *
* This file is part of the SG++ project. For conditions of distribution and   *
* use, please see the copyright notice at http://www5.in.tum.de/SGpp          *
******************************************************************************/
// @author Joerg Blank (blankj@in.tum.de), Alexander Heinecke (Alexander.Heinecke@mytum.de), Dirk Pflueger (pflueged@in.tum.de)

%module jsgpp

%include "stl.i"
%include "std_vector.i"
%include "std_pair.i"
%include "std_string.i"

%include "typemaps.i"

%include "exception.i"

%exception {
  try {
    $action
  } catch (const std::exception& e) {
    SWIG_exception(SWIG_RuntimeError, e.what());
  }
}


namespace std {
    %template(DoubleVector) vector<double>;
    %template(IndexValPair) pair<size_t, double>;
    %template(IndexValVector) vector<pair<size_t, double> >;
    %template(SizeTVector) vector<size_t>;
	%template(SizeTVector) vector<size_t>;
	%template(SizeTVector) vector<size_t>;
}



// This should include all necessary header files
%{
#include "sgpp.hpp"
%}

// The Good, i.e. without any modifications
%include "src/sgpp/base/grid/storage/hashmap/SerializationVersion.hpp"
%include "src/sgpp/base/grid/storage/hashmap/HashGridIndex.hpp"
%include "src/sgpp/base/grid/storage/hashmap/HashGridIterator.hpp"
%include "src/sgpp/base/grid/storage/hashmap/HashGridStorage.hpp"
%include "src/sgpp/base/grid/GridStorage.hpp"
%include "src/sgpp/base/grid/common/BoundingBox.hpp"
%include "src/sgpp/base/grid/common/Stretching.hpp"
%include "src/sgpp/base/grid/common/DirichletUpdateVector.hpp"

%include "Operations.i"

%include "src/sgpp/base/grid/generation/hashmap/HashGenerator.hpp"
%include "src/sgpp/base/grid/generation/hashmap/HashRefinement.hpp"
%include "src/sgpp/base/grid/generation/hashmap/HashCoarsening.hpp"
%include "src/sgpp/base/grid/generation/hashmap/HashRefinementBoundaries.hpp"
%include "src/sgpp/base/grid/generation/StandardGridGenerator.hpp"
%include "src/sgpp/base/grid/generation/BoundaryGridGenerator.hpp"
%include "src/sgpp/base/grid/generation/TrapezoidBoundaryGridGenerator.hpp"
%include "src/sgpp/base/grid/generation/StretchedTrapezoidBoundaryGridGenerator.hpp"
%include "src/sgpp/base/grid/generation/SurplusRefinementFunctor.hpp"
%include "src/sgpp/base/grid/generation/SurplusVolumeRefinementFunctor.hpp"
%include "src/sgpp/base/grid/generation/SurplusCoarseningFunctor.hpp"

%include "GridFactory.i"

%include "src/sgpp/base/tools/GridPrinter.hpp"
%include "src/sgpp/base/tools/GridPrinterForStretching.hpp"
%include "src/sgpp/base/tools/StdNormalDistribution.hpp"

%include "src/sgpp/base/grid/GridDataBase.hpp"
%include "src/sgpp/base/tools/OperationQuadratureMC.hpp"

// the Bad
%include "src/sgpp/base/datatypes/DataVector.hpp"
%include "src/sgpp/base/datatypes/DataMatrix.hpp"

// and the rest

%include "src/sgpp/sgpp.hpp"

%include "src/sgpp/pde/operation/OperationParabolicPDESolverSystem.hpp"
%include "src/sgpp/pde/operation/OperationParabolicPDESolverSystemDirichlet.hpp"
%include "src/sgpp/pde/operation/OperationParabolicPDESolverSystemNeumann.hpp"

%include "src/sgpp/datadriven/algorithm/AlgorithmDGEMV.hpp"
%include "src/sgpp/datadriven/algorithm/AlgorithmMultipleEvaluation.hpp"
%include "src/sgpp/datadriven/algorithm/test_dataset.hpp"
%include "src/sgpp/base/algorithm/GetAffectedBasisFunctions.hpp"
%include "src/sgpp/base/algorithm/AlgorithmEvaluation.hpp"
%include "src/sgpp/base/algorithm/AlgorithmEvaluationTransposed.hpp"
%include "src/sgpp/base/algorithm/sweep.hpp"
%include "src/sgpp/datadriven/algorithm/DMSystemMatrix.hpp"
%include "src/sgpp/pde/algorithm/BlackScholesParabolicPDESolverSystem.hpp"
%include "src/sgpp/pde/algorithm/BlackScholesParabolicPDESolverSystemEuroAmer.hpp"
%include "src/sgpp/pde/algorithm/BlackScholesParabolicPDESolverSystemEuroAmerParallelOMP.hpp"
%include "src/sgpp/pde/algorithm/HeatEquationParabolicPDESolverSystem.hpp"

%include "src/sgpp/base/application/ScreenOutput.hpp"

%include "src/sgpp/pde/application/PDESolver.hpp"
%include "src/sgpp/pde/application/ParabolicPDESolver.hpp"
%include "src/sgpp/pde/application/BlackScholesSolver.hpp"
%include "src/sgpp/pde/application/BlackScholesSolverWithStretching.hpp"
%include "src/sgpp/pde/application/HeatEquationSolver.hpp"
%include "src/sgpp/pde/application/HeatEquationSolverWithStretching.hpp"
%include "src/sgpp/finance/application/VariableDiscountFactor.hpp"

%include "src/sgpp/base/basis/linear/noboundary/LinearBasis.hpp"
%include "src/sgpp/base/basis/linear/boundary/LinearBoundaryBasis.hpp"
%include "src/sgpp/base/basis/linearstretched/noboundary/LinearStretchedBasis.hpp"
%include "src/sgpp/base/basis/linearstretched/boundary/LinearStretchedBoundaryBasis.hpp"
%include "src/sgpp/base/basis/modlinear/ModifiedLinearBasis.hpp"
%include "src/sgpp/base/basis/poly/PolyBasis.hpp"
%include "src/sgpp/base/basis/modpoly/ModifiedPolyBasis.hpp"
%include "src/sgpp/base/basis/modwavelet/ModifiedWaveletBasis.hpp"
%include "src/sgpp/base/basis/modbspline/ModifiedBsplineBasis.hpp"
%include "src/sgpp/base/basis/prewavelet/PrewaveletBasis.hpp"

%include "src/sgpp/base//SGSolver.hpp"
%include "src/sgpp/base//SLESolver.hpp"
%include "src/sgpp/base//ODESolver.hpp"
%feature("director") ConjugateGradients;
%include "src/sgpp/base/solver/sle/ConjugateGradients.hpp"
%include "src/sgpp/base/solver/sle/BiCGStab.hpp"
%include "src/sgpp/base/solver/ode/Euler.hpp"
%include "src/sgpp/base/solver/ode/CrankNicolson.hpp"

 // static factory methods
%include "src/sgpp/base/basis/operations_factory.hpp"
//%include "src/sgpp/datadriven/operation/DatadrivenOpFactory.hpp"
//%include "src/sgpp/parallel/operation/ParallelOpFactory.hpp"
//%include "src/sgpp/finance/operation/FinanceOpFactory.hpp"
//%include "src/sgpp/pde/operation/PdeOpFactory.hpp"
//%include "src/sgpp/base/operation/BaseOpFactory.hpp"

%apply std::string *INPUT { std::string& istr };

%apply unsigned int *OUTPUT { unsigned int& l, unsigned int& i };

%template(GridIndex) sg::base::HashGridIndex<unsigned int, unsigned int>;
%template(GridStorage) sg::base::HashGridStorage<sg::base::GridIndex>;

%template(SLinearBase) sg::base::LinearBasis<unsigned int, unsigned int>;
%template(SLinearBoundaryBase) sg::base::LinearBoundaryBasis<unsigned int, unsigned int>;
%template(SLinearStretchedBase) sg::base::LinearStretchedBasis<unsigned int, unsigned int>;
%template(SLinearStretchedBoundaryBase) sg::base::LinearStretchedBoundaryBasis<unsigned int, unsigned int>;
%template(SModLinearBase) sg::base::ModifiedLinearBasis<unsigned int, unsigned int>;
%template(SPolyBase) sg::base::PolyBasis<unsigned int, unsigned int>;
%template(SModPolyBase) sg::base::ModifiedPolyBasis<unsigned int, unsigned int>;
%template(SModWaveletBase) sg::base::ModifiedWaveletBasis<unsigned int, unsigned int>;
%template(SModBsplineBase) sg::base::ModifiedBsplineBasis<unsigned int, unsigned int>;
%template(SPrewaveletBase) sg::base::PrewaveletBasis<unsigned int, unsigned int>;

%apply std::vector<std::pair<size_t, double> > *OUTPUT { std::vector<std::pair<size_t, double> >& result };
%apply std::vector<double> *INPUT { std::vector<double>& point }; 

%template(SGetAffectedBasisFunctions) sg::base::GetAffectedBasisFunctions<sg::base::SLinearBase>;
%template(SAlgorithmEvaluation) sg::base::AlgorithmEvaluation<sg::base::SLinearBase>;
%template(SGetAffectedBasisFunctionsBoundaries) sg::base::GetAffectedBasisFunctions<sg::base::SLinearBoundaryBase>;
%template(SGetAffectedBasisFunctionsLinearStretchedBoundaries) sg::base::GetAffectedBasisFunctions<sg::base::SLinearStretchedBoundaryBase>;
%template(DimensionBoundaryVector) std::vector<sg::base::DimensionBoundary>;
%template(Stretching1DVector) std::vector<sg::base::Stretching1D>;

//%template(GridIndex) sg::HashGridIndex<unsigned int, unsigned int>;
//%template(GridStorage) sg::HashGridStorage<sg::GridIndex>;
//
//%template(SLinearBase) sg::LinearBasis<unsigned int, unsigned int>;
//%template(SLinearBoundaryBase) sg::LinearBoundaryBasis<unsigned int, unsigned int>;
//%template(SLinearStretchedBase) sg::LinearStretchedBasis<unsigned int, unsigned int>;
//%template(SLinearStretchedBoundaryBase) sg::LinearStretchedBoundaryBasis<unsigned int, unsigned int>;
//%template(SModLinearBase) sg::ModifiedLinearBasis<unsigned int, unsigned int>;
//%template(SPolyBase) sg::PolyBasis<unsigned int, unsigned int>;
//%template(SModPolyBase) sg::ModifiedPolyBasis<unsigned int, unsigned int>;
//%template(SModWaveletBase) sg::ModifiedWaveletBasis<unsigned int, unsigned int>;
//%template(SModBsplineBase) sg::ModifiedBsplineBasis<unsigned int, unsigned int>;
//%template(SPrewaveletBase) sg::base::prewavelet_base<unsigned int, unsigned int>;
//
//%apply std::vector<std::pair<size_t, double> > *OUTPUT { std::vector<std::pair<size_t, double> >& result };
//%apply std::vector<double> *INPUT { std::vector<double>& point }; 
//
//%template(SGetAffectedBasisFunctions) sg::base::GetAffectedBasisFunctions<sg::SLinearBase>;
//%template(SAlgorithmEvaluation) sg::AlgorithmEvaluation<sg::SLinearBase>;
//%template(SGetAffectedBasisFunctionsBoundaries) sg::base::GetAffectedBasisFunctions<sg::SLinearBoundaryBase>;
