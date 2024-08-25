/*PGR-GNU*****************************************************************

File: check_get_data.cpp

Copyright (c) 2024 pgRouting developers
Mail: pgrouting-dev@discourse.osgeo.org

Developer:
Copyright (c) 2024 Celia Virginia Vergara Castillo
Mail: vicky at erosion.dev

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

#include "cpp_common/check_get_data.hpp"

extern "C" {
#include <postgres.h>
#include <executor/spi.h>
#include <funcapi.h>
#include <fmgr.h>
#include <access/htup_details.h>
#include <catalog/pg_type.h>
#include <utils/lsyscache.h>
#include <utils/builtins.h>
#include <utils/timestamp.h>
#include <utils/date.h>
#include <utils/datetime.h>
}

#include <vector>
#include <unordered_set>
#include <string>
#include <ctime>
#include <limits>
#include "cpp_common/undefPostgresDefine.hpp"

#include "cpp_common/alloc.hpp"
#include "cpp_common/info.hpp"


namespace {

void
check_interval_type(const vrprouting::Info &info) {
    if (!(info.type == 1186)) {
        throw std::string("Unexpected type in column '") + info.name + "'. Expected INTERVAL";
    }
}

void
check_jsonb_type(vrprouting::Info info) {
    if (!(info.type == JSONBOID)) {
        throw std::string("Unexpected type in column '") + info.name + "'. Expected JSONB";
    }
}

void
check_integer_type(vrprouting::Info info) {
    if (!(info.type == INT2OID || info.type == INT4OID)) {
        throw std::string("Unexpected type in column '") + info.name + "'. Expected SMALLINT or INTEGER";
    }
}

void
check_integerarray_type(vrprouting::Info info) {
    if (!(info.type == INT2ARRAYOID || info.type == INT4ARRAYOID)) {
        throw std::string("Unexpected type in column '") + info.name + "'. Expected INTEGER-ARRAY";
    }
}

/**
 * @brief The function check whether column type is ANY-INTEGER-ARRAY or not.
 *
 * Where ANY-INTEGER-ARRAY is SQL type:
 *   SMALLINT[], INTEGER[], BIGINT[]
 *
 * @param[in] info contain column information.
 * @throw ERROR Unexpected type in column. Expected ANY-INTEGER-ARRAY.
 */

void
check_any_integerarray_type(vrprouting::Info info) {
    if (!(info.type == INT2ARRAYOID
                || info.type == INT4ARRAYOID
                || info.type == 1016)) {
        throw std::string("Unexpected type in column '") + info.name + "'. Expected ANY-INTEGER-ARRAY";
    }
}

void
check_timestamp_type(vrprouting::Info info) {
    if (!(info.type == 1114)) {
        throw std::string("Unexpected type in column '") + info.name + "'. Expected TIMESTAMP";
    }
}


/** @brief Function tells expected type of each column and then check the correspondence type of each column.
 *
 * [SPI_fnumber](https://www.postgresql.org/docs/current/spi-spi-fnumber.html)
 * [SPI_gettypeid](https://www.postgresql.org/docs/9.1/static/spi-spi-gettypeid.html)
 * @param[in] tupdesc  tuple description.
 * @param[in] info     contain one or more column information.
 * @throw column not found.
 * @throw ERROR Unknown type of column.
 * @return @b TRUE when column exist.
 *        @b FALSE when column was not found.
 */
bool
get_column_info(const TupleDesc &tupdesc, vrprouting::Info &info) {
    info.colNumber =  SPI_fnumber(tupdesc, info.name.c_str());
    if (info.strict && info.colNumber == SPI_ERROR_NOATTRIBUTE) {
        throw std::string("Column '") + info.name + "' not Found";
    }

    if (info.colNumber != SPI_ERROR_NOATTRIBUTE) {
        info.type = SPI_gettypeid(tupdesc, info.colNumber);
        if (info.type == InvalidOid) {
            throw std::string("Type of column '") + info.name + "' not Found";
        }
        return true;
    }
    return false;
}

/** @brief The function check whether column type is ANY-INTEGER or not.
 *
 * Where ANY-INTEGER is SQL type: SMALLINT, INTEGER, BIGINT
 *
 * @param[in] info contain column information.
 * @throw ERROR Unexpected type in column. Expected column type is ANY-INTEGER.
 */
