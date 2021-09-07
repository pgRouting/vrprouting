/*PGR-GNU*****************************************************************
File: get_check_data.c

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

#include <stdbool.h>
#include <time.h>
#include "c_common/postgres_connection.h"
#include <utils/date.h>  // NOLINT [build/include_order]
#include <utils/datetime.h>  // NOLINT [build/include_order]

#include "c_common/get_check_data.h"
#include "c_common/arrays_input.h"

#include "catalog/pg_type.h"

#include "c_common/debug_macro.h"
#include "c_types/typedefs.h"

static
void
check_interval_type(Column_info_t info) {
  if (!(info.type == 1186)) {
    elog(ERROR,
        "Unexpected Column '%s' type. Expected INTERVAL",
        info.name);
  }
}

/*
 * [BPCHAROID](https://doxygen.postgresql.org/include_2catalog_2pg__type_8h.html#afa7749dbe36d31874205189d9d6b21d7)
 * [INT2ARRAYOID](https://doxygen.postgresql.org/include_2catalog_2pg__type_8h.html#ac265fe7b0bb75fead13b16bf072722e9)
 */
static
void
check_char_type(Column_info_t info) {
  if (!(info.type == BPCHAROID)) {
    elog(ERROR, "Unexpected Column '%s' type. Expected CHAR", info.name);
  }
}

static
void
check_text_type(Column_info_t info) {
  if (!(info.type == TEXTOID)) {
    elog(ERROR, "Unexpected Column '%s' type. Expected TEXT", info.name);
  }
}

static
void
check_integer_type(Column_info_t info) {
  if (!(info.type == INT2OID || info.type == INT4OID)) {
    ereport(ERROR,
            (errmsg_internal("Unexpected type in column '%s'.", info.name),
             errhint("Expected SMALLINT or INTEGER")));
  }
}

static
void
check_any_integer_type(Column_info_t info) {
  if (!(info.type == INT2OID
        || info.type == INT4OID
        || info.type == INT8OID)) {
    ereport(ERROR,
        (errmsg_internal("Unexpected type in column '%s'.", info.name),
         errhint("Expected ANY-INTEGER")));
  }
}

static
void
check_integerarray_type(Column_info_t info) {
  if (!(info.type == INT2ARRAYOID
        || info.type == INT4ARRAYOID)) {
    elog(ERROR,
        "Unexpected Column '%s' type. Expected SMALLINT-ARRAY or INTEGER-ARRAY",
        info.name);
  }
}

static
void
check_any_integerarray_type(Column_info_t info) {
  if (!(info.type == INT2ARRAYOID
        || info.type == INT4ARRAYOID
        || info.type == 1016)) {
    elog(ERROR,
        "Unexpected Column '%s' type. Expected ANY-INTEGER-ARRAY",
        info.name);
  }
}

static
void
check_any_numerical_type(Column_info_t info) {
  if (!(info.type == INT2OID
        || info.type == INT4OID
        || info.type == INT8OID
        || info.type == FLOAT4OID
        || info.type == FLOAT8OID
        || info.type == NUMERICOID)) {
    ereport(ERROR,
        (errmsg_internal("Unexpected type in column '%s'.", info.name),
         errhint("Found: %lu\nExpected ANY-NUMERICAL", info.type)));
  }
}

static
void
check_timestamp_type(Column_info_t info) {
  if (!(info.type == 1114)) {
    elog(ERROR,
        "Unexpected Column '%s' type. Expected TIMESTAMP",
        info.name);
  }
}

