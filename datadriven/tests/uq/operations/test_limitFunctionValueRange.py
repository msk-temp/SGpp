import numpy as np
import matplotlib.pyplot as plt

from pysgpp import (Grid, DataVector,
                    createOperationLimitFunctionValueRange,
                    MakePositiveCandidateSearchAlgorithm_Intersections,
                    MakePositiveInterpolationAlgorithm_SetToZero)
from pysgpp.extensions.datadriven.uq.plot import plotDensity2d, plotSG2d, \
    plotFunction2d, plotFunction1d
# from pysgpp.extensions.datadriven.uq.dists import MultivariateNormal, SGDEdist, Beta, J
from pysgpp.extensions.datadriven.uq.operations import (hierarchize,
                                                        evalSGFunctionMulti)
from pysgpp.extensions.datadriven.uq.operations.forcePositivity import OperationMakePositiveFast, InterpolateFunction
from pysgpp.extensions.datadriven.uq.operations.forcePositivity.findIntersections import IntersectionCandidates
from pysgpp.extensions.datadriven.uq.operations.forcePositivity.fullGridSearch import FullGridCandidates
from pysgpp.extensions.datadriven.uq.operations.forcePositivity.searchNextLevel import SearchLevelWiseForCandidates
from pysgpp.extensions.datadriven.uq.operations.forcePositivity.localFullGridSearch import LocalFullGridCandidates
from pysgpp.extensions.datadriven.uq.plot.plot1d import plotSG1d
from pysgpp.extensions.datadriven.uq.operations.sparse_grid import addConst
from pysgpp.pysgpp_swig import MakePositiveInterpolationAlgorithm_InterpolateExp, \
    MakePositiveInterpolationAlgorithm_InterpolateBoundaries1d

numDims = 2
level = 7
code = "c++"
side = "lower"
verbose = False
plot = True
interpolationAlgorithm = MakePositiveInterpolationAlgorithm_InterpolateBoundaries1d
# interpolationAlgorithm = MakePositiveInterpolationAlgorithm_SetToZero
ylower, yupper = -1.2, 1.0


def hierarchizeFun(fun):
    nodalValues = np.ndarray(grid.getSize())
    p = DataVector(gs.getDimension())
    for i in xrange(gs.getSize()):
        gs.getPoint(i).getStandardCoordinates(p)
        nodalValues[i] = fun(p.array())

    return hierarchize(grid, nodalValues)


# mu = np.ones(numDims) * 0.75
# cov = np.diag(np.ones(numDims) * 0.1 / 10.)
# dist1 = MultivariateNormal(mu, cov, 0, 1)
#
# mu = np.ones(numDims) * 0.25
# cov = np.diag(np.ones(numDims) * 0.1 / 10.)
# dist2 = MultivariateNormal(mu, cov, 0, 1)
#
# mu = np.ones(numDims) * 0.5
# cov = np.diag(np.ones(numDims) * 0.1 / 10.)
# dist3 = MultivariateNormal(mu, cov, 0, 1)
#
#
# def twoModalGauss(x):
#     return dist1.pdf(x) + dist2.pdf(x) + dist3.pdf(x)


def sin(x):
    return np.sum(np.sin(np.pi * (2 * x - 1.0)))


def arctan(x):
    return np.sum(np.arctan(np.pi * (2 * x - 1.0)))

fun = sin

# plot analytic function
if plot:
    if numDims == 1:
        fig = plt.figure()
        plotFunction1d(fun)
        plt.title("analytic")
        fig.show()
    elif numDims == 2:
        fig = plt.figure()
        plotFunction2d(fun)
        plt.title("analytic")
        fig.show()

# get a sparse grid approximation
grid = Grid.createLinearGrid(numDims)
grid.getGenerator().regular(level)
gs = grid.getStorage()

alpha = hierarchizeFun(fun)
print "l=%i: (gs=%i)" % (level, grid.getSize())
print "-" * 80

# plot the result
if plot and numDims < 3:
    fig = plt.figure()
    if numDims == 1:
        plotSG1d(grid, alpha)
    elif numDims == 2:
        plotSG2d(grid, alpha, show_negative=False, show_grid_points=True)

    plt.title("neg: #gp = %i" % grid.getStorage().getSize())
    fig.show()

if side == "lower":
    sides = ["lower"]
elif side == "upper":
    sides = ["upper"]
else:  # both
    sides = ["lower", "upper"]

if code == "c++":
    opLimit = createOperationLimitFunctionValueRange(MakePositiveCandidateSearchAlgorithm_Intersections,
                                                     interpolationAlgorithm,
                                                     verbose)

    alpha_vec = DataVector(alpha)
    if len(sides) == 1:
        if sides[0] == "lower":
            opLimit.doLowerLimitation(grid, alpha_vec, ylower)
        else:
            opLimit.doUpperLimitation(grid, alpha_vec, yupper)
    else:
        opLimit.doLimitation(grid, alpha_vec, ylower, yupper)

    alpha = alpha_vec.array()
else:
    for i, side in enumerate(sides):
        if side == "lower":
            alpha = addConst(grid, alpha, 1.0, -ylower)
        elif side == "upper":
            alpha = addConst(grid, alpha, -1.0, yupper)
        # plot the result
        if plot and numDims < 3:
            fig = plt.figure()
            if numDims == 1:
                plotSG1d(grid, alpha, show_grid_points=True)
            elif numDims == 2:
                plotSG2d(grid, alpha, show_negative=False, show_grid_points=True)

            plt.title("%i) before %s: #gp = %i" % (i, side, grid.getStorage().getSize()))
            fig.show()

        # now make the function positive
#         createOperationMakePositive(grid), makePositive(newGrid, alpha)
#         grid = newGrid
        grid, alpha = OperationMakePositiveFast(grid).makePositive(alpha)

        if side == "lower":
            alpha = addConst(grid, alpha, 1.0, ylower)
        elif side == "upper":
            alpha = addConst(grid, alpha, -1.0, yupper)

        print "-" * 80
        print "l=%i: (gs=%i)" % (level, grid.getSize())

        # plot the result
        if plot and numDims < 3:
            fig = plt.figure()
            if numDims == 1:
                plotSG1d(grid, alpha, show_grid_points=True)
            elif numDims == 2:
                plotSG2d(grid, alpha, show_negative=False, show_grid_points=True)

            plt.title("%i) after %s: #gp = %i" % (i, side, grid.getStorage().getSize()))
            fig.show()

print "l=%i: (gs=%i)" % (level, grid.getSize())
print "-" * 80

# plot the final result
if plot and numDims < 3:
    fig = plt.figure()
    if numDims == 1:
        plotSG1d(grid, alpha)
    elif numDims == 2:
        plotSG2d(grid, alpha, show_negative=False, show_grid_points=True)

    plt.title("pos: #gp = %i" % grid.getStorage().getSize())
    fig.show()

    plt.show()
