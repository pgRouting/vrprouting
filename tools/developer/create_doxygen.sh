#!/bin/bash

mkdir build
cd build || exit
cmake -DBUILD_DOXY=ON ..
make doxy
cd build || exit
echo "INFO: documentation is in build/doxygen/html"
echo "INFO: to build the pdf run the following commands:"
echo "cd build/doxygen/latex
pdflatex refman.tex"
echo "INFO: this requires tex to be installed"
