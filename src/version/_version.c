/*PGR-GNU*****************************************************************

File: _version.c

Function's developer:
Copyright (c) 2015 Celia Virginia Vergara Castillo
Mail: vicky_vergara@hotmail.com

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

#include "c_common/postgres_connection.h"

#define UNUSED(x) (void)(x)
PG_MODULE_MAGIC;

PGDLLEXPORT Datum _vrp_build_type(PG_FUNCTION_ARGS);
PGDLLEXPORT Datum _vrp_compilation_date(PG_FUNCTION_ARGS);
PGDLLEXPORT Datum _vrp_compiler_version(PG_FUNCTION_ARGS);
PGDLLEXPORT Datum _vrp_git_hash(PG_FUNCTION_ARGS);
PGDLLEXPORT Datum _vrp_lib_version(PG_FUNCTION_ARGS);
PGDLLEXPORT Datum _vrp_operating_system(PG_FUNCTION_ARGS);
PGDLLEXPORT Datum _vrp_pgsql_version(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(_vrp_build_type);
PGDLLEXPORT Datum _vrp_build_type(PG_FUNCTION_ARGS) {
    UNUSED(fcinfo);
    char *ver = CMAKE_BUILD_TYPE;
    text *result = cstring_to_text(ver);
    PG_RETURN_TEXT_P(result);
}

PG_FUNCTION_INFO_V1(_vrp_compilation_date);
PGDLLEXPORT Datum _vrp_compilation_date(PG_FUNCTION_ARGS) {
    UNUSED(fcinfo);
    char *ver = COMPILATION_DATE;
    text *result = cstring_to_text(ver);
    PG_RETURN_TEXT_P(result);
}

PG_FUNCTION_INFO_V1(_vrp_compiler_version);
PGDLLEXPORT Datum _vrp_compiler_version(PG_FUNCTION_ARGS) {
    UNUSED(fcinfo);
    char *ver = COMPILER_VERSION;
    text *result = cstring_to_text(ver);
    PG_RETURN_TEXT_P(result);
}

PG_FUNCTION_INFO_V1(_vrp_git_hash);
PGDLLEXPORT Datum _vrp_git_hash(PG_FUNCTION_ARGS) {
    UNUSED(fcinfo);
    char *ver = PROJECT_GIT_HASH;
    text *result = cstring_to_text(ver);
    PG_RETURN_TEXT_P(result);
}

PG_FUNCTION_INFO_V1(_vrp_lib_version);
PGDLLEXPORT Datum _vrp_lib_version(PG_FUNCTION_ARGS) {
    UNUSED(fcinfo);
    char *ver = PROJECT_LIB_NAME;
    text *result = cstring_to_text(ver);
    PG_RETURN_TEXT_P(result);
}

PG_FUNCTION_INFO_V1(_vrp_operating_system);
PGDLLEXPORT Datum _vrp_operating_system(PG_FUNCTION_ARGS) {
    UNUSED(fcinfo);
    char *ver = SYSTEM_NAME;
    text *result = cstring_to_text(ver);
    PG_RETURN_TEXT_P(result);
}


PG_FUNCTION_INFO_V1(_vrp_pgsql_version);
PGDLLEXPORT Datum _vrp_pgsql_version(PG_FUNCTION_ARGS) {
    UNUSED(fcinfo);
    char *ver = PostgreSQL_VERSION_STRING;
    text *result = cstring_to_text(ver);
    PG_RETURN_TEXT_P(result);
}