void
check_any_integer_type(const vrprouting::Info &info) {
    if (!(info.type == INT2OID
                || info.type == INT4OID
                || info.type == INT8OID)) {
        throw std::string("Unexpected type in column '") + info.name + "'. Expected ANY-INTEGER";
    }
}

/**
 * @brief The function check whether column type is ANY-NUMERICAL.
 *        Where ANY-NUMERICAL is SQL type:
 *             SMALLINT, INTEGER, BIGINT, REAL, FLOAT
 *
 * @param[in] info contain column information.
 * @throw ERROR Unexpected type in column. Expected column type is ANY-NUMERICAL.
 */
void check_any_numerical_type(const vrprouting::Info &info) {
    if (!(info.type == INT2OID
                || info.type == INT4OID
                || info.type == INT8OID
                || info.type == FLOAT4OID
                || info.type == FLOAT8OID
                || info.type == NUMERICOID)) {
        throw std::string("Unexpected type in column '") + info.name + "'. Expected ANY-NUMERICAL";
    }
}

/**
 * @brief The function check whether column type is TEXT or not.
 *       Where TEXT is SQL type:
 *             TEXT
 *
 * @param[in] info contain column information.
 * @throw ERROR Unexpected type in column. Expected column type is TEXT.
 */
void
check_text_type(const vrprouting::Info &info) {
    if (!(info.type == TEXTOID)) {
        throw std::string("Unexpected type in column '") + info.name + "'. Expected TEXT";
    }
}

/**
 * @brief The function check whether column type is CHAR or not.
 *        Where CHAR is SQL type:
 *             CHARACTER
 *
 * [BPCHAROID](https://doxygen.postgresql.org/include_2catalog_2pg__type_8h.html#afa7749dbe36d31874205189d9d6b21d7)
 * @param[in] info contain column information.
 * @throw ERROR Unexpected type in column. Expected column type is CHAR.
 */
void
check_char_type(const vrprouting::Info &info) {
    if (!(info.type == BPCHAROID)) {
        throw std::string("Unexpected type in column '") + info.name + "'. Expected TEXT";
    }
}

TInterval
getInterval(const HeapTuple tuple, const TupleDesc &tupdesc, const vrprouting::Info &info) {
    Datum binval;
    bool isnull;
    Interval*   interval;

    binval = SPI_getbinval(tuple, tupdesc, info.colNumber, &isnull);

    if (isnull) throw std::string("Unexpected Null value in column ") + info.name;

    switch (info.type) {
        case INTERVALOID:
            interval = DatumGetIntervalP(binval);
            break;
        default:
            throw std::string("Unexpected type value in column '") + info.name + "'. Expected INTERVALOID";
    }

    return interval->time / 1000000
        + interval->day * SECS_PER_DAY
        + static_cast<int64_t>(
                interval->month * ((DAYS_PER_YEAR / static_cast<double>(MONTHS_PER_YEAR)) * SECS_PER_DAY));
}


TTimestamp
getTimeStamp(const HeapTuple tuple, const TupleDesc &tupdesc, const vrprouting::Info &info) {
    Datum binval;
    bool isnull;
    TTimestamp value = 0;
    binval = SPI_getbinval(tuple, tupdesc, info.colNumber, &isnull);

    if (isnull) throw std::string("Unexpected Null value in column ") + info.name;

    switch (info.type) {
        case 1114:
            value = vrprouting::get_timestamp_without_timezone((TTimestamp) Int64GetDatum(binval));
            break;
        default:
            throw std::string("Unexpected type value in column '") + info.name + "'. Expected 1114";
    }
    return value;
}

/**
 * @param[in] tuple   input row to be examined.
 * @param[in] tupdesc  tuple descriptor
 * @param[in] info    contain column information.
 * @throw ERROR Unexpected type in column. Expected column type is ANY-INTEGER.
 * @throw ERROR When value of column is NULL.
 *
 * @return Integer type of column value is returned.
 */
int64_t getBigInt(
        const HeapTuple tuple, const TupleDesc &tupdesc, const vrprouting::Info &info) {
    Datum binval;
    bool isnull;
    int64_t value = 0;
    binval = SPI_getbinval(tuple, tupdesc, info.colNumber, &isnull);
    if (isnull)
        throw std::string("Unexpected Null value in column ") + info.name;
    switch (info.type) {
        case INT2OID:
            value = static_cast<int64_t>(DatumGetInt16(binval));
            break;
        case INT4OID:
            value = static_cast<int64_t>(DatumGetInt32(binval));
            break;
        case INT8OID:
            value = DatumGetInt64(binval);
            break;
        default:
            throw std::string("Unexpected type in column type of ") + info.name + ". Expected ANY-INTEGER";
    }
    return value;
}

