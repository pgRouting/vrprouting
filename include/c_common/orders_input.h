/*PGR-GNU*****************************************************************
File: orders_input.h

Copyright (c) 2016 Celia Virginia Vergara Castillo
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

#ifndef INCLUDE_C_COMMON_ORDERS_INPUT_H_
#define INCLUDE_C_COMMON_ORDERS_INPUT_H_
#pragma once

#include <stddef.h>
typedef struct Orders_t Orders_t;

/** @brief Reads the pick-Deliver shipments for timestams and intervals*/
void
get_shipments(
    char *,
    Orders_t **,
    size_t *);

/** @brief Reads the pick-Deliver shipments for raw data*/
void
get_shipments_raw(
    char *,
    Orders_t **,
    size_t *);

/** @brief Reads the pick-Deliver shipments for euclidean information*/
void
get_shipments_euclidean(
    char *,
    Orders_t **,
    size_t *);

#endif  // INCLUDE_C_COMMON_ORDERS_INPUT_H_