static
bool
fetch_column_info(
    Column_info_t *info) {
  /*
   * [SPI_fnumber](https://www.postgresql.org/docs/12/spi-spi-fnumber.html)
   */
  info->colNumber =  SPI_fnumber(SPI_tuptable->tupdesc, info->name);
  if (info->strict && !column_found(info->colNumber)) {
    elog(ERROR, "Column '%s' not Found", info->name);
  }

  if (column_found(info->colNumber)) {
    /*
     * [SPI_gettypeid](https://www.postgresql.org/docs/12/spi-spi-gettypeid.html)
     */
    (info->type) = SPI_gettypeid(SPI_tuptable->tupdesc, (info->colNumber));
    PGR_DBG("%s %ld", info->name, info->type);
    if (SPI_result == SPI_ERROR_NOATTRIBUTE) {
      elog(ERROR, "Type of column '%s' not Found", info->name);
    }
    return true;
  }
  return false;
}

static
int32_t
spi_getInt(HeapTuple *tuple, TupleDesc *tupdesc, Column_info_t info) {
  Datum binval;
  bool isnull;
  int32_t value = 0;
  binval = SPI_getbinval(*tuple, *tupdesc, info.colNumber, &isnull);

  if (isnull) elog(ERROR, "Unexpected Null value in column %s", info.name);

  switch (info.type) {
    case INT2OID:
      value = (int32_t) DatumGetInt16(binval);
      break;
    case INT4OID:
      value = (int32_t) DatumGetInt32(binval);
      break;
    default:
    ereport(ERROR,
        (errmsg_internal("Unexpected type in column '%s'.", info.name),
         errhint("Found: %lu\nExpected INTEGER", info.type)));
  }
  return value;
}

static
int64_t
spi_getBigInt(HeapTuple *tuple, TupleDesc *tupdesc, Column_info_t info) {
  Datum binval;
  bool isnull;
  int64_t value = 0;
  binval = SPI_getbinval(*tuple, *tupdesc, info.colNumber, &isnull);

  if (isnull) elog(ERROR, "Unexpected Null value in column %s", info.name);

  switch (info.type) {
    case INT2OID:
      value = (int64_t) DatumGetInt16(binval);
      break;
    case INT4OID:
      value = (int64_t) DatumGetInt32(binval);
      break;
    case INT8OID:
      value = DatumGetInt64(binval);
      break;
    default:
    ereport(ERROR,
        (errmsg_internal("Unexpected type in column '%s'.", info.name),
         errhint("Found: %lu\nExpected ANY-INTEGER", info.type)));
  }
  return value;
}

static
TTimestamp
spi_getTimeStamp(HeapTuple *tuple, TupleDesc *tupdesc, Column_info_t info) {
  Datum binval;
  bool isnull;
  TTimestamp value = 0;
  binval = SPI_getbinval(*tuple, *tupdesc, info.colNumber, &isnull);

  if (isnull) elog(ERROR, "Unexpected Null value in column %s", info.name);

  switch (info.type) {
    case 1114:
      value = timestamp_without_timezone((TTimestamp) Int64GetDatum(binval));
      break;
    default:
      elog(ERROR,
          "[SPI_getTimeStamp] Unexpected type in column %s. found %ld expected 1114",
          info.name, info.type);
  }
  return value;
}

static
TInterval
spi_getInterval(HeapTuple *tuple, TupleDesc *tupdesc, Column_info_t info) {
  Datum binval;
  bool isnull;
  Interval*   interval;

  binval = SPI_getbinval(*tuple, *tupdesc, info.colNumber, &isnull);

  if (isnull) elog(ERROR, "Unexpected Null value in column %s", info.name);

  switch (info.type) {
    case INTERVALOID:
      interval = DatumGetIntervalP(binval);
      break;
    default:
      elog(ERROR,
          "[spi_getInterval] Unexpected type in column %s. found %ld expected %d",
          info.name, info.type, INTERVALOID);
  }
  PGR_DBG("time %ld secs %ld", interval->time,
      interval->time / 1000000
      + interval->day * SECS_PER_DAY
      + (int64_t)(interval->month * ((DAYS_PER_YEAR / (double) MONTHS_PER_YEAR) * SECS_PER_DAY)));

  return interval->time / 1000000
    + interval->day * SECS_PER_DAY
    + (int64_t)(interval->month * ((DAYS_PER_YEAR / (double) MONTHS_PER_YEAR) * SECS_PER_DAY));
}