/**
 * @param[in] tuple   input row to be examined.
 * @param[in] tupdesc  tuple descriptor
 * @param[in] info    contain column information.
 * @throw ERROR Unexpected type in column. Expected column type is ANY-NUMERICAL.
 * @throw ERROR When value of column is NULL.
 * @return Double type of column value is returned.
 */
double getFloat8(
        const HeapTuple tuple, const TupleDesc &tupdesc, const vrprouting::Info &info) {
    Datum binval;
    bool isnull = false;
    binval = SPI_getbinval(tuple, tupdesc, info.colNumber, &isnull);
    if (isnull)
        throw std::string("Unexpected Null value in column ") + info.name;

    switch (info.type) {
        case INT2OID:
            return static_cast<double>(DatumGetInt16(binval));
            break;
        case INT4OID:
            return static_cast<double>(DatumGetInt32(binval));
            break;
        case INT8OID:
            return static_cast<double>(DatumGetInt64(binval));
            break;
        case FLOAT4OID:
            return static_cast<double>(DatumGetFloat4(binval));
            break;
        case FLOAT8OID:
            return static_cast<double>(DatumGetFloat8(binval));
            break;
        case NUMERICOID:
            /* Note: out-of-range values will be clamped to +-HUGE_VAL */
            return static_cast<double>(DatumGetFloat8(DirectFunctionCall1(numeric_float8_no_overflow, binval)));
            break;
        default:
            throw std::string("Unexpected type in column type of ") + info.name + ". Expected ANY-NUMERICAL";
    }
    return 0.0;
}

/**
 * http://doxygen.postgresql.org/include_2catalog_2pg__type_8h.html;
 * [SPI_getbinval](https://www.postgresql.org/docs/8.1/static/spi-spi-getbinval.html)
 * [Datum](https://doxygen.postgresql.org/datum_8h.html)
 * [DatumGetInt16](https://doxygen.postgresql.org/postgres_8h.html#aec991e04209850f29a8a63df0c78ba2d)
 *
 * @param[in] tuple         input row to be examined.
 * @param[in] tupdesc       tuple descriptor
 * @param[in] info          contain column information.
 * @param[in] default_value returned when column contain NULL value.
 * @throw ERROR Unexpected type in column. Expected column type is CHAR.
 * @throw ERROR When value of column is NULL.
 * @return Char type of column value is returned.
 */
char getChar(
        const HeapTuple tuple, const TupleDesc &tupdesc, const vrprouting::Info &info, char default_value) {
    Datum binval;
    bool isNull;
    char value = default_value;

    binval = SPI_getbinval(tuple, tupdesc, info.colNumber, &isNull);
    if (!(info.type == BPCHAROID)) {
        throw std::string("Unexpected type in column type of ") + info.name + ". Expected CHAR";
    }

    if (!isNull) {
        value =  reinterpret_cast<char*>(binval)[1];
    } else {
        if (info.strict) {
            throw std::string("Unexpected Null value in column ") + info.name;
        }
        value = default_value;
    }
    return value;
}


/** @brief get the array contents from postgres
 *
 * @details This function generates the array inputs according to their type
 * received through @a ArrayType *v parameter and store them in @a c_array. It
 * can be empty also if received @a allow_empty true. The cases of failure are:-
 * 1. When @a ndim is not equal to one dimension.
 * 2. When no element is found i.e. nitems is zero or negative.
 * 3. If the element type doesn't lie in switch cases, give the error of expected array of any integer type
 * 4. When size of @a c_array is out of range or memory.
 * 5. When null value is found in the array.
 *
 * All these failures are represented as error through @a elog.
 * @param[in] v Pointer to the postgres C array
 *
 * @pre the array has to be one dimension
 * @pre Must have elements (when allow_empty is false)
 *
 * @returns set of elements on the PostgreSQL array
 */
