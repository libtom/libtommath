This is the git repository for [LibTomMath](http://www.libtom.org/), a free open source portable number theoretic multiple-precision integer library written entirely in C.

The `develop` branch contains the in-development version. Stable releases are tagged.

Documentation is built from the LaTeX file `bn.tex`. There is also limited documentation in `tommath.h`. There is also a document, `tommath.pdf`, which describes the goals of the project and many of the algorithms used.

Tests are located in `demo/`.

The project can be build by using `make`. Along with the usual `make`, `make clean` and `make install`, there are several other build targets, see the makefile for details. There are also makefiles for certain specific platforms.