#!/bin/sh
# -*- coding: utf-8 -*-
###############################################################################
#
#  Project:  pgRouting
#  Purpose:  (Interactive) script to lint C and C++ code
#  Author:   Vicky Vergara <vicky_vergara@hotmail.com>
#
###############################################################################
#  Copyright (c) 2016, Vicky Vergara <vicky_vergara@hotmail.com>
#
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,
#  and/or sell copies of the Software, and to permit persons to whom the
#  Software is furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included
#  in all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
#  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#  DEALINGS IN THE SOFTWARE.
###############################################################################

set -e

if ! test -d code_linter; then
    # Get our fork of codespell that adds --words-white-list and full filename support for -S option
    mkdir code_linter
    cd code_linter || exit 1
    git clone https://github.com/google/styleguide
    cd styleguide || exit 1
    git checkout gh-pages
    cd ../.. || exit 1
    echo code_linter installed
fi

function test_c_files {
    if test -n "$1"; then
        echo "--------------------"
        echo "------   *.c  ------"
        echo "--------------------"
        python2 code_linter/styleguide/cpplint/cpplint.py --extensions=c  --linelength=120 --filter=-readability/casting $1
    fi
}

function test_cpp_files {
    if test -n "$1"; then
        echo "--------------------"
        echo "------ *.cpp  ------"
        echo "--------------------"
        python2 code_linter/styleguide/cpplint/cpplint.py --linelength=120 $1
    fi
}

function test_headers {
    if test -n "$1"; then
        echo "--------------------"
        echo "----- HEADERS  -----"
        echo "--------------------"
        python2 code_linter/styleguide/cpplint/cpplint.py --extensions=hpp,h --headers=hpp,h  --linelength=120 --filter=-runtime/references $1
    fi
}

DIRECTORY="$1"

if test -z "$DIRECTORY"; then
    test_c_files "$(git ls-files | grep -w 'c')"
    test_cpp_files "$(git ls-files | grep -w 'cpp')"
    test_headers  "$(git ls-files | grep -w 'h') $(git ls-files | grep -w 'hpp')"

else
    if [ "$DIRECTORY" = "h" ]; then
    test_headers  "$(git ls-files | grep -w 'h') $(git ls-files | grep -w 'hpp')"

    else
        test_headers  "$(git ls-files | grep -w 'h' | grep ${DIRECTORY})"
        test_c_files "$(git ls-files | grep -w 'c' | grep ${DIRECTORY})"
        test_headers  "$(git ls-files | grep -w 'hpp' | grep ${DIRECTORY})"
        test_cpp_files "$(git ls-files | grep -w 'cpp' | grep ${DIRECTORY})"
    fi
fi