std::unordered_set<uint32_t>
pgarray_to_unordered_set(ArrayType *v) {
    std::unordered_set<uint32_t> results;

    if (!v) return results;

    auto    element_type = ARR_ELEMTYPE(v);
    auto    dim = ARR_DIMS(v);
    auto    ndim = ARR_NDIM(v);
    auto    nitems = ArrayGetNItems(ndim, dim);
    Datum  *elements = nullptr;
    bool   *nulls = nullptr;
    int16   typlen;
    bool    typbyval;
    char    typalign;


    if (ndim == 0 || nitems <= 0) {
        return results;
    }

    if (ndim != 1) {
        throw std::string("One dimension expected");
    }

    get_typlenbyvalalign(element_type, &typlen, &typbyval, &typalign);

    switch (element_type) {
        case INT2OID:
        case INT4OID:
        case INT8OID:
            break;
        default:
            throw std::string("Expected array of ANY-INTEGER");
    }

    deconstruct_array(v, element_type, typlen, typbyval,
            typalign, &elements, &nulls,
            &nitems);

    int64_t data(0);

    for (int i = 0; i < nitems; i++) {
        if (nulls[i]) {
            throw std::string("NULL value found in Array!");
        } else {
            switch (element_type) {
                case INT2OID:
                    data = static_cast<int64_t>(DatumGetInt16(elements[i]));
                    break;
                case INT4OID:
                    data = static_cast<int64_t>(DatumGetInt32(elements[i]));
                    break;
                case INT8OID:
                    data = DatumGetInt64(elements[i]);
                    break;
            }
        }
        /*
         * Before saving, check if its a uint32_t
         */

        if (data < 0 || data > std::numeric_limits<uint32_t>::max()) {
            throw std::string("Illegal value found on array");
        }
        results.insert(static_cast<uint32_t>(data));
    }

    pfree(elements);
    pfree(nulls);
    return results;
}

/** @brief get the array contents from postgres
 *
 * @details This function generates the array inputs according to their type
 * received through @a ArrayType *v parameter and store them in @a c_array. It
 * can be empty also if received @a allow_empty true. The cases of failure are:-
 * 1. When @a ndim is not equal to one dimension.
 * 2. When no element is found i.e. nitems is zero or negative.
 * 3. If the element type doesn't lie in switch cases, give the error of expected array of any integer type
 * 4. When size of @a c_array is out of range or memory.
 * 5. When null value is found in the array.
 *
 * All these failures are represented as error through @a elog.
 * @param[in] v Pointer to the postgres C array
 * @param[in] allow_empty flag to allow empty arrays
 *
 * @pre the array has to be one dimension
 * @pre Must have elements (when allow_empty is false)
 *
 * @returns Vector of elements of the PostgreSQL array
 */
std::vector<int64_t>
get_pgarray(ArrayType *v, bool allow_empty) {
    std::vector<int64_t> results;
    if (!v) return results;

    auto    element_type = ARR_ELEMTYPE(v);
    auto    dim = ARR_DIMS(v);
    auto    ndim = ARR_NDIM(v);
    auto    nitems = ArrayGetNItems(ndim, dim);
    Datum  *elements = nullptr;
    bool   *nulls = nullptr;
    int16   typlen;
    bool    typbyval;
    char    typalign;


    if (allow_empty && (ndim == 0 || nitems <= 0)) {
        return results;
    }

    if (ndim != 1) {
        throw std::string("One dimension expected");
    }

    if (nitems <= 0) {
        throw std::string("No elements found");
    }

    get_typlenbyvalalign(element_type, &typlen, &typbyval, &typalign);

    /* validate input data type */
    switch (element_type) {
        case INT2OID:
        case INT4OID:
        case INT8OID:
            break;
        default:
            throw std::string("Expected array of ANY-INTEGER");
    }

    deconstruct_array(v, element_type, typlen, typbyval,
            typalign, &elements, &nulls,
            &nitems);

    int64_t data(0);

    results.reserve(static_cast<size_t>(nitems));

    for (int i = 0; i < nitems; i++) {
        if (nulls[i]) {
            throw std::string("NULL value found in Array!");
        } else {
            switch (element_type) {
                case INT2OID:
                    data = static_cast<int64_t>(DatumGetInt16(elements[i]));
                    break;
                case INT4OID:
                    data = static_cast<int64_t>(DatumGetInt32(elements[i]));
                    break;
                case INT8OID:
                    data = DatumGetInt64(elements[i]);
                    break;
            }
        }
        results.push_back(data);
    }

    pfree(elements);
    pfree(nulls);
    return results;
}

