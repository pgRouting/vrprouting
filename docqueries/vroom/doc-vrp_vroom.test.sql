\echo -- q1
SELECT *
FROM vrp_vroom(
  'SELECT * FROM vroom.jobs',
  'SELECT * FROM vroom.jobs_time_windows',
  'SELECT * FROM vroom.shipments',
  'SELECT * FROM vroom.shipments_time_windows',
  'SELECT * FROM vroom.vehicles',
  'SELECT * FROM vroom.breaks',
  'SELECT * FROM vroom.breaks_time_windows',
  'SELECT * FROM vroom.matrix'
);
\echo -- q2
SELECT *
FROM vrp_vroom(
  $jobs$
    SELECT * FROM (
      VALUES (1414, 2), (1515, 3)
    ) AS C(id, location_index)
  $jobs$,
  NULL,
  $shipments$
    SELECT * FROM (
      VALUES (100, 1, 4)
    ) AS C(id, p_location_index, d_location_index)
  $shipments$,
  NULL,
  $vehicles$
    SELECT * FROM (
      VALUES (1, 1, 4)
    ) AS C(id, start_index, end_index)
  $vehicles$,
  NULL,
  NULL,
  $matrix$
    SELECT * FROM (
      VALUES (1, 2, 2104), (1, 3, 197), (1, 4, 1299),
             (2, 1, 2103), (2, 3, 2255), (2, 4, 3152),
             (3, 1, 197), (3, 2, 2256), (3, 4, 1102),
             (4, 1, 1299), (4, 2, 3153), (4, 3, 1102)
    ) AS C(start_vid, end_vid, agg_cost)
  $matrix$
);
\echo -- q3
