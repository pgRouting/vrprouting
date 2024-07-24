#!/bin/bash

set -e

QUIET="-v"
QUIET="-q"

if [[ -z  $1 ]]; then
    echo "taptest.sh help:"
    echo Copy on the repository root directory and adjust to your needs
    echo "Usage:"
    echo "taptest.sh <pgtap file or directory> [<postgresql options>]";
    echo "Examples:"
    echo "taptest.sh pgtap/withPoints/directed_equalityDD.pg -v -p 5433"
    echo "taptest.sh pgtap/withPoints -p 5433"
    exit 1;
fi

DIR="$1"
shift
PGFLAGS="$*"

echo "----------------------------------------------------"
echo "-----------  taptest of:  $DIR"
echo "----------------------------------------------------"
PGDATABASE="___vrp___taptest___"

dropdb  "${PGFLAGS[@]}" --if-exists "${PGDATABASE}"
createdb "${PGFLAGS[@]}" "${PGDATABASE}"

pushd tools/testers/
psql "${PGFLAGS[@]}"  -f setup_db.sql -d "${PGDATABASE}"
popd
pg_prove --recurse "${QUIET}" "${PGFLAGS[@]}"  -d "${PGDATABASE}" "${DIR}"
echo pg_prove --recurse "${QUIET}" "${PGFLAGS[@]}"  -d "${PGDATABASE}" "${DIR}"
#dropdb --if-exists "${PGFLAGS[@]}" "${PGDATABASE}"
