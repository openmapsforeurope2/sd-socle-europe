# =========================
# STAGE 1 : BUILD
# =========================
FROM condaforge/miniforge3:latest AS build

RUN apt-get update \
 && apt-get install -y --no-install-recommends \
      git \
      build-essential \
      clang \
 && rm -rf /var/lib/apt/lists/*

ENV IGN_SOCLE_DIR=/usr/local/src/sd-socle

RUN git clone --branch mborne --recurse-submodules \
    http://gitlab.forge-idi.ign.fr/socle/sd-socle $IGN_SOCLE_DIR

# DEBUG
COPY ./CMakeLists.txt $IGN_SOCLE_DIR/CMakeLists.txt
COPY ./CMakeLists_cgal.txt $IGN_SOCLE_DIR/extension/cgal/src/CMakeLists.txt

ENV IGN_DATA=$IGN_SOCLE_DIR/data

WORKDIR $IGN_SOCLE_DIR

# environnment conda
RUN conda update -n base -c conda-forge conda \
 && conda env create -f environment/conda/env_conda.yml

ENV CONDA_PREFIX=/opt/conda/envs/ign-socle
ENV PATH=$CONDA_PREFIX/bin:$PATH \
    CMAKE_MODULE_PATH=$IGN_SOCLE_DIR/CMakeModules

# intallation de GL/gl.sh
RUN conda install -y -c conda-forge libgl-devel=1.7

# construction
COPY ./build-unix-europe.sh .

ENV CONFIG=Release
RUN ./build-unix-europe.sh

ENV CONFIG=Debug
RUN ./build-unix-europe.sh

RUN find $CONDA_PREFIX -name "*.a" -delete \ 
 && find $CONDA_PREFIX -name "*.pyc" -delete \ 
 && find $CONDA_PREFIX -name "__pycache__" -type d -exec rm -rf {} +

# =========================
# STAGE 2
# =========================
FROM ubuntu:24.04

ENV IGN_SOCLE_DIR=/usr/local/src/sd-socle \
    CONDA_PREFIX=/opt/conda/envs/ign-socle

COPY --from=build $IGN_SOCLE_DIR/include                      $IGN_SOCLE_DIR/include
COPY --from=build $IGN_SOCLE_DIR/extension/cgal/include       $IGN_SOCLE_DIR/extension/cgal/include
COPY --from=build $IGN_SOCLE_DIR/src                          $IGN_SOCLE_DIR/src
COPY --from=build $IGN_SOCLE_DIR/extension/cgal/src           $IGN_SOCLE_DIR/extension/cgal/src
COPY --from=build $IGN_SOCLE_DIR/lib                          $IGN_SOCLE_DIR/lib
COPY --from=build $IGN_SOCLE_DIR/data                         $IGN_SOCLE_DIR/data
COPY --from=build $IGN_SOCLE_DIR/*.cmake                      $IGN_SOCLE_DIR/
COPY --from=build $IGN_SOCLE_DIR/CMakeModules                 $IGN_SOCLE_DIR/CMakeModules
COPY --from=build $CONDA_PREFIX/include                       $CONDA_PREFIX/include
COPY --from=build $CONDA_PREFIX/lib                           $CONDA_PREFIX/lib
# pour Qt5
COPY --from=build $CONDA_PREFIX/bin                           $CONDA_PREFIX/bin
COPY --from=build $CONDA_PREFIX/mkspecs/linux-g++             $CONDA_PREFIX/mkspecs/linux-g++
COPY --from=build $CONDA_PREFIX/plugins                       $CONDA_PREFIX/plugins

ENV LD_LIBRARY_PATH=$CONDA_PREFIX/lib:$IGN_SOCLE_DIR/lib:$LD_LIBRARY_PATH \
    IGN_DATA=$IGN_SOCLE_DIR/data \
    CMAKE_MODULE_PATH=$IGN_SOCLE_DIR/CMakeModules \
    CMAKE_PREFIX_PATH=$CONDA_PREFIX/lib/cmake

WORKDIR $IGN_SOCLE_DIR