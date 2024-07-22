/*PGR-GNU*****************************************************************
File: bin_packing.sql

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

-- v0.4
CREATE OR REPLACE FUNCTION vrp_bin_packing(
  inner_query TEXT, -- weights SQL
  bin_capacity INTEGER, -- Bin Capacity
  max_rows INTEGER = 100000 -- Maximum number of rows to be fetched. Default is value = 100000.
)
RETURNS TABLE(bin INTEGER, id INTEGER)
AS $$
  try:
    from ortools.linear_solver import pywraplp
  except Exception as err:
    plpy.error(err)

  global max_rows
  if inner_query == None:
    raise Exception('Inner Query Cannot be NULL')
  if bin_capacity == None:
    raise Exception('Capacity Cannot be NULL')
  if max_rows == None:
    max_rows = 100000

  # Program Execution Starts here

  try:
    inner_query_result = plpy.execute(inner_query, max_rows)
    num_of_rows = inner_query_result.nrows()
    colnames = inner_query_result.colnames()
    coltypes = inner_query_result.coltypes()
  except plpy.SPIError as error_msg:
    plpy.error("Error Processing Inner Query. The given query is not a valid SQL command")

  if len(colnames) != 2:
    plpy.error("Expected 2 column, Got ", len(colnames))
  if ('weight' in colnames) and ('id' in colnames):
    # got expected column names
    pass
  else:
    plpy.error("Expected column weight, Got ", colnames)
  if all(item in [20, 21, 23] for item in coltypes):
    # got expected column types
    pass
  else:
    raise Exception("Returned columns of different type. Expected Integer")

  data = {}
  weights = []
  data_ids = []

  for i in range(num_of_rows):
    weights.append(inner_query_result[i]["weight"])
    data_ids.append(inner_query_result[i]["id"])


  data['weights'] = weights
  data['items'] = list(range(len(weights)))
  data['bins'] = data['items']
  data['bin_capacity'] = bin_capacity

  try:
    solver = pywraplp.Solver.CreateSolver('SCIP')
  except:
    plpy.error("Unable to Initialize solver")

  if solver is None:
    plpy.error('SCIP solver unavailable.')


  x = {}
  for i in data['items']:
    for j in data['bins']:
      x[(i, j)] = solver.IntVar(0, 1, 'x_%i_%i' % (i, j))

  y = {}
  for j in data['bins']:
     y[j] = solver.IntVar(0, 1, 'y[%i]' % j)

  for i in data['items']:
    solver.Add(sum(x[i, j] for j in data['bins']) == 1)

  for j in data['bins']:
    solver.Add(sum(x[(i, j)] * data['weights'][i]
    for i in data['items']) <= y[j] * data['bin_capacity'])

  solver.Minimize(solver.Sum([y[j] for j in data['bins']]))

  status = solver.Solve()

  if status == pywraplp.Solver.OPTIMAL:
    for j in data['bins']:
      if y[j].solution_value() == 1:
        bin_items = []
        bin_weight = 0
        for i in data['items']:
          if x[i, j].solution_value() > 0:
            bin_items.append(i)
            bin_weight += data['weights'][i]
        if bin_weight > 0:
          for k in range(len(bin_items)):
            yield(j+1, data_ids[bin_items[k]])
  else:
    plpy.notice('The problem does not have an optimal solution')

  # end of the program

$$ LANGUAGE plpython3u VOLATILE;

COMMENT ON FUNCTION vrp_bin_packing(TEXT, INTEGER, INTEGER)
IS 'vrp_bin_packing
- Documentation:
  - ${PROJECT_DOC_LINK}/vrp_bin_packing.html
';
