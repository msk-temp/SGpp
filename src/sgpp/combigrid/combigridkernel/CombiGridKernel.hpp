/*
 * CombiCombiGridKernel.hpp
 *
 *  Created on: Feb 22, 2011
 *      Author: benk
 */

#ifndef COMBICOMBIGRIDKERNEL_HPP_
#define COMBICOMBIGRIDKERNEL_HPP_

#include "combigrid/fullgrid/CombiFullGrid.hpp"
#include "combigrid/combischeme/CombiSchemeBasis.hpp"

using namespace std;

namespace combigrid {

/** This class is the kernel component of the combination technique. <br>
 * Contains a set of full grid and its coefficient */

template<typename ELEMENT>
class CombiGridKernel {
public:

	/** Ctor for the set of full grids (kernel component)*/
	CombiGridKernel(int dim) {
		dim_ = dim;
		fullgrids_.resize(0);
		coefs_.resize(0);
		nrFG_ = 0;
	}

	/** Ctor with a combi scheme as an input */
	CombiGridKernel(const CombiSchemeBasis* combischeme , const std::vector<bool>& hasBoundaryPts) {
		dim_ = combischeme->getDim();
		fullgrids_.resize(0);
		coefs_.resize(0);
		nrFG_ = 0;
		initialize( combischeme , hasBoundaryPts );
	}

	virtual ~CombiGridKernel() {
		// delete all full grids
		deleteAll();
	}

	/** method which initializes the object with a combi scheme object*/
	void initialize(const CombiSchemeBasis* combischeme , const std::vector<bool>& hasBoundaryPts){

		// first delete everything what was there before
		deleteAll();

		// now create the kernel from the scheme
		dim_ = combischeme->getDim();
		for (int i = 0 ; i < combischeme->getNrSapces() ; i++){
			// add a full grid
			addFullGrid( combischeme->getLevel(i) , hasBoundaryPts , combischeme->getCoef(i) );
		}
	}

	/** adds a full grid with the specified level (dimension was specified in the Ctor) and boundary
	 * @param levels levels of the FG
	 * @param hasBoundaryPts for each dimension if the full grid should have boundary points
	 * @param coef the coeficient in the combination scheme */
	void addFullGrid(const std::vector<int>& levels, const std::vector<bool>& hasBoundaryPts , double coef){
		// create the full grid and add to the grid vector
		FullGrid<ELEMENT>* fg = new combigrid::FullGrid<ELEMENT>( dim_ , levels , hasBoundaryPts );
		fullgrids_.resize(nrFG_+1);
		fullgrids_[nrFG_] = fg;
		coefs_.resize(nrFG_+1);
		coefs_[nrFG_] = coef;
		nrFG_ = nrFG_ + 1;
	}

	/** the full grids which apear twice will be deleted.
	 * The last instance will be deleted including the coefficient */
	void deleteDuplicate() {

		std::vector<bool> markForDelete(fullgrids_.size(),false);
		bool isEqual = false;

		// compare each grid to each grid and check if two are equal
		for (int i = 0 ; i < (int)fullgrids_.size() ; i++ ){
			for (int j = i+1 ; j < (int)fullgrids_.size() ; j++ ){
				FullGrid<ELEMENT>* fg1 = fullgrids_[i];
				FullGrid<ELEMENT>* fg2 = fullgrids_[j];
				// test if full grid i is equal
				isEqual = true;
				for (int k = 0 ; k < dim_ ; k++){
					isEqual = ( isEqual && (fg1->getLevels()[k] == fg2->getLevels()[k]) );
				}
				markForDelete[j] = markForDelete[j] || isEqual;
			}
		}

		// delete the marked full grids
		for (int i = 0 ; i < (int)fullgrids_.size() ; i++ ){
			// if this grid was marked then
			if (markForDelete[i]) { deleteFullGrid(i); }
		}
	}

	/** detele the choosen full grid
	 * @param i */
	void deleteFullGrid(int i){
		//delete one full grid
		if ( i < nrFG_ -1 ){
			// here we swap the last element with the i-th element
			FullGrid<ELEMENT>* fg = fullgrids_[i];
			fullgrids_[i] = fullgrids_[nrFG_-1];
			fullgrids_[nrFG_-1] = fg;
			coefs_[i] = coefs_[nrFG_-1];
		}
		// delete the last element
		delete fullgrids_[nrFG_-1];
		coefs_.resize(nrFG_-1);
		nrFG_ = nrFG_ - 1;
	}

	/** return the number of full grids */
	inline int getNrFullGrids() const { return nrFG_; }

	/** return the full grids level vector */
	inline const std::vector<int>& getFullGridLevel(int i) const { return fullgrids_[i]->getLevels(); }

	/** return the full grid at a given position */
	inline FullGrid<ELEMENT>* getFullGrid(int i) { return fullgrids_[i]; }

	inline const FullGrid<ELEMENT>* getFullGrid(int i) const { return fullgrids_[i]; }

	/** return the dimension */
	int getDim() const { return dim_; }

	/** return the coefficient of one space */
	int getCoef(int i) const { return coefs_[i]; }

private:

	/** deletes all the full grids and frees the memory */
	void deleteAll(){
		// delete all full grids
		for (int i = 0 ; i < nrFG_ ; i++){
			//COMBIGRID_OUT_LEVEL3( 6, "~deleteAll delete i:" << i << " , fullgrids_.size():" << fullgrids_.size());
			delete fullgrids_[i];
		}
	}

	/** dimensions of the full grids */
	int dim_;

	/** number of full grids */
	int nrFG_;

	/** the coefficients of the full grids*/
	std::vector<double> coefs_;

	/** vector contains the full grids */
	std::vector< FullGrid<ELEMENT>* > fullgrids_;
};

}

#endif /* COMBICOMBIGRIDKERNEL_HPP_ */