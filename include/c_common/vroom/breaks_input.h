/*PGR-GNU*****************************************************************
File: breaks_input.h

Copyright (c) 2021 pgRouting developers
Mail: project@pgrouting.org

Function's developer:
Copyright (c) 2021 Ashish Kumar
Mail: ashishkr23438@gmail.com

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

#ifndef INCLUDE_C_COMMON_VROOM_BREAKS_INPUT_H_
#define INCLUDE_C_COMMON_VROOM_BREAKS_INPUT_H_
#pragma once

#include <stddef.h>

#include "c_common/get_check_data.h"
#include "c_types/column_info_t.h"
#include "c_types/vroom/vroom_break_t.h"

#ifdef PROFILE
#include "c_common/debug_macro.h"
#include "c_common/time_msg.h"
#endif

/** @brief Reads the VROOM breaks */
void
get_vroom_breaks(
    char *breaks_sql,
    Vroom_break_t **breaks,
    size_t *total_breaks);

#endif  // INCLUDE_C_COMMON_VROOM_BREAKS_INPUT_H_