/*
  @param[in]  tuple         input row to be examined.
  @param[in]  tupdesc       input row description.
  @param[in]  info          contain column information.
  @param[in]  default_value returned when column contain NULL value.

  @throw ERROR Unexpected Column type. Expected column type is CHAR.
  @throw ERROR When value of column is NULL.

  @return Char type of column value is returned.

 * http://doxygen.postgresql.org/include_2catalog_2pg__type_8h.html;
 * [SPI_getbinval](https://www.postgresql.org/docs/8.1/static/spi-spi-getbinval.html)
 * [Datum](https://doxygen.postgresql.org/datum_8h.html)
 * [DatumGetInt16](https://doxygen.postgresql.org/postgres_8h.html#aec991e04209850f29a8a63df0c78ba2d)
 */
char
spi_getChar(
    HeapTuple *tuple, TupleDesc *tupdesc, Column_info_t info, char default_value) {
  Datum binval;
  bool isNull;
  char value = default_value;

  binval = SPI_getbinval(*tuple, *tupdesc, info.colNumber, &isNull);
  if (!(info.type == BPCHAROID)) {
    elog(ERROR, "Unexpected Column type of %s. Expected CHAR", info.name);
  }
  if (!isNull) {
    value =  ((char*)binval)[1];
  } else {
    if (info.strict) {
      elog(ERROR, "Unexpected Null value in column %s", info.name);
    }
    value = default_value;
  }
  return value;
}

int64_t*
spi_getBigIntArr(
    HeapTuple *tuple,
    TupleDesc *tupdesc,
    Column_info_t info,
    size_t *the_size) {
  bool is_null = false;

  Datum raw_array = SPI_getbinval(*tuple, *tupdesc, info.colNumber, &is_null);
  /*
   * [DatumGetArrayTypeP](https://doxygen.postgresql.org/array_8h.html#aa1b8e77c103863862e06a7b7c07ec532)
   * [pgr_get_bigIntArray](http://docs.pgrouting.org/doxy/2.2/arrays__input_8c_source.html)
   */
  ArrayType *pg_array = DatumGetArrayTypeP(raw_array);

  return pgr_get_bigIntArray((size_t*)the_size, pg_array);
}

int64_t*
spi_getBigIntArr_allowEmpty(
    HeapTuple *tuple,
    TupleDesc *tupdesc,
    Column_info_t info,
    size_t *the_size) {
  bool is_null = false;

  Datum raw_array = SPI_getbinval(*tuple, *tupdesc, info.colNumber, &is_null);
  /*
   * [DatumGetArrayTypeP](https://doxygen.postgresql.org/array_8h.html#aa1b8e77c103863862e06a7b7c07ec532)
   * [pgr_get_bigIntArray](http://docs.pgrouting.org/doxy/2.2/arrays__input_8c_source.html)
   */
  if (!raw_array) {
    *the_size = 0;
    return  NULL;
  }

  ArrayType *pg_array = DatumGetArrayTypeP(raw_array);

  return pgr_get_bigIntArray_allowEmpty(the_size, pg_array);
}

int64_t*
spi_getPositiveBigIntArr_allowEmpty(
    HeapTuple *tuple,
    TupleDesc *tupdesc,
    Column_info_t info,
    size_t *the_size) {
  int64_t *array = spi_getBigIntArr_allowEmpty(tuple, tupdesc, info, the_size);
  for (size_t i = 0; i < *the_size; i++) {
    if (array[i] < 0) {
      elog(ERROR, "Unexpected Negative value %ld in array", array[i]);
    }
  }
  return array;
}


