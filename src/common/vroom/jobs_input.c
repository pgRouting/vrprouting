/*PGR-GNU*****************************************************************
File: jobs_input.c

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

#include "c_common/vroom/jobs_input.h"

/*
.. vrp_vroom start

A ``SELECT`` statement that returns the following columns:

::

    id, location_index
    [, service, delivery, pickup, skills, priority]


====================  =========================  =========== ================================================
Column                Type                       Default     Description
====================  =========================  =========== ================================================
**id**                ``ANY-INTEGER``                        Non-negative unique identifier of the job.

**location_index**    ``ANY-INTEGER``                        Non-negative identifier of the job location.

**service**           ``INTEGER``                0           Job service duration, in seconds

**delivery**          ``ARRAY[ANY-INTEGER]``                 Array of non-negative integers describing
                                                             multidimensional quantities for delivery such
                                                             as number of items, weight, volume etc.

                                                             - All jobs must have the same value of
                                                               :code:`array_length(delivery, 1)`

**pickup**            ``ARRAY[ANY-INTEGER]``                 Array of non-negative integers describing
                                                             multidimensional quantities for pickup such as
                                                             number of items, weight, volume etc.

                                                             - All jobs must have the same value of
                                                               :code:`array_length(pickup, 1)`

**skills**            ``ARRAY[INTEGER]``                     Array of non-negative integers defining
                                                             mandatory skills.

**priority**          ``INTEGER``                0           Priority level of the job

                                                             - Ranges from ``[0, 100]``
====================  =========================  =========== ================================================

Where:

:ANY-INTEGER: SMALLINT, INTEGER, BIGINT

.. vrp_vroom end
*/

static
void fetch_jobs(
    HeapTuple *tuple,
    TupleDesc *tupdesc,
    Column_info_t *info,
    Vroom_job_t *job) {
  job->id = get_Idx(tuple, tupdesc, info[0], 0);
  job->location_index = get_MatrixIndex(tuple, tupdesc, info[1], 0);
  job->service = get_Duration(tuple, tupdesc, info[2], 0);

  /*
   * The deliveries
   */
  job->delivery_size = 0;
  job->delivery = column_found(info[3].colNumber) ?
    spi_getPositiveBigIntArr_allowEmpty(tuple, tupdesc, info[3], &job->delivery_size)
    : NULL;

  /*
   * The pickups
   */
  job->pickup_size = 0;
  job->pickup = column_found(info[4].colNumber) ?
    spi_getPositiveBigIntArr_allowEmpty(tuple, tupdesc, info[4], &job->pickup_size)
    : NULL;

  job->skills_size = 0;
  job->skills = column_found(info[5].colNumber) ?
    spi_getPositiveIntArr_allowEmpty(tuple, tupdesc, info[5], &job->skills_size)
    : NULL;

  job->priority = get_Priority(tuple, tupdesc, info[6], 0);
}


static
void db_get_jobs(
    char *jobs_sql,
    Vroom_job_t **jobs,
    size_t *total_jobs,

    Column_info_t *info,
    const int column_count) {
#ifdef PROFILE
  clock_t start_t = clock();
  PGR_DBG("%s", jobs_sql);
#endif

  const int tuple_limit = 1000000;

  size_t total_tuples;

  void *SPIplan;
  SPIplan = pgr_SPI_prepare(jobs_sql);
  Portal SPIportal;
  SPIportal = pgr_SPI_cursor_open(SPIplan);

  bool moredata = true;
  (*total_jobs) = total_tuples = 0;

  /* on the first tuple get the column numbers */

  while (moredata == true) {
    SPI_cursor_fetch(SPIportal, true, tuple_limit);
    if (total_tuples == 0) {
      pgr_fetch_column_info(info, column_count);
    }
    size_t ntuples = SPI_processed;
    total_tuples += ntuples;
    if (ntuples > 0) {
      if ((*jobs) == NULL)
        (*jobs) = (Vroom_job_t *)palloc0(
            total_tuples * sizeof(Vroom_job_t));
      else
        (*jobs) = (Vroom_job_t *)repalloc(
            (*jobs),
            total_tuples * sizeof(Vroom_job_t));

      if ((*jobs) == NULL) {
        elog(ERROR, "Out of memory");
      }

      size_t t;
      SPITupleTable *tuptable = SPI_tuptable;
      TupleDesc tupdesc = SPI_tuptable->tupdesc;
      for (t = 0; t < ntuples; t++) {
        HeapTuple tuple = tuptable->vals[t];
        fetch_jobs(&tuple, &tupdesc, info,
            &(*jobs)[total_tuples - ntuples + t]);
      }
      SPI_freetuptable(tuptable);
    } else {
      moredata = false;
    }
  }

  SPI_cursor_close(SPIportal);

  if (total_tuples == 0) {
    (*total_jobs) = 0;
    return;
  }

  (*total_jobs) = total_tuples;
#ifdef PROFILE
  time_msg("reading jobs", start_t, clock());
#endif
}


/**
 * @param[in] sql SQL query to execute
 * @param[out] rows C Container that holds the data
 * @param[out] total_rows Total rows recieved
 */
void
get_vroom_jobs(
    char *sql,
    Vroom_job_t **rows,
    size_t *total_rows) {
  int kColumnCount = 7;
  Column_info_t info[kColumnCount];

  for (int i = 0; i < kColumnCount; ++i) {
    info[i].colNumber = -1;
    info[i].type = 0;
    info[i].strict = false;
    info[i].eType = ANY_INTEGER;
  }

  info[0].name = "id";
  info[1].name = "location_index";
  info[2].name = "service";
  info[3].name = "delivery";
  info[4].name = "pickup";
  info[5].name = "skills";
  info[6].name = "priority";

  info[2].eType = INTEGER;            // service
  info[3].eType = ANY_INTEGER_ARRAY;  // delivery
  info[4].eType = ANY_INTEGER_ARRAY;  // pickup
  info[5].eType = INTEGER_ARRAY;      // skills
  info[6].eType = INTEGER;            // priority

  /* Only id and location_index are mandatory */
  info[0].strict = true;
  info[1].strict = true;

  db_get_jobs(sql, rows, total_rows, info, kColumnCount);
}
