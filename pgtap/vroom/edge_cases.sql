BEGIN;
SET search_path TO 'vroom', 'public';
SET client_min_messages TO ERROR;

SELECT CASE WHEN min_version('0.2.0') THEN plan (45) ELSE plan(1) END;

CREATE OR REPLACE FUNCTION edge_cases()
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  fn TEXT;
  start_sql TEXT;
  rest_sql TEXT;
BEGIN

  IF NOT min_version('0.2.0') THEN
    RETURN QUERY
    SELECT skip(1, 'Function is new on 0.2.0');
    RETURN;
  END IF;

  PREPARE jobs AS SELECT * FROM jobs;
  PREPARE empty_jobs AS SELECT * FROM jobs WHERE id = -1;

  PREPARE jobs_time_windows AS SELECT * FROM jobs_time_windows;
  PREPARE empty_jobs_time_windows AS SELECT * FROM jobs_time_windows WHERE id = -1;

  PREPARE shipments AS SELECT * FROM shipments;
  PREPARE empty_shipments AS SELECT * FROM shipments WHERE id = -1;

  PREPARE shipments_time_windows AS SELECT * FROM shipments_time_windows;
  PREPARE empty_shipments_time_windows AS SELECT * FROM shipments_time_windows WHERE id = -1;

  PREPARE vehicles AS SELECT * FROM vehicles;
  PREPARE empty_vehicles AS SELECT * FROM vehicles WHERE id = -1;

  PREPARE breaks AS SELECT * FROM breaks;
  PREPARE empty_breaks AS SELECT * FROM breaks WHERE id = -1;

  PREPARE breaks_time_windows AS SELECT * FROM breaks_time_windows;
  PREPARE empty_breaks_time_windows AS SELECT * FROM breaks_time_windows WHERE id = -1;

  PREPARE matrix AS SELECT * FROM matrix;
  PREPARE empty_matrix AS SELECT * FROM matrix WHERE start_id = -1;

  PREPARE vroom_sql AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );

  RETURN QUERY
  SELECT set_eq('vroom_sql',
    $$
      VALUES
      (1, 1, 1, 1, 1, -1, 300, 0, 0, 0, ARRAY[30]),
      (2, 1, 1, 2, 5, 1, 300, 0, 0, 0, ARRAY[30]),
      (3, 1, 1, 3, 2, 2, 350, 50, 250, 900, ARRAY[30]),
      (4, 1, 1, 4, 3, 5, 1500, 50, 2250, 11850, ARRAY[40]),
      (5, 1, 1, 5, 3, 3, 15650, 100, 2250, 0, ARRAY[60]),
      (6, 1, 1, 6, 4, 5, 17950, 150, 2250, 225, ARRAY[50]),
      (7, 1, 1, 7, 4, 3, 20425, 150, 2250, 200, ARRAY[30]),
      (8, 1, 1, 8, 6, -1, 22925, 200, 0, 0, ARRAY[30]),
      (9, 2, 2, 1, 1, -1, 275, 0, 0, 0, ARRAY[70]),
      (10, 2, 2, 2, 5, 2, 275, 0, 10, 0, ARRAY[70]),
      (11, 2, 2, 3, 2, 5, 360, 75, 250, 665, ARRAY[70]),
      (12, 2, 2, 4, 2, 3, 1325, 125, 250, 1400, ARRAY[70]),
      (13, 2, 2, 5, 2, 4, 2975, 125, 250, 550, ARRAY[70]),
      (14, 2, 2, 6, 6, -1, 3775, 125, 0, 0, ARRAY[70]),
      (15, 3, 3, 1, 1, -1, 0, 0, 0, 0, ARRAY[20]),
      (16, 3, 3, 2, 5, 3, 0, 0, 0, 0, ARRAY[20]),
      (17, 3, 3, 3, 2, 1, 0, 0, 250, 3625, ARRAY[20]),
      (18, 3, 3, 4, 3, 4, 3875, 0, 2250, 2500, ARRAY[40]),
      (19, 3, 3, 5, 4, 4, 8700, 75, 2250, 225, ARRAY[20]),
      (20, 3, 3, 6, 6, -1, 11250, 150, 0, 0, ARRAY[20]),
      (21, 4, 4, 1, 1, -1, 250, 0, 0, 0, ARRAY[0]),
      (22, 4, 4, 2, 5, 4, 250, 0, 0, 0, ARRAY[0]),
      (23, 4, 4, 3, 3, 2, 275, 25, 2250, 100, ARRAY[10]),
      (24, 4, 4, 4, 3, 1, 2650, 50, 2250, 0, ARRAY[20]),
      (25, 4, 4, 5, 4, 2, 4990, 140, 2250, 0, ARRAY[10]),
      (26, 4, 4, 6, 4, 1, 7351, 251, 2250, 17574, ARRAY[0]),
      (27, 4, 4, 7, 6, -1, 27200, 276, 0, 0, ARRAY[0])
    $$,
    'Query with the vroomdata'
  );

  -- tests for no jobs/shipments, no vehicles, or no matrix

  PREPARE no_jobs_and_shipments AS
  SELECT * FROM vrp_vroomPlain(
    'empty_jobs',
    'jobs_time_windows',
    'empty_shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY
  SELECT is_empty('no_jobs_and_shipments', 'Problem with no jobs and shipments');

  PREPARE no_vehicles AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'empty_vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY
  SELECT is_empty('no_vehicles', 'Problem with no vehicles');

  PREPARE no_matrix AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'empty_matrix'
  );
  RETURN QUERY
  SELECT is_empty('no_matrix', 'Problem with no cost matrix');


  -- priority range test (jobs)

  PREPARE jobs_neg_priority AS
  SELECT * FROM vrp_vroomPlain(
    'SELECT id, location_index, service, delivery, pickup, skills, -1 AS priority FROM jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY
  SELECT throws_ok(
    'jobs_neg_priority',
    'XX000',
    'Unexpected Negative value in column priority',
    'Problem with negative priority jobs'
  );

  PREPARE jobs_101_priority AS
  SELECT * FROM vrp_vroomPlain(
    'SELECT id, location_index, service, delivery, pickup, skills, 101 AS priority FROM jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY
  SELECT throws_ok(
    'jobs_101_priority',
    'XX000',
    'Priority exceeds the max priority 100',
    'Problem with > 100 priority jobs'
  );

  PREPARE jobs_zero_priority AS
  SELECT * FROM vrp_vroomPlain(
    'SELECT id, location_index, service, delivery, pickup, skills, 0 AS priority FROM jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY
  SELECT lives_ok('jobs_zero_priority', 'Problem with 0 priority jobs');

  PREPARE jobs_100_priority AS
  SELECT * FROM vrp_vroomPlain(
    'SELECT id, location_index, service, delivery, pickup, skills, 0 AS priority FROM jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY
  SELECT lives_ok('jobs_100_priority', 'Problem with 100 priority jobs');


  -- priority range tests (shipments)

  PREPARE shipments_neg_priority AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'SELECT id, p_location_index, p_service, d_location_index, d_service,
    amount, skills, -1 AS priority FROM shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY
  SELECT throws_ok(
    'shipments_neg_priority',
    'XX000',
    'Unexpected Negative value in column priority',
    'Problem with negative priority shipments'
  );

  PREPARE shipments_101_priority AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'SELECT id, p_location_index, p_service, d_location_index, d_service,
    amount, skills, 101 AS priority FROM shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY
  SELECT throws_ok(
    'shipments_101_priority',
    'XX000',
    'Priority exceeds the max priority 100',
    'Problem with > 100 priority shipments'
  );

  PREPARE shipments_zero_priority AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'SELECT id, p_location_index, p_service, d_location_index, d_service,
    amount, skills, 0 AS priority FROM shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY
  SELECT lives_ok('shipments_zero_priority', 'Problem with 0 priority shipments');

  PREPARE shipments_100_priority AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'SELECT id, p_location_index, p_service, d_location_index, d_service,
    amount, skills, 100 AS priority FROM shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY
  SELECT lives_ok('shipments_100_priority', 'Problem with 100 priority shipments');


  -- Missing id on matrix test

  PREPARE missing_id_on_matrix AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'SELECT * FROM matrix WHERE start_id != 5 AND end_id != 5'
  );
  RETURN QUERY
  SELECT throws_ok(
    'missing_id_on_matrix',
    'XX000',
    'An Infinity value was found on the Matrix. Might be missing information of a node',
    'Problem with missing node 5 on the cost matrix'
  );

  PREPARE missing_same_vid_on_matrix AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'SELECT * FROM matrix WHERE start_id != end_id'
  );
  RETURN QUERY
  SELECT set_eq('missing_same_vid_on_matrix', 'vroom_sql', 'Cost between same vertex is always 0');

  PREPARE missing_reverse_cost_on_matrix AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'SELECT * FROM matrix WHERE start_id < end_id'
  );
  RETURN QUERY
  SELECT set_eq('missing_reverse_cost_on_matrix', 'vroom_sql', 'Reverse cost is equal to the cost, if not specified');


  -- jobs time windows validate

  PREPARE jobs_equal_tw AS SELECT id, 3000 AS tw_open, 3000 AS tw_close FROM jobs_time_windows;
  PREPARE jobs_invalid_tw AS SELECT id, 3001 AS tw_open, 3000 AS tw_close FROM jobs_time_windows;

  PREPARE jobs_equal_tw_problem AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_equal_tw',
    'shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY
  SELECT lives_ok('jobs_equal_tw_problem', 'Problem with jobs having equal time windows');

  PREPARE jobs_invalid_tw_problem AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_invalid_tw',
    'shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY
  SELECT throws_ok(
    'jobs_invalid_tw_problem',
    'XX000',
    'Invalid time window (3001, 3000)',
    'Problem with jobs having invalid time windows'
  );


  -- shipments time windows validate

  PREPARE shipments_equal_tw AS SELECT id, kind, 3000 AS tw_open, 3000 AS tw_close FROM shipments_time_windows;
  PREPARE shipments_invalid_tw AS SELECT id, kind, 3001 AS tw_open, 3000 AS tw_close FROM shipments_time_windows;

  PREPARE shipments_equal_tw_problem AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_equal_tw',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY
  SELECT lives_ok('shipments_equal_tw_problem', 'Problem with shipments having equal time windows');

  PREPARE shipments_invalid_tw_problem AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_invalid_tw',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY
  SELECT throws_ok(
    'shipments_invalid_tw_problem',
    'XX000',
    'Invalid time window (3001, 3000)',
    'Problem with shipments having invalid time windows'
  );


  -- breaks time windows validate

  PREPARE breaks_equal_tw AS SELECT id, 3000 AS tw_open, 3000 AS tw_close FROM breaks_time_windows;
  PREPARE breaks_invalid_tw AS SELECT id, 3001 AS tw_open, 3000 AS tw_close FROM breaks_time_windows;

  PREPARE breaks_equal_tw_problem AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_equal_tw',
    'matrix'
  );
  RETURN QUERY
  SELECT lives_ok('breaks_equal_tw_problem', 'Problem with breaks having equal time windows');

  PREPARE breaks_invalid_tw_problem AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_invalid_tw',
    'matrix'
  );
  RETURN QUERY
  SELECT throws_ok(
    'breaks_invalid_tw_problem',
    'XX000',
    'Invalid time window (3001, 3000)',
    'Problem with breaks having invalid time windows'
  );


  -- vehicles time windows validate

  PREPARE vehicles_equal_tw AS
  SELECT id, start_index, end_index, capacity, skills, 3000 AS tw_open, 3000 AS tw_close, speed_factor FROM vehicles;
  PREPARE vehicles_invalid_tw AS
  SELECT id, start_index, end_index, capacity, skills, 3001 AS tw_open, 3000 AS tw_close, speed_factor FROM vehicles;

  PREPARE vehicles_equal_tw_problem AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles_equal_tw',
    'empty_breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY
  SELECT lives_ok('vehicles_equal_tw_problem', 'Problem with vehicles having equal time windows');

  PREPARE vehicles_invalid_tw_problem AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles_invalid_tw',
    'empty_breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY
  SELECT throws_ok(
    'vehicles_invalid_tw_problem',
    'XX000',
    'Invalid time window (3001, 3000)',
    'Problem with vehicles having invalid time windows'
  );


  -- Query with jobs having only one of delivery or pickup amount should succeed

  PREPARE jobs_only_delivery AS SELECT id, location_index, service, delivery, skills, priority FROM jobs;
  PREPARE jobs_only_pickup AS SELECT id, location_index, service, pickup, skills, priority FROM jobs;
  PREPARE jobs_only_delivery_problem AS
  SELECT * FROM vrp_vroomPlain(
    'jobs_only_delivery',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  PREPARE jobs_only_pickup_problem AS
  SELECT * FROM vrp_vroomPlain(
    'jobs_only_pickup',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY SELECT lives_ok('jobs_only_delivery_problem', 'Problem with jobs having only delivery amount');
  RETURN QUERY SELECT lives_ok('jobs_only_pickup_problem', 'Problem with jobs having only pickup amount');


  -- Query with vehicles having only one of start_index or end_index should succeed

  PREPARE vehicles_only_start_index AS SELECT id, start_index, capacity, skills, tw_open, tw_close, speed_factor FROM vehicles;
  PREPARE vehicles_only_end_index AS SELECT id, end_index, capacity, skills, tw_open, tw_close, speed_factor FROM vehicles;
  PREPARE vehicles_only_start_index_problem AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles_only_start_index',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  PREPARE vehicles_only_end_index_problem AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles_only_end_index',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY SELECT lives_ok('vehicles_only_start_index_problem', 'Problem with vehicles having only start_index');
  RETURN QUERY SELECT lives_ok('vehicles_only_end_index_problem', 'Problem with vehicles having only end_index');


  -- Query with vehicles having neither start_index nor end_index must fail

  PREPARE vehicles_no_index AS SELECT id, capacity, skills, tw_open, tw_close, speed_factor FROM vehicles;
  PREPARE vehicles_no_index_problem AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles_no_index',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY
  SELECT throws_ok(
    'vehicles_no_index_problem',
    'XX000',
    'At least one out of start_index or end_index must be present',
    'Problem with vehicles having neither start_index nor end_index'
  );


  -- Number of quantities of delivery, pickup, amount and capacity must be the same

  PREPARE jobs_inconsistent_delivery AS
  SELECT id, location_index, service, ARRAY[10, 20]::BIGINT[] AS delivery, pickup, skills, priority FROM jobs;
  PREPARE jobs_inconsistent_pickup AS
  SELECT id, location_index, service, delivery, ARRAY[10, 20]::BIGINT[] AS pickup, skills, priority FROM jobs;
  PREPARE shipments_inconsistent_amount AS
  SELECT id, p_location_index, p_service, d_location_index, d_service, ARRAY[10, 20]::BIGINT[] AS amount, skills, priority FROM shipments;
  PREPARE vehicles_inconsistent_capacity AS
  SELECT id, start_index, end_index, ARRAY[10, 20]::BIGINT[] AS capacity, skills, tw_open, tw_close, speed_factor FROM vehicles;

  PREPARE jobs_inconsistent_delivery_problem AS
  SELECT * FROM vrp_vroomPlain(
    'jobs_inconsistent_delivery',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  PREPARE jobs_inconsistent_pickup_problem AS
  SELECT * FROM vrp_vroomPlain(
    'jobs_inconsistent_pickup',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  PREPARE shipments_inconsistent_amount_problem AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments_inconsistent_amount',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  PREPARE vehicles_inconsistent_capacity_problem AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles_inconsistent_capacity',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );


  RETURN QUERY
  SELECT throws_ok(
    'jobs_inconsistent_delivery_problem',
    'XX000',
    'Inconsistent delivery length: 2 instead of 1.',
    'Problem with jobs having inconsistent delivery length'
  );

  RETURN QUERY
  SELECT throws_ok(
    'jobs_inconsistent_pickup_problem',
    'XX000',
    'Inconsistent pickup length: 2 instead of 1.',
    'Problem with jobs having inconsistent pickup length'
  );

  RETURN QUERY
  SELECT throws_ok(
    'shipments_inconsistent_amount_problem',
    'XX000',
    'Inconsistent delivery length: 2 instead of 1.',
    'Problem with shipments having inconsistent amount length'
  );

  -- Length is determined by vehicle capacity, so this error message
  -- denotes that job delivery has inconsistent length
  RETURN QUERY
  SELECT throws_ok(
    'vehicles_inconsistent_capacity_problem',
    'XX000',
    'Inconsistent delivery length: 1 instead of 2.',
    'Problem with vehicles having inconsistent capacity length'
  );


  -- If speed_factor and duration are multiplied by same number, result should be same

  PREPARE vehicles_2x_speed AS
  SELECT id, start_index, end_index, capacity, skills, tw_open, tw_close, 2 * speed_factor AS speed_factor FROM vehicles;
  PREPARE matrix_2x_distance AS
  SELECT start_id, end_id, 2 * duration AS duration FROM matrix;

  PREPARE vehicles_4x_speed AS
  SELECT id, start_index, end_index, capacity, skills, tw_open, tw_close, 4 * speed_factor AS speed_factor FROM vehicles;
  PREPARE matrix_4x_distance AS
  SELECT start_id, end_id, 4 * duration AS duration FROM matrix;

  PREPARE vehicles_2x_speed_problem AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles_2x_speed',
    'breaks',
    'breaks_time_windows',
    'matrix_2x_distance'
  );
  PREPARE vehicles_4x_speed_problem AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles_4x_speed',
    'breaks',
    'breaks_time_windows',
    'matrix_4x_distance'
  );
  RETURN QUERY
  SELECT set_eq('vehicles_2x_speed_problem', 'vroom_sql', 'Problem with 2x vehicle speed and distance');
  RETURN QUERY
  SELECT set_eq('vehicles_4x_speed_problem', 'vroom_sql', 'Problem with 4x vehicle speed and distance');


  -- one job/shipment tests

  -- Column Names:
  -- seq, vehicle_seq, vehicle_id, step_seq, step_type, task_id,
  -- arrival, travel_time, service_time, waiting_time, load

  PREPARE one_job_q1 AS
  SELECT * FROM vrp_vroomPlain(
    'SELECT * FROM jobs WHERE id = 1',
    'jobs_time_windows',
    'empty_shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY
  SELECT set_eq('one_job_q1',
    $$
      VALUES
      (1, 1, 1, 1, 1, -1, 300, 0, 0, 0, ARRAY[20]),
      (2, 1, 1, 2, 5, 1, 300, 0, 0, 0, ARRAY[20]),
      (3, 1, 1, 3, 2, 1, 300, 0, 250, 3325, ARRAY[20]),
      (4, 1, 1, 4, 6, -1, 3875, 0, 0, 0, ARRAY[20])
    $$,
    'Problem with only one job having id 1'
  );

  PREPARE one_job_q2 AS
  SELECT * FROM vrp_vroomPlain(
    'SELECT * FROM jobs WHERE id = 5',
    'jobs_time_windows',
    'empty_shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY
  SELECT set_eq('one_job_q2',
    $$
      VALUES
      (1, 1, 4, 1, 1, -1, 250, 0, 0, 0, ARRAY[20]),
      (2, 1, 4, 2, 5, 4, 250, 0, 0, 0, ARRAY[20]),
      (3, 1, 4, 3, 2, 5, 300, 50, 250, 725, ARRAY[20]),
      (4, 1, 4, 4, 6, -1, 1325, 100, 0, 0, ARRAY[20])
    $$,
    'Problem with only one job having id 5'
  );

  PREPARE one_shipment_q1 AS
  SELECT * FROM vrp_vroomPlain(
    'empty_jobs',
    'jobs_time_windows',
    'SELECT * FROM shipments WHERE id = 1',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY
  SELECT set_eq('one_shipment_q1',
    $$
      VALUES
      (1, 1, 4, 1, 1, -1, 250, 0, 0, 0, ARRAY[0]),
      (2, 1, 4, 2, 5, 4, 250, 0, 0, 0, ARRAY[0]),
      (3, 1, 4, 3, 3, 1, 250, 0, 2250, 1375, ARRAY[10]),
      (4, 1, 4, 4, 4, 1, 3900, 25, 2250, 21025, ARRAY[0]),
      (5, 1, 4, 5, 6, -1, 27200, 50, 0, 0, ARRAY[0])
    $$,
    'Problem with only one shipment having id 1'
  );

  PREPARE one_shipment_q2 AS
  SELECT * FROM vrp_vroomPlain(
    'empty_jobs',
    'jobs_time_windows',
    'SELECT * FROM shipments WHERE id = 5',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY
  SELECT set_eq('one_shipment_q2',
    $$
      VALUES
      (1, 1, 1, 1, 1, -1, 300, 0, 0, 0, ARRAY[0]),
      (2, 1, 1, 2, 5, 1, 300, 0, 0, 0, ARRAY[0]),
      (3, 1, 1, 3, 3, 5, 350, 50, 2250, 13000, ARRAY[10]),
      (4, 1, 1, 4, 4, 5, 15600, 50, 2250, 2575, ARRAY[0]),
      (5, 1, 1, 5, 6, -1, 20475, 100, 0, 0, ARRAY[0])
    $$,
    'Problem with only one shipment having id 5'
  );

  PREPARE one_job_shipment AS
  SELECT * FROM vrp_vroomPlain(
    'SELECT * FROM jobs WHERE id = 2',
    'jobs_time_windows',
    'SELECT * FROM shipments WHERE id = 2',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY
  SELECT set_eq('one_job_shipment',
    $$
      VALUES
      (1, 1, 1, 1, 1, -1, 300, 0, 0, 0, ARRAY[30]),
      (2, 1, 1, 2, 5, 1, 300, 0, 0, 0, ARRAY[30]),
      (3, 1, 1, 3, 2, 2, 350, 50, 250, 900, ARRAY[30]),
      (4, 1, 1, 4, 6, -1, 1550, 100, 0, 0, ARRAY[30]),
      (5, 2, 4, 1, 1, -1, 250, 0, 0, 0, ARRAY[0]),
      (6, 2, 4, 2, 5, 4, 250, 0, 0, 0, ARRAY[0]),
      (7, 2, 4, 3, 3, 2, 275, 25, 2250, 100, ARRAY[10]),
      (8, 2, 4, 4, 4, 2, 2736, 136, 2250, 1514, ARRAY[0]),
      (9, 2, 4, 5, 6, -1, 6590, 226, 0, 0, ARRAY[0])
    $$,
    'Problem with one job and one shipment having id 2'
  );

  PREPARE exploration_level_negative AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix',
    exploration_level => -1
  );
  RETURN QUERY
  SELECT throws_ok(
    'exploration_level_negative',
    'P0001',
    'Invalid value found on ''exploration_level''',
    'Problem with negative exploration_level'
  );

  PREPARE exploration_level_greater_5 AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix',
    exploration_level => 6
  );
  RETURN QUERY
  SELECT throws_ok(
    'exploration_level_greater_5',
    'P0001',
    'Invalid value found on ''exploration_level''',
    'Problem with exploration_level => 6'
  );

  PREPARE exploration_level_0 AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix',
    exploration_level => 0
  );
  RETURN QUERY
  SELECT set_eq('exploration_level_0',
    $$
      VALUES
      (1, 1, 1, 1, 1, -1, 300, 0, 0, 0, ARRAY[20]),
      (2, 1, 1, 2, 5, 1, 300, 0, 0, 0, ARRAY[20]),
      (3, 1, 1, 3, 2, 1, 300, 0, 250, 3325, ARRAY[20]),
      (4, 1, 1, 4, 3, 4, 3875, 0, 2250, 2500, ARRAY[40]),
      (5, 1, 1, 5, 4, 4, 8700, 75, 2250, 225, ARRAY[20]),
      (6, 1, 1, 6, 3, 5, 11265, 165, 2250, 2085, ARRAY[30]),
      (7, 1, 1, 7, 3, 3, 15650, 215, 2250, 0, ARRAY[50]),
      (8, 1, 1, 8, 4, 5, 17950, 265, 2250, 225, ARRAY[40]),
      (9, 1, 1, 9, 4, 3, 20425, 265, 2250, 200, ARRAY[20]),
      (10, 1, 1, 10, 6, -1, 22925, 315, 0, 0, ARRAY[20]),
      (11, 2, 2, 1, 1, -1, 275, 0, 0, 0, ARRAY[50]),
      (12, 2, 2, 2, 5, 2, 275, 0, 10, 0, ARRAY[50]),
      (13, 2, 2, 3, 2, 2, 335, 50, 250, 915, ARRAY[50]),
      (14, 2, 2, 4, 2, 5, 1590, 140, 250, 0, ARRAY[50]),
      (15, 2, 2, 5, 3, 1, 1890, 190, 2250, 0, ARRAY[60]),
      (16, 2, 2, 6, 4, 1, 4165, 215, 2250, 20760, ARRAY[50]),
      (17, 2, 2, 7, 6, -1, 27200, 240, 0, 0, ARRAY[50]),
      (18, 3, 4, 1, 1, -1, 250, 0, 0, 0, ARRAY[50]),
      (19, 3, 4, 2, 5, 4, 250, 0, 0, 0, ARRAY[50]),
      (20, 3, 4, 3, 3, 2, 275, 25, 2250, 100, ARRAY[60]),
      (21, 3, 4, 4, 2, 3, 2650, 50, 250, 75, ARRAY[60]),
      (22, 3, 4, 5, 2, 4, 2975, 50, 250, 550, ARRAY[60]),
      (23, 3, 4, 6, 4, 2, 3865, 140, 2250, 385, ARRAY[50]),
      (24, 3, 4, 7, 6, -1, 6590, 230, 0, 0, ARRAY[50])
    $$,
    'Query with the vroomdata and exploration_level => 0'
  );

  PREPARE exploration_level_5 AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix',
    exploration_level => 5
  );
  RETURN QUERY
  SELECT set_eq('exploration_level_5', 'vroom_sql', 'Problem with exploration_level => 5');


  PREPARE timeout_0 AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix',
    timeout => 0
  );
  RETURN QUERY
  SELECT set_eq('timeout_0',
    $$
      VALUES
      (1, 1, 1, 1, 1, -1, 300, 0, 0, 0, ARRAY[110]),
      (2, 1, 1, 2, 5, 1, 300, 0, 0, 0, ARRAY[110]),
      (3, 1, 1, 3, 2, 2, 350, 50, 250, 900, ARRAY[110]),
      (4, 1, 1, 4, 2, 5, 1590, 140, 250, 0, ARRAY[110]),
      (5, 1, 1, 5, 2, 4, 1890, 190, 250, 1635, ARRAY[110]),
      (6, 1, 1, 6, 2, 1, 3865, 280, 250, 0, ARRAY[110]),
      (7, 1, 1, 7, 6, -1, 4115, 280, 0, 0, ARRAY[110]),
      (8, 2, 4, 1, 1, -1, 250, 0, 0, 0, ARRAY[10]),
      (9, 2, 4, 2, 5, 4, 250, 0, 0, 0, ARRAY[10]),
      (10, 2, 4, 3, 3, 2, 275, 25, 2250, 100, ARRAY[20]),
      (11, 2, 4, 4, 2, 3, 2650, 50, 250, 75, ARRAY[20]),
      (12, 2, 4, 5, 3, 1, 2975, 50, 2250, 0, ARRAY[30]),
      (13, 2, 4, 6, 4, 2, 5315, 140, 2250, 0, ARRAY[20]),
      (14, 2, 4, 7, 3, 4, 7692, 267, 2250, 0, ARRAY[40]),
      (15, 2, 4, 8, 4, 4, 10017, 342, 2250, 0, ARRAY[20]),
      (16, 2, 4, 9, 3, 5, 12357, 432, 2250, 993, ARRAY[30]),
      (17, 2, 4, 10, 3, 3, 15650, 482, 2250, 0, ARRAY[50]),
      (18, 2, 4, 11, 4, 5, 17950, 532, 2250, 225, ARRAY[40]),
      (19, 2, 4, 12, 4, 3, 20425, 532, 2250, 200, ARRAY[20]),
      (20, 2, 4, 13, 4, 1, 23020, 677, 2250, 1905, ARRAY[10]),
      (21, 2, 4, 14, 6, -1, 27200, 702, 0, 0, ARRAY[10])
    $$,
    'Query with the vroomdata and timeout => 0'
  );

  PREPARE timeout_negative AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix',
    timeout => -1
  );
  RETURN QUERY
  SELECT set_eq('exploration_level_5', 'vroom_sql', 'Problem with timeout => -1');

END;
$BODY$
LANGUAGE plpgsql;


SELECT edge_cases();

SELECT * FROM finish();
ROLLBACK;