uint32_t*
spi_getPositiveIntArr_allowEmpty(
    HeapTuple *tuple,
    TupleDesc *tupdesc,
    Column_info_t info,
    size_t *the_size) {
  bool is_null = false;

  Datum raw_array = SPI_getbinval(*tuple, *tupdesc, info.colNumber, &is_null);
  if (!raw_array) {
    *the_size = 0;
    return  NULL;
  }

  ArrayType *pg_array = DatumGetArrayTypeP(raw_array);

  return pgr_get_positiveIntArray_allowEmpty(the_size, pg_array);
}


/**
 * @params [in] tuple
 * @params [in] tupdesc
 * @params [in] info about the column been fetched
 * @params [in] opt_value default value when the column does not exist
 *
 * @returns The value found
 * @returns opt_value when the column does not exist
 */
TTimestamp
get_TTimestamp_plain(HeapTuple *tuple, TupleDesc *tupdesc, Column_info_t info, TTimestamp opt_value) {
  return column_found(info.colNumber)?
    (TTimestamp)spi_getBigInt(tuple, tupdesc, info)
    : opt_value;
}

/**
 * @params [in] tuple
 * @params [in] tupdesc
 * @params [in] info about the column been fetched
 * @params [in] opt_value default value when the column does not exist
 *
 * @returns The value found
 * @returns opt_value when the column does not exist
 *
 * exceptions when the value is negative
 * @pre for positive values only
 */
TTimestamp
get_PositiveTTimestamp_plain(HeapTuple *tuple, TupleDesc *tupdesc, Column_info_t info, TTimestamp opt_value) {
  TTimestamp value = get_TTimestamp_plain(tuple, tupdesc, info, opt_value);
  if (value < 0) elog(ERROR, "Unexpected Negative value in column %s", info.name);
  return value;
}

/**
 * @params [in] tuple
 * @params [in] tupdesc
 * @params [in] info about the column been fetched
 * @params [in] opt_value default value when the column does not exist
 *
 * @returns The value found
 * @returns opt_value when the column does not exist
 */
TTimestamp
get_TTimestamp(HeapTuple *tuple, TupleDesc *tupdesc, Column_info_t info, TTimestamp opt_value) {
  return column_found(info.colNumber)?
    spi_getTimeStamp(tuple, tupdesc, info)
    : opt_value;
}

/**
 * @params [in] tuple
 * @params [in] tupdesc
 * @params [in] info about the column been fetched
 * @params [in] opt_value default value when the column does not exist
 *
 * @returns The value found
 * @returns opt_value when the column does not exist
 *
 * exceptions when the value is negative
 * @pre for positive values only
 */
TTimestamp
get_PositiveTTimestamp(HeapTuple *tuple, TupleDesc *tupdesc, Column_info_t info, TTimestamp opt_value) {
  TTimestamp value = get_TTimestamp(tuple, tupdesc, info, opt_value);
  if (value < 0) elog(ERROR, "Unexpected Negative value in column %s", info.name);
  return value;
}

/**
 * @params [in] tuple
 * @params [in] tupdesc
 * @params [in] info about the column been fetched
 * @params [in] opt_value default value when the column does not exist
 *
 * @returns The value found
 * @returns opt_value when the column does not exist
 */
TInterval
get_TInterval(HeapTuple *tuple, TupleDesc *tupdesc, Column_info_t info, TInterval opt_value) {
  return column_found(info.colNumber)?
    (TInterval)spi_getInterval(tuple, tupdesc, info)
    : opt_value;
}

/**
 * @params [in] tuple
 * @params [in] tupdesc
 * @params [in] info about the column been fetched
 * @params [in] opt_value default value when the column does not exist
 *
 * @returns The value found
 * @returns opt_value when the column does not exist
 *
 * exceptions when the value is negative
 * @pre for positive values only
 */
TInterval
get_PositiveTInterval(HeapTuple *tuple, TupleDesc *tupdesc, Column_info_t info, TInterval opt_value) {
  TInterval value = get_TInterval(tuple, tupdesc, info, opt_value);
  if (value < 0) elog(ERROR, "Unexpected Negative value in column %s", info.name);
  return (TInterval) value;
}