std::vector<int64_t>
get_BigIntArr_wEmpty(
        const HeapTuple tuple, const TupleDesc &tupdesc,
        const vrprouting::Info &info) {
    bool is_null = false;

    Datum raw_array = SPI_getbinval(tuple, tupdesc, info.colNumber, &is_null);
    /*
     * [DatumGetArrayTypeP](https://doxygen.postgresql.org/array_8h.html#aa1b8e77c103863862e06a7b7c07ec532)
     * [pgr_get_bigIntArray](http://docs.vrprouting.org/doxy/2.2/arrays__input_8c_source.html)
     */
    if (!raw_array) return std::vector<int64_t>();

    ArrayType *pg_array = DatumGetArrayTypeP(raw_array);

    return get_pgarray(pg_array, true);
}


}  // namespace

namespace vrprouting {

namespace detail {
std::vector<int64_t>
get_any_positive_array(const HeapTuple tuple, const TupleDesc &tupdesc, const Info &info) {
    if (!column_found(info)) return std::vector<int64_t>();
    auto data = get_BigIntArr_wEmpty(tuple, tupdesc, info);
    for (const auto &e : data) {
        if (e < 0) throw std::string("Unexpected negative value in array '") + info.name + "'";
    }
    return data;
}

std::vector<uint32_t>
get_uint_array(const HeapTuple tuple, const TupleDesc &tupdesc, const Info &info) {
    bool is_null = false;

    Datum raw_array = SPI_getbinval(tuple, tupdesc, info.colNumber, &is_null);
    if (!raw_array) return  std::vector<uint32_t>();

    ArrayType *pg_array = DatumGetArrayTypeP(raw_array);

    auto data = get_pgarray(pg_array, true);
    std::vector<uint32_t> results(data.begin(), data.end());
    return results;
}

/**
 * @param [in] tuple
 * @param [in] tupdesc
 * @param [in] info about the column been fetched
 * @param [in] opt_value default value when the column does not exist
 *
 * @returns The value found
 * @returns opt_value when the column does not exist
 *
 * exceptions when the value is negative
 * @pre for positive values only
 */
TInterval
get_interval(
        const HeapTuple tuple, const TupleDesc &tupdesc, const Info &info, TInterval opt_value) {
    TInterval value = column_found(info)? getInterval(tuple, tupdesc, info) : opt_value;
    if (value < 0) throw std::string("Unexpected negative value in column '") + info.name + "'";
    return (TInterval) value;
}

/**
 * @param [in] tuple
 * @param [in] tupdesc
 * @param [in] info about the column been fetched
 * @param [in] opt_value default value when the column does not exist
 *
 * @returns The value found
 * @returns opt_value when the column does not exist
 */
TTimestamp
get_timestamp(
        const HeapTuple tuple, const TupleDesc &tupdesc, const Info &info, TTimestamp opt_value) {
    return column_found(info)?  getTimeStamp(tuple, tupdesc, info) : opt_value;
}

/**
 * @param [in] tuple from postgres
 * @param [in] tupdesc from postgres
 * @param [in] info about the column been fetched
 * @param [in] opt_value default value when the column does not exist
 *
 * @returns The value found
 * @returns opt_value when the column does not exist
 *
 * Used with vrprouting::ANY_INTEGER
 */
int64_t
get_anyinteger(const HeapTuple tuple, const TupleDesc &tupdesc, const Info &info, int64_t opt_value) {
    return column_found(info)? getBigInt(tuple, tupdesc, info) : opt_value;
}
}  // namespace detail

/**
 * @param[in] info column information
 * @return @b TRUE when colNumber exist.
 *         @b FALSE when colNumber was not found.
 *
 * [SPI_ERROR_NOATTRIBUTE](https://doxygen.postgresql.org/spi_8h.html#ac1512d8aaa23c2d57bb0d1eb8f453ee2)
 */
bool column_found(const Info &info) {
    return !(info.colNumber == SPI_ERROR_NOATTRIBUTE);
}


/**
 * @param[in] tupdesc  tuple descriptor
 * @param[in] info     contain one or more column information.
 *
 * @throw ERROR Unknown type of column.
 */
void fetch_column_info(
        const TupleDesc &tupdesc,
        std::vector<vrprouting::Info> &info) {
    for (auto &coldata : info) {
        if (get_column_info(tupdesc, coldata)) {
            switch (coldata.eType) {
                case ANY_INTEGER:
                case TINTERVAL:
                case ANY_UINT:
                    check_any_integer_type(coldata);
                    break;
                case ANY_NUMERICAL:
                    check_any_numerical_type(coldata);
                    break;
                case TEXT:
                    check_text_type(coldata);
                    break;
                case CHAR1:
                    check_char_type(coldata);
                    break;
                case ANY_INTEGER_ARRAY:
                case ANY_POSITIVE_ARRAY:
                    check_any_integerarray_type(coldata);
                    break;
                case POSITIVE_INTEGER:
                case INTEGER:
                    check_integer_type(coldata);
                    break;
                case JSONB:
                    check_jsonb_type(coldata);
                    break;
                case INTEGER_ARRAY:
                case ANY_UINT_ARRAY:
                    check_integerarray_type(coldata);
                    break;
                case TIMESTAMP:
                    check_timestamp_type(coldata);
                    break;
                case INTERVAL:
                    check_interval_type(coldata);
                    break;
                default:
                    throw std::string("Case not found in column '") + coldata.name + "' Please inform the developers";
            }
        }
    }
}


/**
 * @param [in] tuple from postgres
 * @param [in] tupdesc from postgres
 * @param [in] info about the column been fetched
 * @param [in] opt_value default value when the column does not exist
 *
 * @returns The value found
 * @returns opt_value when the column does not exist
 *
 * Used with vrprouting::ANY_NUMERICAL
 */
double
get_anynumerical(const HeapTuple tuple, const TupleDesc &tupdesc, const Info &info, double opt_value) {
    return column_found(info)? getFloat8(tuple, tupdesc, info) : opt_value;
}


/**
 * @param [in] tuple
 * @param [in] tupdesc
 * @param [in] info about the column been fetched
 * @param [in] opt_value default value when the column does not exist
 *
 * @returns The value found
 * @returns opt_value when the column does not exist
 */
char
get_char(const HeapTuple tuple, const TupleDesc &tupdesc, const Info &info, char opt_value) {
    return getChar(tuple, tupdesc, info, opt_value);
}


/**
 * @param [in] tuple
 * @param [in] tupdesc
 * @param [in] info about the column been fetched
 *
 * @returns "{}" (empty jsonb) when when the column does not exist
 */
std::string get_jsonb(const HeapTuple tuple, const TupleDesc &tupdesc,  const vrprouting::Info &info) {
    return column_found(info)? DatumGetCString(SPI_getvalue(tuple, tupdesc, info.colNumber)) : "{}";
}

std::unordered_set<uint32_t>
get_uint_unordered_set(const HeapTuple tuple, const TupleDesc &tupdesc, const Info &info) {
    bool is_null = false;
    Datum raw_array = SPI_getbinval(tuple, tupdesc, info.colNumber, &is_null);
    if (!raw_array) return  std::unordered_set<uint32_t>();

    ArrayType *pg_array = DatumGetArrayTypeP(raw_array);

    return pgarray_to_unordered_set(pg_array);
}

/**
 * Steps:
 * 1) Similar to: https://doxygen.postgresql.org/backend_2utils_2adt_2timestamp_8c.html#a52973f03ed8296b632d4028121f7e077
 * 2) Using time.h to convert
 *
 * from time.h
 * struct tm
 * timezone
 */
TTimestamp
get_timestamp_without_timezone(TTimestamp timestamp) {
    /*
     * step 1
     */
    Timestamp date;
    Timestamp time = timestamp;
    TMODULO(time, date, USECS_PER_DAY);
    if (time < INT64CONST(0)) {
        time += USECS_PER_DAY;
        date -= 1;
    }
    date += POSTGRES_EPOCH_JDATE;
    /* Julian day routine does not work for negative Julian days */
    if (date < 0 || date > (Timestamp) INT_MAX) {
        ereport(ERROR,
                (errcode(ERRCODE_INTERNAL_ERROR),
                 errmsg("Julian day routine does not work for negative Julian days")));
    }

    /*
     * using structure from time.h to store values
     */
    struct tm info;
    fsec_t fsec;

    /*
     * calling postgres functions
     */
    j2date(static_cast<int>(date), &info.tm_year, &info.tm_mon, &info.tm_mday);
    dt2time(time, &info.tm_hour, &info.tm_min, &info.tm_sec, &fsec);

    /*
     * adjust values before calling mktime
     */
    info.tm_isdst = -1;
    info.tm_year = info.tm_year - 1900;
    info.tm_mon = info.tm_mon - 1;

  /*
   * mktime & timezone are defined in time.h
   */
  return mktime(&info) - timezone;
}


}  // namespace vrprouting
