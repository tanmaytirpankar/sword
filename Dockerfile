FROM klee/klee:2.1

MAINTAINER Tanmay
# To build image:
# docker build -t tanmaytirpankar/openmpracechecker:1.0 .
# To run container:
# docker run -it tanmaytirpankar/openmpracechecker:1.0 /bin/bash
RUN sudo apt-get update

# Installing all dependencies
RUN sudo apt-get install -y git ninja-build glpk-utils libglpk-dev glpk-doc libboost-all-dev
ENV RACE_CHECKER_BUILD=/home/klee/RaceCheckerBuild

# Cloning OpenMP
RUN mkdir $RACE_CHECKER_BUILD && cd $RACE_CHECKER_BUILD && git clone https://github.com/llvm-mirror/openmp.git openmp && cd openmp && git reset --hard 307b6fcfcd1dd8983e77d8fc83f913ddc55b7a5f
ENV OPENMP_INSTALL=/home/klee/usr

# Installing OpenMP
RUN mkdir $RACE_CHECKER_BUILD/openmp/build && cd $RACE_CHECKER_BUILD/openmp/build && cmake -G Ninja  -D CMAKE_C_COMPILER=clang  -D CMAKE_CXX_COMPILER=clang++  -D CMAKE_BUILD_TYPE=Release  -D CMAKE_INSTALL_PREFIX:PATH=$OPENMP_INSTALL  .. && ninja -j8 -l8 && ninja install
RUN mkdir $RACE_CHECKER_BUILD/sword
COPY . /home/klee/RaceCheckerBuild/sword

# Setting install Paths
ENV SWORD_INSTALL=/home/klee/usr
ENV ARCHER_INSTALL=/home/klee/usr

# Installing Sword
RUN mkdir $RACE_CHECKER_BUILD/sword/build && cd $RACE_CHECKER_BUILD/sword/build && cmake -G Ninja  -D CMAKE_C_COMPILER=clang  -D CMAKE_CXX_COMPILER=clang++  -D CMAKE_BUILD_TYPE=Release -D OMP_PREFIX:PATH=$OPENMP_INSTALL  -D CMAKE_INSTALL_PREFIX:PATH=${SWORD_INSTALL} -D COMPRESSION=LZO .. && ninja -j8 -l8 && ninja install

RUN cd $RACE_CHECKER_BUILD && git clone https://github.com/PRUNERS/archer.git archer
# Installing Archer
RUN mkdir $RACE_CHECKER_BUILD/archer/build && cd $RACE_CHECKER_BUILD/archer/build && cmake -G Ninja -D CMAKE_C_COMPILER=clang -D CMAKE_CXX_COMPILER=clang++ -D OMP_PREFIX:PATH=$OPENMP_INSTALL -D CMAKE_INSTALL_PREFIX:PATH=${ARCHER_INSTALL} .. && ninja -j8 -l8 && ninja install

# Adding installed path to $PATH
ENV PATH=$SWORD_INSTALL/bin:$PATH
ENV PATH=$ARCHER_INSTALL/bin:$PATH