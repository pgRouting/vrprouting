#!/bin/bash

set -e


# copy this file into the root of your repository
# adjust to your needs

# set up your postgres version and port
PGVERSION="12"
PGPORT="5432"
PGBIN="/usr/lib/postgresql/${PGVERSION}/bin"

# Compiler setup

# When more than one compiler is installed
GCC="8"

ALLDIRS="
pickDeliver
vrp_basic
version
"

TESTDIRS=${ALLDIRS}


function test_compile {

echo ------------------------------------
echo ------------------------------------
echo "Compiling with G++-$1"
echo ------------------------------------

if [ -n "$1" ]; then
    update-alternatives --set gcc "/usr/bin/gcc-$1"
fi


cd build/

cmake  -DPOSTGRESQL_BIN=${PGBIN} -DDOC_USE_BOOTSTRAP=ON -DWITH_DOC=ON -DBUILD_DOXY=ON  -DBUILD_LATEX=ON  -DCMAKE_BUILD_TYPE=Debug -DPROJECT_DEBUG=ON ..
#cmake  -DPOSTGRESQL_BIN=${PGBIN} -DDOC_USE_BOOTSTRAP=ON -DWITH_DOC=ON -DBUILD_DOXY=ON  -DBUILD_LATEX=ON  -DES=ON -DLINKCHECK=ON ..

#make doc
#time make doxy
make
sudo make install
cd ..

echo
echo --------------------------------------------
echo  Update signatures
echo --------------------------------------------
tools/release-scripts/get_signatures.sh -p ${PGPORT}

#exit

echo --------------------------------------------
echo  Execute documentation queries for a particular directories
echo --------------------------------------------


# choose what is going to be tested while developing
for d in ${TESTDIRS}
do
    tools/testers/doc_queries_generator.pl  -alg ${d} -documentation  -pgport ${PGPORT}
    #tools/testers/doc_queries_generator.pl  -alg ${d} -pgport ${PGPORT}
    time tools/developer/taptest.sh  ${d} -p ${PGPORT}
done


#exit

########################################################
# pgTap test all
########################################################

dropdb --if-exists -p $PGPORT ___pgr___test___
createdb  -p $PGPORT ___pgr___test___
echo $PGPORT
tools/testers/pg_prove_tests.sh vicky $PGPORT
dropdb  -p $PGPORT ___pgr___test___


################################
################################
## checks all the repository
#
#  the rest of the script use PGPORT variable
################################
################################
echo
echo --------------------------------------------
echo  Update / Verify NEWS
echo --------------------------------------------
tools/release-scripts/notes2news.pl
if git status | grep 'NEWS'; then
    echo "**************************************************"
    echo "           WARNING"
    echo "the signatures changed, copying generated files"
    echo "Plese verify the changes are minimal"
    echo "**************************************************"
    git diff NEWS
fi

########################################################
#  Execute documentation queries for the whole project
########################################################
tools/testers/doc_queries_generator.pl  -documentation  -pgport $PGPORT
tools/testers/doc_queries_generator.pl -pgport $PGPORT

cd build
#rm -rf doc/*
make doc
#rm -rf doxygen/*
make doxy
cd ..

#tools/testers/update-tester.sh

}

# Uncomment what you need
for compiler in ${GCC}
do
    if [ -n "$1" ]; then
        echo "Fresh build"
        rm -rf build/*
    fi
    test_compile ${compiler}
done
