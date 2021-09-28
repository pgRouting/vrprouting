#!/bin/bash
# ------------------------------------------------------------------------------
# vrpRouting Scripts
# Copyright(c) vrpRouting Contributors
#
# Remove all the obsolete entries, i.e. lines starting with #~ from .po files
# ------------------------------------------------------------------------------

# For all the chapter files
for file in locale/en/LC_MESSAGES/*.po; do
    if grep -q '#~' "$file"; then
        perl -pi -0777 -e 's/#~.*//s' "$file"
        git add "$file"
    fi
done