/**
 * @params [in] tuple
 * @params [in] tupdesc
 * @params [in] info about the column been fetched
 * @params [in] opt_value default value when the column does not exist
 *
 * @returns The value found
 * @returns opt_value when the column does not exist
 */
TInterval
get_TInterval_plain(HeapTuple *tuple, TupleDesc *tupdesc, Column_info_t info, TInterval opt_value) {
  return column_found(info.colNumber)?
    (TInterval)spi_getBigInt(tuple, tupdesc, info)
    : opt_value;
}

/**
 * @params [in] tuple
 * @params [in] tupdesc
 * @params [in] info about the column been fetched
 * @params [in] opt_value default value when the column does not exist
 *
 * @returns The value found
 * @returns opt_value when the column does not exist
 *
 * exceptions when the value is negative
 * @pre for positive values only
 */
TInterval
get_PositiveTInterval_plain(HeapTuple *tuple, TupleDesc *tupdesc, Column_info_t info, TInterval opt_value) {
  TInterval value = get_TInterval_plain(tuple, tupdesc, info, opt_value);
  if (value < 0) elog(ERROR, "Unexpected Negative value in column %s", info.name);
  return (TInterval) value;
}

/**
 * @params [in] tuple
 * @params [in] tupdesc
 * @params [in] info about the column been fetched
 * @params [in] opt_value default value when the column does not exist
 *
 * @returns The value found
 * @returns opt_value when the column does not exist
 */
Id
get_Id(HeapTuple *tuple, TupleDesc *tupdesc, Column_info_t info, Id opt_value) {
  return column_found(info.colNumber)?
    (Id)spi_getBigInt(tuple, tupdesc, info)
    : opt_value;
}

/**
 * @params [in] tuple
 * @params [in] tupdesc
 * @params [in] info about the column been fetched
 * @params [in] opt_value default value when the column does not exist
 *
 * @returns The value found
 * @returns opt_value when the column does not exist
 *
 * exceptions when the value is negative
 * @pre for non-negative values only
 */
Idx
get_Idx(HeapTuple *tuple, TupleDesc *tupdesc, Column_info_t info, Idx opt_value) {
  Id value = get_Id(tuple, tupdesc, info, 0);
  if (value < 0) elog(ERROR, "Unexpected Negative value in column %s", info.name);
  return column_found(info.colNumber)? (Idx) value : opt_value;
}

/**
 * @params [in] tuple
 * @params [in] tupdesc
 * @params [in] info about the column been fetched
 * @params [in] opt_value default value when the column does not exist
 *
 * @returns The value found
 * @returns opt_value when the column does not exist
 */
StepType get_StepType(HeapTuple *tuple, TupleDesc *tupdesc, Column_info_t info,
                  StepType opt_value) {
  StepType step_type = column_found(info.colNumber)
                           ? spi_getInt(tuple, tupdesc, info)
                           : opt_value;
  StepType min_value = 1;
  StepType max_value = 6;
  if (step_type < min_value || step_type > max_value) {
    elog(ERROR, "Step value should lie between %d and %d", min_value, max_value);
  }
  return step_type;
}

/**
 * @params [in] tuple
 * @params [in] tupdesc
 * @params [in] info about the column been fetched
 * @params [in] opt_value default value when the column does not exist
 *
 * @returns The value found
 * @returns opt_value when the column does not exist
 */
Amount
get_Amount(HeapTuple *tuple, TupleDesc *tupdesc, Column_info_t info, Amount opt_value) {
  return (Amount) column_found(info.colNumber)?
    spi_getBigInt(tuple, tupdesc, info)
    : opt_value;
}

