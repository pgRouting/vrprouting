/*PGR-GNU*****************************************************************
File: multiple_knapsack.sql

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

    vrp_multiple_knapsack(
      Weights_Costs SQL, capacities ARRAY[ANY-INTEGER], [, max_rows])

    RETURNS SET OF
    (knapsack_number, item_id)

signature end

parameters start

============================== ======================= =========================================================
Parameter                      Type                    Description
============================== ======================= =========================================================
**Weights_Costs SQL**                ``TEXT``                `Weights_Costs SQL`_ query describing the weight of each item
**Capacities**                 ``ARRAY[ANY-INTEGER]``  An array describing the capacity of each knapsack
============================== ======================= =========================================================

parameters end

optional parameters start

===================== ================ ============================= =================================================
Parameter             Type             Default                       Description
===================== ================ ============================= =================================================
**max_rows**          ``ANY-INTEGER``  :math:`100000`                Maximum items(rows) to fetch from bin_packing_data
                                                                     table
===================== ================ ============================= =================================================

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

    (knapsack_number, item_id)

=================== ================= =================================================
Column              Type              Description
=================== ================= =================================================
**knapsack_number**  ``ANY-INTEGER``  Integer to uniquely identify a knapsack

**item_id**          ``ANY-INTEGER``  Integer to uniquely identify an item in the 
                                      bin
=================== ================= =================================================
result end

**Note**:

- ANY-INTEGER: [SMALLINT, INTEGER, BIGINT]
*/

DROP FUNCTION IF EXISTS vrp_multiple_knapsack CASCADE;
-- DROP TABLE IF EXISTS multiple_knapsack_data CASCADE;

-- CREATE TABLE multiple_knapsack_data(
--   id INTEGER,
--   weight INTEGER,
--   cost INTEGER);

-- INSERT INTO multiple_knapsack_data (id, weight,  cost)
-- VALUES
-- (1, 48, 10),
-- (2, 30, 30),
-- (3, 42, 25),
-- (4, 36, 50),
-- (5, 36, 35),
-- (6, 48, 30), 
-- (7, 42, 15), 
-- (8, 42, 40),
-- (9, 36, 30),
-- (10, 24, 35), 
-- (11, 30, 45), 
-- (12, 30, 10), 
-- (13, 42, 20), 
-- (14, 36, 30), 
-- (15, 36, 25);


CREATE OR REPLACE FUNCTION vrp_multiple_knapsack(
  inner_query TEXT, -- weights_cost SQL
  capacities INTEGER[], -- ARRAY of Knapsack Capacities
  max_rows INTEGER = 100000 -- Maximum number of rows to be fetched. Default value is 100000.
)
RETURNS TABLE(knapsack_number INTEGER, item_id INTEGER)
AS $$
  try:
    from ortools.linear_solver import pywraplp
  except Exception as err:
    plpy.error(err)
  
  global max_rows
  if inner_query == None:
    raise Exception('Inner Query Cannot be NULL')
  if capacities == None:
    raise Exception('Capacity Cannot be NULL')
  if max_rows == None:
    max_rows = 100000
  
  data = {}
  data['values'] = []
  data['weights'] = []
  data_ids = []

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
  if ('weight' in colnames) and ('cost' in colnames) and ('cost' in colnames):
    # got expected column names
    pass
  else:
    plpy.error("Expected columns weight and cost, Got ", colnames)
  if all(item in [20, 21, 23] for item in coltypes):
    # got expected column types
    pass
  else:
    raise Exception("Returned columns of different type. Expected Integer, Integer")
    
  for i in range(num_of_rows):
    data['values'].append(inner_query_result[i]["cost"])
    data['weights'].append(inner_query_result[i]["weight"])
    data_ids.append(inner_query_result[i]["id"])
    

  data['num_items'] = len(data['weights'])
  data['all_items'] = range(data['num_items'])
    
  data['bin_capacities'] = capacities
  data['num_bins'] = len(data['bin_capacities'])
  data['all_bins'] = range(data['num_bins'])
  
  try:
    solver = pywraplp.Solver.CreateSolver('SCIP')
  except:
    plpy.error("Unable to Initialize solver")

  if solver is None:
    plpy.error('SCIP solver unavailable.')
  
  x = {}
  for i in data['all_items']:
    for b in data['all_bins']:
        x[i, b] = solver.BoolVar(f'x_{i}_{b}')

  for i in data['all_items']:
    solver.Add(sum(x[i, b] for b in data['all_bins']) <= 1)

  for b in data['all_bins']:
    solver.Add(sum(x[i, b] * data['weights'][i] for i in data['all_items']) <= data['bin_capacities'][b])

  objective = solver.Objective()
  for i in data['all_items']:
    for b in data['all_bins']:
        objective.SetCoefficient(x[i, b], data['values'][i])
  objective.SetMaximization()

  status = solver.Solve()

  if status == pywraplp.Solver.OPTIMAL:
    for b in data['all_bins']:
        for i in data['all_items']:
            if x[i, b].solution_value() > 0:
                yield(b+1, data_ids[i])
  else:
    plpy.notice('The problem does not have an optimal solution.')
  # end of the program
$$ LANGUAGE plpython3u VOLATILE;

-- SELECT * FROM vrp_multiple_knapsack('SELECT id, weight, cost FROM multiple_knapsack_data', ARRAY[100,100,100,100,100]);

-- COMMENTS

COMMENT ON FUNCTION vrp_multiple_knapsack(TEXT, INTEGER[], INTEGER)
IS 'vrp_multiple_knapsack
- Documentation:
  - ${PROJECT_DOC_LINK}/vrp_multiple_knapsack.html
';
