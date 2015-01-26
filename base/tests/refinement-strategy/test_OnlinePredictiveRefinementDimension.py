# Copyright (C) 2008-today The SG++ project
# This file is part of the SG++ project. For conditions of distribution and
# use, please see the copyright notice provided with SG++ or at 
# sgpp.sparsegrids.org

import unittest
import math
import random
from pysgpp import Grid, DataVector, DataMatrix, OnlinePredictiveRefinementDimension, HashRefinement, refinement_map, createOperationMultipleEval, GridIndex

class TestOnlinePredictiveRefinementDimension(unittest.TestCase):

    def setUp(self):

        #
        # Grid
        #

        DIM = 2
        LEVEL = 2

        self.grid = Grid.createLinearGrid(DIM)
        self.grid_gen = self.grid.createGridGenerator()
        self.grid_gen.regular(LEVEL)


        #
        # trainData, classes, errors
        #

        xs = []
        DELTA = 0.05
        DELTA_RECI = int(1/DELTA)

        for i in xrange(DELTA_RECI):
            for j in xrange(DELTA_RECI):
                xs.append([DELTA*i, DELTA*j])

        random.seed(1208813)
        ys = [ random.randint(-10, 10) for i in xrange(DELTA_RECI**2)]

        self.trainData = DataMatrix(xs)
        self.classes = DataVector(ys)
        self.alpha = DataVector([3, 6, 7, 9, -1])
        self.multEval = createOperationMultipleEval(self.grid, self.trainData)

        self.errors = DataVector(DELTA_RECI**2)
        coord = DataVector(DIM)

        for i in xrange(self.trainData.getNrows()):
            self.trainData.getRow(i, coord)
            self.errors.__setitem__ (i, abs(self.classes[i] - self.grid.eval(self.alpha, coord)))

        #
        # OnlinePredictiveRefinementDimension
        #

        hash_refinement = HashRefinement();
        self.strategy = OnlinePredictiveRefinementDimension(hash_refinement)
        self.strategy.setTrainDataset(self.trainData)
        self.strategy.setClasses(self.classes)
        self.strategy.setErrors(self.errors)

    def test_1(self):

        storage = self.grid.getStorage()
        gridSize = self.grid.getSize()
        numDim = storage.dim()

        print "######"
        print "Expected result:"
        print "######"

        expected = {}

        for j in xrange(gridSize):

            gridIndex = storage.get(j)
            gridIndex.setLeaf(False)
                
            print "Point: ", j, " (", gridIndex.toString(), ")"

            for d in xrange(numDim):

                #
                # Get left and right child
                #

                leftChild = GridIndex(gridIndex)
                rightChild = GridIndex(gridIndex)

                storage.left_child(leftChild, d)
                storage.right_child(rightChild, d)

                #
                # Check if point is refinable
                #

                if storage.has_key(leftChild) or storage.has_key(rightChild):
                    continue

                #
                # Insert children temporarily
                #

                storage.insert(leftChild) 
                storage.insert(rightChild) 

                val1 = self.calc_indicator_value(leftChild)
                val2 = self.calc_indicator_value(rightChild)
                
                storage.deleteLast()
                storage.deleteLast()

                print "Dimension: ", d
                print "Left Child: ", val1
                print "Right Child: ", val2
                print ""

                expected[(j, d)] = val1 + val2
            
            print ""

        for k, v in expected.iteritems():
            print(k, v)

        print "######"
        print "Actual result:"
        print "######"

        actual = refinement_map({})
        self.strategy.collectRefinablePoints(storage, 10, actual)
        
        for k, v in actual.iteritems():
            print(k, v)

        #
        # Assertions
        #

        for k, v in expected.iteritems():
            self.assertEqual(actual[k], v)

    def calc_indicator_value(self, index):

        numData = self.trainData.getNrows()
        numCoeff = self.grid.getSize()
        seq = self.grid.getStorage().seq(index)

        num = 0
        denom = 0

        tmp = DataVector(numCoeff)
        self.multEval.multTranspose(self.errors, tmp) 

        num = tmp.__getitem__(seq)
        num **= 2

        alpha = DataVector(numCoeff)
        col = DataVector(numData)
        alpha.__setitem__(seq, 1.0)
        self.multEval.mult(alpha, col)

        col.sqr()

        denom = col.sum()

        if denom == 0:
            print "Denominator is zero"
            value = 0
        else:
            value = num/denom 

        return value

if __name__=='__main__':
    unittest.main()