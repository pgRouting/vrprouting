#!/bin/bash

# This runs shellcheck on all sh files


DIR=$(git rev-parse --show-toplevel)

pushd "${DIR}" > /dev/null || exit
code=0

for f in $(git ls-files | grep '\.sh')
do
  result=$(shellcheck "${f}")

  if [[ $result ]]; then
    echo "$result"
    echo " *** shellcheck found script errors while processing $f"
    code=1
  fi
done
popd > /dev/null || exit 1
exit $code
