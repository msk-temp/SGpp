#!/bin/sh

scons -j $(nproc) VERBOSE=1 OPT=1 SG_ALL=0 SG_BASE=1 SG_OPTIMIZATION=1 SG_PYTHON=1 \
    USE_MATLAB=1 BUILD_STATICLIB=1 USE_PYTHON3_FOR_PYSGPP=1 RUN_PYTHON_TESTS=0 \
    CPPPATH=/opt/MATLAB/R2017b/extern/include \
    LIBPATH=/opt/MATLAB/R2017b/bin/glnxa64 \
    RPATH=/opt/MATLAB/R2017b/bin/glnxa64