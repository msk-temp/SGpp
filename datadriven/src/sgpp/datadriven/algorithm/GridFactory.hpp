/**
 * Copyright (C) 2008-today The SG++ project
 * This file is part of the SG++ project. For conditions of distribution and
 * use, please see the copyright notice provided with SG++ or at
 * sgpp.sparsegrids.org
 *
 * GridFactory.hpp
 *
 *  Created on: May 22, 2018
 *      Author: dominik
 */

#pragma once

#include <sgpp/base/grid/Grid.hpp>
#include <sgpp/datadriven/configuration/GeometryConfiguration.hpp>

#include <string>
#include <vector>

namespace sgpp {
namespace datadriven {

/**
 * Factory class to create grids based on a configuration file
 */
class GridFactory {
 public:
  /*
   * Default constructor
   */
  GridFactory() {}

  /**
   * Creates and initializes a grid based on a configuration file
   * @param gridConfig the grid configuration
   * @param interactions the interactions for each dimension
   * @return pointer to the grid object created
   */
  sgpp::base::Grid* createGrid(const sgpp::base::GeneralGridConfiguration& gridConfig,
                               const std::vector<std::vector<size_t>> interactions) const;

  /*
   * method to decide which interactions have to be calculated based on the stencil
   * @param stencil(geometry relation of pixels) e.g. DirectNeighbours
   * @return returns the calculated interaction that have been choosen by the stencil
   */
  std::vector<std::vector<size_t>> getInteractions(
      sgpp::datadriven::GeometryConfiguration config) const;

  /*
   * calculates hierachical parent interactions
   * @param vector of resolution
   * @param defines if only for the first level the hierarchical interactions are created
   * @param defines if only for the first parent each the hierarchical interactions are created
   * @return all hierachical interactions of all pixels in a vector
   */
  void getHierarchicalParents(std::vector<std::vector<size_t>>& interactions,
                              sgpp::datadriven::GeometryConfiguration& geometryConf,
                              sgpp::datadriven::StencilConfiguration& stencilConf) const;

  /*
   * calculates direct neighbour interactions
   * @param vector of resolution
   * @return all direct neighbour interactions of all pixels in a vector
   */
  void getDirectNeighbours(std::vector<std::vector<size_t>>& interactions,
                           sgpp::datadriven::GeometryConfiguration& geometryConf,
                           sgpp::datadriven::StencilConfiguration& stencilConf) const;

 private:
  void addChildParentInteractionRecursive(
      std::vector<double>& rescale, std::vector<int64_t>& childDim, size_t currentDimension,
      std::vector<int64_t>& childPosition, std::vector<int64_t>& parentPosition,
      std::vector<size_t>& parentMultiplicators, std::vector<size_t>& childMultiplicators,
      size_t parentOffset, size_t childOffset, std::vector<std::vector<size_t>>& res) const;

  /*
   * calculates the index of a given position. This method exspect a row wise data layout
   * with each additional dimension beeing stored between the data of the pixels
   * @param vector containing the size of each dimension
   * @param vector containing the position
   * @return the index of the given position in the data
   */
  size_t getDataIndex(size_t numberOfDimensions, std::vector<size_t>& multiplicators,
                      std::vector<int64_t>& position) const;

  /*
   * Increases a given position like a counter. Therefore if a overflow happens the position will be
   * set to 0 in all dimensions
   * @param vector of resolution
   * @param current position
   */
  void getNextPosition(std::vector<int64_t>& dimension, std::vector<int64_t>& position,
                       size_t colorIndex) const;

  /*
   * Calculates the multiplicators for each level. The multiplicator is the change in the data index
   * for one step further into each dimension
   * @param vector of resolutions
   * @return vector of vectors containing the multiplicator for each dimension
   */
  std::vector<std::vector<size_t>> getMultiplicatorsPerLevel(
      std::vector<std::vector<int64_t>>& imageDimensions) const;

  void addColorInteractions(
      GeometryConfiguration& geometryConf, StencilConfiguration& stencilConf, size_t layer, size_t offset,
      std::vector<size_t>& multiplicators, std::vector<std::vector<size_t>>& interactions) const;
  /*
   * Calculates the offset for each level. The offset is the starting index of each image level
   * @param vector of resolutions
   * @param multiplicators per level
   * @return vector containing the offset for each image level
   */
  std::vector<size_t> getOffsetPerLevel(
      std::vector<std::vector<int64_t>>& imageDimensions,
      std::vector<std::vector<size_t>>& multiplicatorsPerLevel) const;

  /*
   * Adds all one dimensional interactions to the resulting vector
   * @param vector of resolutions
   * @param the vector in which the interactions are stored
   */
  void addOneDimensionalInteractions(std::vector<std::vector<int64_t>>& imageDimensions,
                                     std::vector<std::vector<size_t>>& vec) const;
};
}  // namespace datadriven
}  // namespace sgpp
