#!/bin/bash
# ------------------------------------------------------------------------------
# Travis CI scripts
# Copyright(c) pgRouting Contributors
#
# Test pgRouting
# ------------------------------------------------------------------------------

set -e

PGDATABASE="___vrp___test___"

while [[ $# -gt 0 ]]; do
  case $1 in
    -U)
      PGUSER=(-U "$2")
      shift
      shift
      ;;
    -d)
      PGDATABASE="$2"
      shift
      shift
      ;;
    -c|--clean)
      CLEANDB=YES
      shift
      ;;
    -p)
      PGPORT=(-p "$2")
      shift
      shift
      ;;
    -*)
      echo "Unknown option $1"
      exit 1
      ;;
    *)
        POSITIONAL_ARGS+=("$1") # save positional arg
        shift # past argument
        ;;
  esac
done

set -- "${POSITIONAL_ARGS[@]}" # restore positional parameters

echo PGUSER= "${PGUSER[@]}"
echo PGPORT= "${PGPORT[@]}"
echo PGDATABASE= "${PGDATABASE}"
echo CLEANDB= "${CLEANDB}"

if [ -n "${CLEANDB}" ]; then
    echo "Recreating database ${PGDATABASE}"
    dropdb --if-exists "${PGPORT[@]}" "${PGUSER[@]}" "${PGDATABASE}"
    createdb "${PGPORT[@]}" "${PGUSER[@]}" "${PGDATABASE}"
fi

cd ./tools/testers/
psql "${PGPORT[@]}" "${PGUSER[@]}" -d "${PGDATABASE}" -X -q -v ON_ERROR_STOP=1 --pset pager=off -f setup_db.sql

pg_prove --failures --quiet --recurse "${PGPORT[@]}" "${PGUSER[@]}" -d "${PGDATABASE}" ../../pgtap/

# database wont be removed unless script does not fails
if [ -n "$CLEANDB" ]; then
    dropdb --if-exists "${PGPORT[@]}" "${PGUSER[@]}" "${PGDATABASE}"
fi
