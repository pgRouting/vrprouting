/*PGR-GNU*****************************************************************
File: postgres_connection.c

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

#include "c_common/postgres_connection.h"

#include <string.h>
#include "utils/builtins.h"

#include "catalog/pg_type.h"


#include "c_common/debug_macro.h"



// http://www.postgresql.org/docs/9.4/static/spi-spi-finish.html
void
vrp_SPI_finish(void) {
    int code = SPI_finish();
    if (code != SPI_OK_FINISH) {  // SPI_ERROR_UNCONNECTED
        elog(ERROR, "There was no connection to SPI");
    }
}

void
vrp_SPI_connect(void) {
    int SPIcode;
    SPIcode = SPI_connect();
    if (SPIcode  != SPI_OK_CONNECT) {
        elog(ERROR, "Couldn't open a connection to SPI");
    }
}

SPIPlanPtr
vrp_SPI_prepare(const char* sql) {
    SPIPlanPtr SPIplan;
    SPIplan = SPI_prepare(sql, 0, NULL);
    if (SPIplan  == NULL) {
        elog(ERROR, "Couldn't create query plan via SPI: %s", sql);
    }
    return SPIplan;
}

Portal
vrp_SPI_cursor_open(SPIPlanPtr SPIplan) {
    Portal SPIportal;
    SPIportal = SPI_cursor_open(NULL, SPIplan, NULL, NULL, true);
    if (SPIportal == NULL) {
        elog(ERROR, "SPI_cursor_open returns NULL");
    }
    return SPIportal;
}