/**
 * @params [in] tuple
 * @params [in] tupdesc
 * @params [in] info about the column been fetched
 * @params [in] opt_value default value when the column does not exist
 *
 * @returns The value found
 * @returns opt_value when the column does not exist
 *
 * exceptions when the value is negative
 * @pre for non-negative values only
 */
PAmount
get_PositiveAmount(HeapTuple *tuple, TupleDesc *tupdesc, Column_info_t info, PAmount opt_value) {
  Amount value = get_Amount(tuple, tupdesc, info, 0);
  if (value < 0) elog(ERROR, "Unexpected Negative value in column %s", info.name);
  return column_found(info.colNumber)? (PAmount) value : opt_value;
}


/**
 * @params [in] tuple
 * @params [in] tupdesc
 * @params [in] info about the column been fetched
 * @params [in] opt_value default value when the column does not exist
 *
 * @returns The value found
 * @returns opt_value when the column does not exist
 *
 * exceptions when the value is negative
 * @pre for non-negative values only
 */
MatrixIndex
get_MatrixIndex(HeapTuple *tuple, TupleDesc *tupdesc, Column_info_t info, MatrixIndex opt_value) {
  if (column_found(info.colNumber)) {
    int64_t value = spi_getBigInt(tuple, tupdesc, info);
    if (value < 0) elog(ERROR, "Unexpected Negative value in column %s", info.name);
    return (MatrixIndex) value;
  }
  return opt_value;
}


/**
 * @params [in] tuple
 * @params [in] tupdesc
 * @params [in] info about the column been fetched
 * @params [in] opt_value default value when the column does not exist
 *
 * @returns The value found
 * @returns opt_value when the column does not exist
 *
 * exceptions when the value is negative
 * @pre for non-negative values only
 */
Duration
get_Duration(HeapTuple *tuple, TupleDesc *tupdesc, Column_info_t info, Duration opt_value) {
  if (column_found(info.colNumber)) {
    int32_t value = spi_getInt(tuple, tupdesc, info);
    if (value < 0) elog(ERROR, "Unexpected Negative value in column %s", info.name);
    return (Duration) value;
  }
  return opt_value;
}

/**
 * @params [in] tuple
 * @params [in] tupdesc
 * @params [in] info about the column been fetched
 * @params [in] opt_value default value when the column does not exist
 *
 * @returns The value found
 * @returns opt_value when the column does not exist
 *
 */
char
get_Kind(HeapTuple *tuple, TupleDesc *tupdesc, Column_info_t info, char opt_value) {
  if (column_found(info.colNumber)) {
    char value = spi_getChar(tuple, tupdesc, info, opt_value);
    return value;
  }
  return opt_value;
}


/**
 * @params [in] tuple
 * @params [in] tupdesc
 * @params [in] info about the column been fetched
 * @params [in] opt_value default value when the column does not exist
 *
 * @returns The value found
 * @returns opt_value when the column does not exist
 *
 * exceptions when the value is negative
 * @pre for non-negative values only
 */
Priority
get_Priority(HeapTuple *tuple, TupleDesc *tupdesc, Column_info_t info, Priority opt_value) {
  if (column_found(info.colNumber)) {
    int32_t value = spi_getInt(tuple, tupdesc, info);
    if (value < 0) elog(ERROR, "Unexpected Negative value in column %s", info.name);
    if (value > 100) elog(ERROR, "Priority exceeds the max priority 100");
    return (Priority) value;
  }
  return opt_value;
}


/**
 * @params [in] tuple
 * @params [in] tupdesc
 * @params [in] info about the column been fetched
 * @params [in] opt_value default value when the column does not exist
 *
 * @returns The value found
 * @returns opt_value when the column does not exist
 *
 * exceptions when the value is negative
 * @pre for non-negative values only
 */
Distance
get_Distance(HeapTuple *tuple, TupleDesc *tupdesc, Column_info_t info, Distance opt_value) {
  if (column_found(info.colNumber)) {
    int32_t value = spi_getInt(tuple, tupdesc, info);
    if (value < 0) elog(ERROR, "Unexpected Negative value in column %s", info.name);
    return (Distance) value;
  }
  return opt_value;
}


