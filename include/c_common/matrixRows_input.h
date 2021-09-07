/*PGR-GNU*****************************************************************
File: matrixRows_input.h

Copyright (c) 2015 Celia Virginia Vergara Castillo
vicky_vergara@hotmail.com

------

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

 ********************************************************************PGR-GNU*/

#ifndef INCLUDE_C_COMMON_MATRIXROWS_INPUT_H_
#define INCLUDE_C_COMMON_MATRIXROWS_INPUT_H_
#pragma once

#include <stddef.h>

typedef struct Matrix_cell_t Matrix_cell_t;

/** @brief Get the travel time matrix */
void get_matrixRows(
    char *sql,
    Matrix_cell_t **rows,
    size_t *total_rows);

/** @brief Get the travel time matrix with numerical types*/
void get_matrixRows_plain(
    char *sql,
    Matrix_cell_t **rows,
    size_t *total_rows);

/** @brief Get the travel time matrix of vroom with integer types*/
void get_matrixRows_vroom_plain(
    char *sql,
    Matrix_cell_t **rows,
    size_t *total_rows);

#endif  // INCLUDE_C_COMMON_MATRIXROWS_INPUT_H_
