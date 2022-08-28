
/*PGR-GNU*****************************************************************
File: knapsack.sql

Copyright (c) 2022 GSoC-2022 pgRouting developers
Mail: project@pgrouting.org

Function's developer:
Copyright (c) 2022 Manas Sivakumar

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

/*
signature start

.. code-block:: none

    vrp_knapsack(
      Weights_Costs SQL, capacity ANY-INTEGER, [, max_rows])

    RETURNS SET OF
    (item_id)

signature end

parameters start

============================== ================ =========================================================
Parameter                      Type             Description
============================== ================ =========================================================
**Weights_Costs SQL**          ``TEXT``         `Weights_Costs SQL`_ query describing the weights and
                                                cost of each item
**Capacity**                   ``ANY-INTEGER``  Maximum Capacity of the knapsack.
============================== ================ =========================================================

parameters end

optional parameters start

===================== =============== ================================== =================================================
Parameter             Type            Default                            Description
===================== =============== ================================== =================================================
**max_rows**          ``ANY-INTEGER`` :math:`100000`                     Maximum items(rows) to fetch from knapsack_data
                                                                         table
===================== =============== ================================== =================================================

optional parameters end

.. Weights_Costs start

A ``SELECT`` statement that returns the following columns:

::

    id, weight, cost


====================  =========================  =========== ================================================
Column                Type                       Default     Description
====================  =========================  =========== ================================================
**id**                ``ANY-INTEGER``                        unique identifier of the item.

**weight**            ``ANY-INTEGER``                        weight of the item.

**cost**              ``ANY-INTEGER``                        cost of the item.
====================  =========================  =========== ================================================

.. Weights_Costs end

result start

Returns set of

.. code-block:: none

    (item_id)

=================== ================= =================================================
Column              Type              Description
=================== ================= =================================================
**item_id**         ``ANY-INTEGER``   Integer to uniquely identify an item in the 
                                      knapsack
=================== ================= =================================================
result end

**Note**:

- ANY-INTEGER: [SMALLINT, INTEGER, BIGINT]
*/

DROP FUNCTION IF EXISTS vrp_knapsack CASCADE;
-- DROP TABLE IF EXISTS knapsack_data;

-- CREATE TABLE knapsack_data(
--   id INTEGER,
--   weight INTEGER,
--   cost INTEGER);

-- INSERT INTO knapsack_data (id, weight,  cost)
-- VALUES
-- (1, 12, 4),
-- (2, 2, 2),
-- (3, 1, 1),
-- (4, 4, 10),
-- (5, 1, 2);

CREATE OR REPLACE FUNCTION vrp_knapsack(
  inner_query TEXT, -- weights_cost SQL
  capacity INTEGER, -- Knapsack Capacity
  max_rows INTEGER = 100000 -- Maximum number of rows to be fetched. Default is 100000.
)
RETURNS TABLE(item_id INTEGER)
AS $$
  try:
    from ortools.algorithms import pywrapknapsack_solver
  except Exception as err:
    plpy.error(err)
  
  global max_rows
  if inner_query == None:
    raise Exception('Inner Query Cannot be NULL')
  if capacity == None:
    raise Exception('Capacity Cannot be NULL')
  if max_rows == None:
    max_rows = 100000
  try:
    solver = pywrapknapsack_solver.KnapsackSolver(
    pywrapknapsack_solver.KnapsackSolver.
    KNAPSACK_MULTIDIMENSION_BRANCH_AND_BOUND_SOLVER, 'KnapsackExample')
  except:
    plpy.error('Unable to Initialize Knapsack Solver')
  
  capacities = []
  capacities.append(capacity)

  # Program Execution Starts here

  try:
    inner_query_result = plpy.execute(inner_query, max_rows)
    num_of_rows = inner_query_result.nrows()
    colnames = inner_query_result.colnames()
    coltypes = inner_query_result.coltypes()
  except plpy.SPIError as error_msg:
    plpy.error("Error Processing Inner Query. The given query is not a valid SQL command")
  
  if len(colnames) != 3:
    plpy.error("Expected 3 columns, Got ", len(colnames))
  if ('weight' in colnames) and ('cost' in colnames) and ('id' in colnames):
    # got correct column names
    pass
  else:
    plpy.error("Expected columns weight and cost, Got ", colnames) 
  if all(item in [20, 21, 23] for item in coltypes):
    # got correct column types
    pass
  else:
    raise Exception("Returned columns of different type. Expected Integer, Integer")

  ids = []
  values = []
  weight1 = []
  weights =[]
  for i in range(num_of_rows):
    ids.append(inner_query_result[i]["id"])
    values.append(inner_query_result[i]["cost"])
    weight1.append(inner_query_result[i]["weight"])
  weights.append(weight1)
  
  try:
    solver.Init(values, weights, capacities)
  except Exception as error_msg:
    plpy.error(error_msg)
  computed_value = solver.Solve()
  
  # prints results
  for i in range(len(values)):
    if solver.BestSolutionContains(i):
      yield (ids[i])

  # end of the program

$$ LANGUAGE plpython3u VOLATILE;

-- SELECT * FROM vrp_knapsack('SELECT id, weight, cost FROM knapsack_data' , 15);

-- COMMENTS

COMMENT ON FUNCTION vrp_knapsack(TEXT, INTEGER, INTEGER)
IS 'vrp_knapsack
- Documentation:
  - ${PROJECT_DOC_LINK}/vrp_knapsack.html
';