double
spi_getFloat8(HeapTuple *tuple, TupleDesc *tupdesc, Column_info_t info) {
  Datum binval;
  bool isnull = false;
  double value = 0.0;
  binval = SPI_getbinval(*tuple, *tupdesc, info.colNumber, &isnull);
  if (isnull)
    elog(ERROR, "Unexpected Null value in column %s", info.name);

  switch (info.type) {
    case INT2OID:
      value = (double) DatumGetInt16(binval);
      break;
    case INT4OID:
      value = (double) DatumGetInt32(binval);
      break;
    case INT8OID:
      value = (double) DatumGetInt64(binval);
      break;
    case FLOAT4OID:
      value = (double) DatumGetFloat4(binval);
      break;
    case FLOAT8OID:
      value = DatumGetFloat8(binval);
      break;
    case NUMERICOID:
      /* Note: out-of-range values will be clamped to +-HUGE_VAL */
      value = (double) DatumGetFloat8(DirectFunctionCall1(numeric_float8_no_overflow, binval));
      break;
    default:
    ereport(ERROR,
        (errmsg_internal("Unexpected type in column '%s'.", info.name),
         errhint("Found: %lu\nExpected ANY-NUMERICAL", info.type)));
  }
  return value;
}

Coordinate
spi_getCoordinate(HeapTuple *tuple, TupleDesc *tupdesc, Column_info_t info, Coordinate opt_value) {
  return column_found(info.colNumber)?
    (Coordinate) spi_getFloat8(tuple, tupdesc, info)
    : opt_value;
}

/**
 * under development
 */
/*
 * [DatumGetCString](https://doxygen.postgresql.org/postgres_8h.html#ae401c8476d1a12b420e3061823a206a7)
 */
char*
spi_getText(HeapTuple *tuple, TupleDesc *tupdesc,  Column_info_t info) {
  char *val = DatumGetCString(SPI_getvalue(*tuple, *tupdesc, info.colNumber));
  if (!val) {
    elog(ERROR, "Unexpected Null value in column %s", info.name);
  }
  return val;
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
timestamp_without_timezone(TTimestamp timestamp) {
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
  j2date((int) date, &info.tm_year, &info.tm_mon, &info.tm_mday);
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


/*
 * @param[in] colNumber Column number (count starts at 1).

 * [SPI_ERROR_NOATTRIBUTE](https://doxygen.postgresql.org/spi_8h.html#ac1512d8aaa23c2d57bb0d1eb8f453ee2)
 * @return @b TRUE when colNumber exist.
 *         @b FALSE when colNumber was not found.
*/
bool
column_found(int colNumber) {
    return !(colNumber == SPI_ERROR_NOATTRIBUTE);
}


void pgr_fetch_column_info(
    Column_info_t info[],
    int info_size) {
  for (int i = 0; i < info_size; ++i) {
    if (fetch_column_info(&info[i])) {
      switch (info[i].eType) {
        case INTEGER:
          check_integer_type(info[i]);
          break;
        case ANY_INTEGER:
          check_any_integer_type(info[i]);
          break;
        case ANY_NUMERICAL:
          check_any_numerical_type(info[i]);
          break;
        case TEXT:
          check_text_type(info[i]);
          break;
        case CHAR1:
          check_char_type(info[i]);
          break;
        case INTEGER_ARRAY:
          check_integerarray_type(info[i]);
          break;
        case ANY_INTEGER_ARRAY:
          check_any_integerarray_type(info[i]);
          break;
        case TIMESTAMP:
          check_timestamp_type(info[i]);
          break;
        case INTERVAL:
          check_interval_type(info[i]);
          break;
        default:
          elog(ERROR, "Unknown type of column %s", info[i].name);
      }
    }
  }
}
