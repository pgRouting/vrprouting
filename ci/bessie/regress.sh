#!/bin/sh
#--
# Jenkins variables passed in
#--
#
# export BRANCH=develop

#-------------------------
# File used in Jenkins setup
#-------------------------
export PATH="${PATH}:/usr/local::/usr/local/lib::/usr/local/bin"
echo "$PATH"
export PGUSER=postgres
export PGROUTING_VER="${BRANCH}"

rm -rf "${WORKSPACE}/build${BRANCH}"
mkdir "${WORKSPACE}/build${BRANCH}"

cmake --version

cd "../build${BRANCH}" || exit 1
cmake "../${BRANCH}"

make
sudo make install
cd "../${BRANCH}" || exit 1
perl tools/testers/doc_queries_generator.pl

psql -c "CREATE DATABASE ___vrp___test___"
bash tools/testers/pg_prove_tests.sh "${PGUSER}"
psql -c "DROP DATABASE ___vrp___test___"
