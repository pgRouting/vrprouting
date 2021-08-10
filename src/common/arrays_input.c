/*PGR-GNU*****************************************************************
File: arrays_input.c

Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Developer:
Copyright (c) 2015 Celia Virginia Vergara Castillo

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
/* @file */

#include "c_common/arrays_input.h"

#include <assert.h>
#include <utils/lsyscache.h>
#include <catalog/pg_type.h>


#ifdef PROFILE
#include "c_common/debug_macro.h"
#endif

static
int64_t*
pgr_get_bigIntArr(ArrayType *v, size_t *arrlen, bool allow_empty) {
#ifdef PROFILE
    clock_t start_t = clock();
#endif

    int64_t *c_array = NULL;

    Oid     element_type = ARR_ELEMTYPE(v);
    int    *dim = ARR_DIMS(v);
    int     ndim = ARR_NDIM(v);
    int     nitems = ArrayGetNItems(ndim, dim);
    Datum  *elements;
    bool   *nulls;
    int16   typlen;
    bool    typbyval;
    char    typalign;

    assert((*arrlen) == 0);


    if (allow_empty && (ndim == 0 || nitems <= 0)) {
        return (int64_t*) NULL;
    }
    /* the array is not empty*/

    if (ndim != 1) {
        elog(ERROR, "One dimension expected");
    }

    if (nitems <= 0) {
        elog(ERROR, "No elements found");
    }

    get_typlenbyvalalign(element_type,
            &typlen, &typbyval, &typalign);

    /* validate input data type */
    switch (element_type) {
        case INT2OID:
        case INT4OID:
        case INT8OID:
            break;
        default:
            elog(ERROR, "Expected array of ANY-INTEGER");
    }

    deconstruct_array(v, element_type, typlen, typbyval,
            typalign, &elements, &nulls,
            &nitems);

    c_array = (int64_t *) palloc(sizeof(int64_t) * (size_t)nitems);
    if (!c_array) {
        elog(ERROR, "Out of memory!");
    }


    int i;
    for (i = 0; i < nitems; i++) {
        if (nulls[i]) {
            pfree(c_array);
            elog(ERROR, "NULL value found in Array!");
        } else {
            switch (element_type) {
                case INT2OID:
                    c_array[i] = (int64_t) DatumGetInt16(elements[i]);
                    break;
                case INT4OID:
                    c_array[i] = (int64_t) DatumGetInt32(elements[i]);
                    break;
                case INT8OID:
                    c_array[i] = DatumGetInt64(elements[i]);
                    break;
            }
        }
    }
    (*arrlen) = (size_t)nitems;

    pfree(elements);
    pfree(nulls);
#ifdef PROFILE
    time_msg("reading Array", start_t, clock());
#endif
    return c_array;
}


static
uint32_t*
pgr_get_positiveIntArr(ArrayType *v, size_t *arrlen, bool allow_empty) {
#ifdef PROFILE
    clock_t start_t = clock();
#endif

    uint32_t *c_array = NULL;

    Oid     element_type = ARR_ELEMTYPE(v);
    int    *dim = ARR_DIMS(v);
    int     ndim = ARR_NDIM(v);
    int     nitems = ArrayGetNItems(ndim, dim);
    Datum  *elements;
    bool   *nulls;
    int16   typlen;
    bool    typbyval;
    char    typalign;

    assert((*arrlen) == 0);


    if (allow_empty && (ndim == 0 || nitems <= 0)) {
        return (uint32_t*) NULL;
    }
    /* the array is not empty*/

    if (ndim != 1) {
        elog(ERROR, "One dimension expected");
    }

    if (nitems <= 0) {
        elog(ERROR, "No elements found");
    }

    get_typlenbyvalalign(element_type,
            &typlen, &typbyval, &typalign);

    /* validate input data type */
    switch (element_type) {
        case INT2OID:
        case INT4OID:
            break;
        default:
            elog(ERROR, "Expected array of INTEGER");
    }

    deconstruct_array(v, element_type, typlen, typbyval,
            typalign, &elements, &nulls,
            &nitems);

    c_array = (uint32_t *) palloc(sizeof(uint32_t) * (size_t)nitems);
    if (!c_array) {
        elog(ERROR, "Out of memory!");
    }


    int i;
    for (i = 0; i < nitems; i++) {
        if (nulls[i]) {
            pfree(c_array);
            elog(ERROR, "NULL value found in Array!");
        } else {
            int32_t element;
            switch (element_type) {
                case INT2OID:
                    element = (int32_t) DatumGetInt16(elements[i]);
                    break;
                case INT4OID:
                    element = (int32_t) DatumGetInt32(elements[i]);
                    break;
            }
            if (element < 0) {
                elog(ERROR, "Unexpected Negative value %d in array", element);
            }
            c_array[i] = (uint32_t) element;
        }
    }
    (*arrlen) = (size_t)nitems;

    pfree(elements);
    pfree(nulls);
#ifdef PROFILE
    time_msg("reading Array", start_t, clock());
#endif
    return c_array;
}


int64_t* pgr_get_bigIntArray(size_t *arrlen, ArrayType *input) {
    return pgr_get_bigIntArr(input, arrlen, false);
}



int64_t* pgr_get_bigIntArray_allowEmpty(size_t *arrlen, ArrayType *input) {
    return pgr_get_bigIntArr(input, arrlen, true);
}


uint32_t* pgr_get_positiveIntArray_allowEmpty(size_t *arrlen, ArrayType *input) {
    return pgr_get_positiveIntArr(input, arrlen, true);
}
