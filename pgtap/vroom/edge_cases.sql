BEGIN;
SET search_path TO 'vroom', 'public';
SET client_min_messages TO ERROR;

SELECT CASE WHEN min_version('0.3.0') THEN plan (54) ELSE plan(1) END;

CREATE OR REPLACE FUNCTION edge_cases()
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  fn TEXT;
  start_sql TEXT;
  rest_sql TEXT;
BEGIN

  IF NOT min_version('0.3.0') THEN
    RETURN QUERY
    SELECT skip(1, 'Function is modified on 0.3.0');
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
      (1, 1, 1, '{}'::JSONB, 1, 1, -1, '{}'::JSONB, 300, 0, 0, 0, 300, ARRAY[30]),
      (2, 1, 1, '{}'::JSONB, 2, 5, 1, '{}'::JSONB, 300, 0, 0, 0, 300, ARRAY[30]),
      (3, 1, 1, '{}'::JSONB, 3, 2, 2, '{}'::JSONB, 350, 50, 250, 900, 1500, ARRAY[30]),
      (4, 1, 1, '{}'::JSONB, 4, 3, 5, '{}'::JSONB, 1500, 0, 2250, 11850, 15600, ARRAY[40]),
      (5, 1, 1, '{}'::JSONB, 5, 3, 3, '{}'::JSONB, 15650, 50, 2250, 0, 17900, ARRAY[60]),
      (6, 1, 1, '{}'::JSONB, 6, 4, 5, '{}'::JSONB, 17950, 50, 2250, 225, 20425, ARRAY[50]),
      (7, 1, 1, '{}'::JSONB, 7, 4, 3, '{}'::JSONB, 20425, 0, 2250, 200, 22875, ARRAY[30]),
      (8, 1, 1, '{}'::JSONB, 8, 6, -1, '{}'::JSONB, 22925, 50, 0, 0, 22925, ARRAY[30]),
      (9, 1, 1, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 200, 9250, 13175, 0, ARRAY[]::INTEGER[]),
      (10, 2, 2, '{}'::JSONB, 1, 1, -1, '{}'::JSONB, 275, 0, 0, 0, 275, ARRAY[70]),
      (11, 2, 2, '{}'::JSONB, 2, 5, 2, '{}'::JSONB, 275, 0, 10, 0, 285, ARRAY[70]),
      (12, 2, 2, '{}'::JSONB, 3, 2, 5, '{}'::JSONB, 360, 75, 250, 665, 1275, ARRAY[70]),
      (13, 2, 2, '{}'::JSONB, 4, 2, 3, '{}'::JSONB, 1325, 50, 250, 1400, 2975, ARRAY[70]),
      (14, 2, 2, '{}'::JSONB, 5, 2, 4, '{}'::JSONB, 2975, 0, 250, 550, 3775, ARRAY[70]),
      (15, 2, 2, '{}'::JSONB, 6, 6, -1, '{}'::JSONB, 3775, 0, 0, 0, 3775, ARRAY[70]),
      (16, 2, 2, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 125, 760, 2615, 0, ARRAY[]::INTEGER[]),
      (17, 3, 3, '{}'::JSONB, 1, 1, -1, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[20]),
      (18, 3, 3, '{}'::JSONB, 2, 5, 3, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[20]),
      (19, 3, 3, '{}'::JSONB, 3, 2, 1, '{}'::JSONB, 0, 0, 250, 3625, 3875, ARRAY[20]),
      (20, 3, 3, '{}'::JSONB, 4, 3, 4, '{}'::JSONB, 3875, 0, 2250, 2500, 8625, ARRAY[40]),
      (21, 3, 3, '{}'::JSONB, 5, 4, 4, '{}'::JSONB, 8700, 75, 2250, 225, 11175, ARRAY[20]),
      (22, 3, 3, '{}'::JSONB, 6, 6, -1, '{}'::JSONB, 11250, 75, 0, 0, 11250, ARRAY[20]),
      (23, 3, 3, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 150, 4750, 6350, 0, ARRAY[]::INTEGER[]),
      (24, 4, 4, '{}'::JSONB, 1, 1, -1, '{}'::JSONB, 250, 0, 0, 0, 250, ARRAY[0]),
      (25, 4, 4, '{}'::JSONB, 2, 5, 4, '{}'::JSONB, 250, 0, 0, 0, 250, ARRAY[0]),
      (26, 4, 4, '{}'::JSONB, 3, 3, 2, '{}'::JSONB, 275, 25, 2250, 100, 2625, ARRAY[10]),
      (27, 4, 4, '{}'::JSONB, 4, 3, 1, '{}'::JSONB, 2650, 25, 2250, 0, 4900, ARRAY[20]),
      (28, 4, 4, '{}'::JSONB, 5, 4, 2, '{}'::JSONB, 4990, 90, 2250, 0, 7240, ARRAY[10]),
      (29, 4, 4, '{}'::JSONB, 6, 4, 1, '{}'::JSONB, 7351, 111, 2250, 17574, 27175, ARRAY[0]),
      (30, 4, 4, '{}'::JSONB, 7, 6, -1, '{}'::JSONB, 27200, 25, 0, 0, 27200, ARRAY[0]),
      (31, 4, 4, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 276, 9000, 17674, 0, ARRAY[]::INTEGER[]),
      (32, 0, 0, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 751, 23760, 39814, 0, ARRAY[]::INTEGER[])
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
    'SELECT id, location_id, service, delivery, pickup, skills, -1 AS priority FROM jobs',
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
    'SELECT id, location_id, service, delivery, pickup, skills, 101 AS priority FROM jobs',
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
    'SELECT id, location_id, service, delivery, pickup, skills, 0 AS priority FROM jobs',
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
    'SELECT id, location_id, service, delivery, pickup, skills, 0 AS priority FROM jobs',
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
    'SELECT id, p_location_id, p_service, d_location_id, d_service,
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
    'SELECT id, p_location_id, p_service, d_location_id, d_service,
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
    'SELECT id, p_location_id, p_service, d_location_id, d_service,
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
    'SELECT id, p_location_id, p_service, d_location_id, d_service,
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
  SELECT id, start_id, end_id, capacity, skills, 3000 AS tw_open, 3000 AS tw_close, speed_factor FROM vehicles;
  PREPARE vehicles_invalid_tw AS
  SELECT id, start_id, end_id, capacity, skills, 3001 AS tw_open, 3000 AS tw_close, speed_factor FROM vehicles;

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

  PREPARE jobs_only_delivery AS SELECT id, location_id, service, delivery, skills, priority FROM jobs;
  PREPARE jobs_only_pickup AS SELECT id, location_id, service, pickup, skills, priority FROM jobs;
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


  -- Query with vehicles having only one of start_id or end_id should succeed

  PREPARE vehicles_only_start_id AS SELECT id, start_id, capacity, skills, tw_open, tw_close, speed_factor FROM vehicles;
  PREPARE vehicles_only_end_id AS SELECT id, end_id, capacity, skills, tw_open, tw_close, speed_factor FROM vehicles;
  PREPARE vehicles_only_start_id_problem AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles_only_start_id',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  PREPARE vehicles_only_end_id_problem AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles_only_end_id',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY SELECT lives_ok('vehicles_only_start_id_problem', 'Problem with vehicles having only start_id');
  RETURN QUERY SELECT lives_ok('vehicles_only_end_id_problem', 'Problem with vehicles having only end_id');


  -- Query with vehicles having neither start_id nor end_id must fail

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
    'At least one out of start_id or end_id must be present',
    'Problem with vehicles having neither start_id nor end_id'
  );


  -- Number of quantities of delivery, pickup, amount and capacity must be the same

  PREPARE jobs_inconsistent_delivery AS
  SELECT id, location_id, service, ARRAY[10, 20]::BIGINT[] AS delivery, pickup, skills, priority FROM jobs;
  PREPARE jobs_inconsistent_pickup AS
  SELECT id, location_id, service, delivery, ARRAY[10, 20]::BIGINT[] AS pickup, skills, priority FROM jobs;
  PREPARE shipments_inconsistent_amount AS
  SELECT id, p_location_id, p_service, d_location_id, d_service, ARRAY[10, 20]::BIGINT[] AS amount, skills, priority FROM shipments;
  PREPARE vehicles_inconsistent_capacity AS
  SELECT id, start_id, end_id, ARRAY[10, 20]::BIGINT[] AS capacity, skills, tw_open, tw_close, speed_factor FROM vehicles;

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
  SELECT id, start_id, end_id, capacity, skills, tw_open, tw_close, 2 * speed_factor AS speed_factor FROM vehicles;
  PREPARE matrix_2x_distance AS
  SELECT start_id, end_id, 2 * duration AS duration FROM matrix;

  PREPARE vehicles_4x_speed AS
  SELECT id, start_id, end_id, capacity, skills, tw_open, tw_close, 4 * speed_factor AS speed_factor FROM vehicles;
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
      (1, 1, 1, '{}'::JSONB, 1, 1, -1, '{}'::JSONB, 300, 0, 0, 0, 300, ARRAY[20]),
      (2, 1, 1, '{}'::JSONB, 2, 5, 1, '{}'::JSONB, 300, 0, 0, 0, 300, ARRAY[20]),
      (3, 1, 1, '{}'::JSONB, 3, 2, 1, '{}'::JSONB, 300, 0, 250, 3325, 3875, ARRAY[20]),
      (4, 1, 1, '{}'::JSONB, 4, 6, -1, '{}'::JSONB, 3875, 0, 0, 0, 3875, ARRAY[20]),
      (5, 1, 1, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 0, 250, 3325, 0, ARRAY[]::INTEGER[]),
      (6, 0, 0, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 0, 250, 3325, 0, ARRAY[]::INTEGER[])
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
      (1, 1, 4, '{}'::JSONB, 1, 1, -1, '{}'::JSONB, 250, 0, 0, 0, 250, ARRAY[20]),
      (2, 1, 4, '{}'::JSONB, 2, 5, 4, '{}'::JSONB, 250, 0, 0, 0, 250, ARRAY[20]),
      (3, 1, 4, '{}'::JSONB, 3, 2, 5, '{}'::JSONB, 300, 50, 250, 725, 1275, ARRAY[20]),
      (4, 1, 4, '{}'::JSONB, 4, 6, -1, '{}'::JSONB, 1325, 50, 0, 0, 1325, ARRAY[20]),
      (5, 1, 4, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 100, 250, 725, 0, ARRAY[]::INTEGER[]),
      (6, 0, 0, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 100, 250, 725, 0, ARRAY[]::INTEGER[])
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
      (1, 1, 4, '{}'::JSONB, 1, 1, -1, '{}'::JSONB, 250, 0, 0, 0, 250, ARRAY[0]),
      (2, 1, 4, '{}'::JSONB, 2, 5, 4, '{}'::JSONB, 250, 0, 0, 0, 250, ARRAY[0]),
      (3, 1, 4, '{}'::JSONB, 3, 3, 1, '{}'::JSONB, 250, 0, 2250, 1375, 3875, ARRAY[10]),
      (4, 1, 4, '{}'::JSONB, 4, 4, 1, '{}'::JSONB, 3900, 25, 2250, 21025, 27175, ARRAY[0]),
      (5, 1, 4, '{}'::JSONB, 5, 6, -1, '{}'::JSONB, 27200, 25, 0, 0, 27200, ARRAY[0]),
      (6, 1, 4, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 50, 4500, 22400, 0, ARRAY[]::INTEGER[]),
      (7, 0, 0, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 50, 4500, 22400, 0, ARRAY[]::INTEGER[])
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
      (1, 1, 1, '{}'::JSONB, 1, 1, -1, '{}'::JSONB, 300, 0, 0, 0, 300, ARRAY[0]),
      (2, 1, 1, '{}'::JSONB, 2, 5, 1, '{}'::JSONB, 300, 0, 0, 0, 300, ARRAY[0]),
      (3, 1, 1, '{}'::JSONB, 3, 3, 5, '{}'::JSONB, 350, 50, 2250, 13000, 15600, ARRAY[10]),
      (4, 1, 1, '{}'::JSONB, 4, 4, 5, '{}'::JSONB, 15600, 0, 2250, 2575, 20425, ARRAY[0]),
      (5, 1, 1, '{}'::JSONB, 5, 6, -1, '{}'::JSONB, 20475, 50, 0, 0, 20475, ARRAY[0]),
      (6, 1, 1, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 100, 4500, 15575, 0, ARRAY[]::INTEGER[]),
      (7, 0, 0, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 100, 4500, 15575, 0, ARRAY[]::INTEGER[])
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
      (1, 1, 1, '{}'::JSONB, 1, 1, -1, '{}'::JSONB, 300, 0, 0, 0, 300, ARRAY[30]),
      (2, 1, 1, '{}'::JSONB, 2, 5, 1, '{}'::JSONB, 300, 0, 0, 0, 300, ARRAY[30]),
      (3, 1, 1, '{}'::JSONB, 3, 2, 2, '{}'::JSONB, 350, 50, 250, 900, 1500, ARRAY[30]),
      (4, 1, 1, '{}'::JSONB, 4, 6, -1, '{}'::JSONB, 1550, 50, 0, 0, 1550, ARRAY[30]),
      (5, 1, 1, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 100, 250, 900, 0, ARRAY[]::INTEGER[]),
      (6, 2, 4, '{}'::JSONB, 1, 1, -1, '{}'::JSONB, 250, 0, 0, 0, 250, ARRAY[0]),
      (7, 2, 4, '{}'::JSONB, 2, 5, 4, '{}'::JSONB, 250, 0, 0, 0, 250, ARRAY[0]),
      (8, 2, 4, '{}'::JSONB, 3, 3, 2, '{}'::JSONB, 275, 25, 2250, 100, 2625, ARRAY[10]),
      (9, 2, 4, '{}'::JSONB, 4, 4, 2, '{}'::JSONB, 2736, 111, 2250, 1514, 6500, ARRAY[0]),
      (10, 2, 4, '{}'::JSONB, 5, 6, -1, '{}'::JSONB, 6590, 90, 0, 0, 6590, ARRAY[0]),
      (11, 2, 4, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 226, 4500, 1614, 0, ARRAY[]::INTEGER[]),
      (12, 0, 0, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 326, 4750, 2514, 0, ARRAY[]::INTEGER[])
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
    (1, 1, 1, '{}'::JSONB, 1, 1, -1, '{}'::JSONB, 300, 0, 0, 0, 300, ARRAY[20]),
    (2, 1, 1, '{}'::JSONB, 2, 5, 1, '{}'::JSONB, 300, 0, 0, 0, 300, ARRAY[20]),
    (3, 1, 1, '{}'::JSONB, 3, 2, 1, '{}'::JSONB, 300, 0, 250, 3325, 3875, ARRAY[20]),
    (4, 1, 1, '{}'::JSONB, 4, 3, 4, '{}'::JSONB, 3875, 0, 2250, 2500, 8625, ARRAY[40]),
    (5, 1, 1, '{}'::JSONB, 5, 4, 4, '{}'::JSONB, 8700, 75, 2250, 225, 11175, ARRAY[20]),
    (6, 1, 1, '{}'::JSONB, 6, 3, 5, '{}'::JSONB, 11265, 90, 2250, 2085, 15600, ARRAY[30]),
    (7, 1, 1, '{}'::JSONB, 7, 3, 3, '{}'::JSONB, 15650, 50, 2250, 0, 17900, ARRAY[50]),
    (8, 1, 1, '{}'::JSONB, 8, 4, 5, '{}'::JSONB, 17950, 50, 2250, 225, 20425, ARRAY[40]),
    (9, 1, 1, '{}'::JSONB, 9, 4, 3, '{}'::JSONB, 20425, 0, 2250, 200, 22875, ARRAY[20]),
    (10, 1, 1, '{}'::JSONB, 10, 6, -1, '{}'::JSONB, 22925, 50, 0, 0, 22925, ARRAY[20]),
    (11, 1, 1, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 315, 13750, 8560, 0, ARRAY[]::INTEGER[]),
    (12, 2, 2, '{}'::JSONB, 1, 1, -1, '{}'::JSONB, 275, 0, 0, 0, 275, ARRAY[50]),
    (13, 2, 2, '{}'::JSONB, 2, 5, 2, '{}'::JSONB, 275, 0, 10, 0, 285, ARRAY[50]),
    (14, 2, 2, '{}'::JSONB, 3, 2, 2, '{}'::JSONB, 335, 50, 250, 915, 1500, ARRAY[50]),
    (15, 2, 2, '{}'::JSONB, 4, 2, 5, '{}'::JSONB, 1590, 90, 250, 0, 1840, ARRAY[50]),
    (16, 2, 2, '{}'::JSONB, 5, 3, 1, '{}'::JSONB, 1890, 50, 2250, 0, 4140, ARRAY[60]),
    (17, 2, 2, '{}'::JSONB, 6, 4, 1, '{}'::JSONB, 4165, 25, 2250, 20760, 27175, ARRAY[50]),
    (18, 2, 2, '{}'::JSONB, 7, 6, -1, '{}'::JSONB, 27200, 25, 0, 0, 27200, ARRAY[50]),
    (19, 2, 2, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 240, 5010, 21675, 0, ARRAY[]::INTEGER[]),
    (20, 3, 4, '{}'::JSONB, 1, 1, -1, '{}'::JSONB, 250, 0, 0, 0, 250, ARRAY[50]),
    (21, 3, 4, '{}'::JSONB, 2, 5, 4, '{}'::JSONB, 250, 0, 0, 0, 250, ARRAY[50]),
    (22, 3, 4, '{}'::JSONB, 3, 3, 2, '{}'::JSONB, 275, 25, 2250, 100, 2625, ARRAY[60]),
    (23, 3, 4, '{}'::JSONB, 4, 2, 3, '{}'::JSONB, 2650, 25, 250, 75, 2975, ARRAY[60]),
    (24, 3, 4, '{}'::JSONB, 5, 2, 4, '{}'::JSONB, 2975, 0, 250, 550, 3775, ARRAY[60]),
    (25, 3, 4, '{}'::JSONB, 6, 4, 2, '{}'::JSONB, 3865, 90, 2250, 385, 6500, ARRAY[50]),
    (26, 3, 4, '{}'::JSONB, 7, 6, -1, '{}'::JSONB, 6590, 90, 0, 0, 6590, ARRAY[50]),
    (27, 3, 4, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 230, 5000, 1110, 0, ARRAY[]::INTEGER[]),
    (28, 0, 0, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 785, 23760, 31345, 0, ARRAY[]::INTEGER[])
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
      (1, 1, 1, '{}'::JSONB, 1, 1, -1, '{}'::JSONB, 300, 0, 0, 0, 300, ARRAY[110]),
      (2, 1, 1, '{}'::JSONB, 2, 5, 1, '{}'::JSONB, 300, 0, 0, 0, 300, ARRAY[110]),
      (3, 1, 1, '{}'::JSONB, 3, 2, 2, '{}'::JSONB, 350, 50, 250, 900, 1500, ARRAY[110]),
      (4, 1, 1, '{}'::JSONB, 4, 2, 5, '{}'::JSONB, 1590, 90, 250, 0, 1840, ARRAY[110]),
      (5, 1, 1, '{}'::JSONB, 5, 2, 4, '{}'::JSONB, 1890, 50, 250, 1635, 3775, ARRAY[110]),
      (6, 1, 1, '{}'::JSONB, 6, 2, 1, '{}'::JSONB, 3865, 90, 250, 0, 4115, ARRAY[110]),
      (7, 1, 1, '{}'::JSONB, 7, 6, -1, '{}'::JSONB, 4115, 0, 0, 0, 4115, ARRAY[110]),
      (8, 1, 1, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 280, 1000, 2535, 0, ARRAY[]::INTEGER[]),
      (9, 2, 4, '{}'::JSONB, 1, 1, -1, '{}'::JSONB, 250, 0, 0, 0, 250, ARRAY[10]),
      (10, 2, 4, '{}'::JSONB, 2, 5, 4, '{}'::JSONB, 250, 0, 0, 0, 250, ARRAY[10]),
      (11, 2, 4, '{}'::JSONB, 3, 3, 2, '{}'::JSONB, 275, 25, 2250, 100, 2625, ARRAY[20]),
      (12, 2, 4, '{}'::JSONB, 4, 2, 3, '{}'::JSONB, 2650, 25, 250, 75, 2975, ARRAY[20]),
      (13, 2, 4, '{}'::JSONB, 5, 3, 1, '{}'::JSONB, 2975, 0, 2250, 0, 5225, ARRAY[30]),
      (14, 2, 4, '{}'::JSONB, 6, 4, 2, '{}'::JSONB, 5315, 90, 2250, 0, 7565, ARRAY[20]),
      (15, 2, 4, '{}'::JSONB, 7, 3, 4, '{}'::JSONB, 7692, 127, 2250, 0, 9942, ARRAY[40]),
      (16, 2, 4, '{}'::JSONB, 8, 4, 4, '{}'::JSONB, 10017, 75, 2250, 0, 12267, ARRAY[20]),
      (17, 2, 4, '{}'::JSONB, 9, 3, 5, '{}'::JSONB, 12357, 90, 2250, 993, 15600, ARRAY[30]),
      (18, 2, 4, '{}'::JSONB, 10, 3, 3, '{}'::JSONB, 15650, 50, 2250, 0, 17900, ARRAY[50]),
      (19, 2, 4, '{}'::JSONB, 11, 4, 5, '{}'::JSONB, 17950, 50, 2250, 225, 20425, ARRAY[40]),
      (20, 2, 4, '{}'::JSONB, 12, 4, 3, '{}'::JSONB, 20425, 0, 2250, 200, 22875, ARRAY[20]),
      (21, 2, 4, '{}'::JSONB, 13, 4, 1, '{}'::JSONB, 23020, 145, 2250, 1905, 27175, ARRAY[10]),
      (22, 2, 4, '{}'::JSONB, 14, 6, -1, '{}'::JSONB, 27200, 25, 0, 0, 27200, ARRAY[10]),
      (23, 2, 4, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 702, 22750, 3498, 0, ARRAY[]::INTEGER[]),
      (24, 0, 0, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 982, 23750, 6033, 0, ARRAY[]::INTEGER[])
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

  PREPARE vehicles_negative_task AS
  SELECT id, start_id, end_id, capacity, skills, tw_open, tw_close, speed_factor, -1 AS max_tasks FROM vehicles;

  PREPARE vehicles_0_task AS
  SELECT id, start_id, end_id, capacity, skills, tw_open, tw_close, speed_factor, 0 AS max_tasks FROM vehicles;

  PREPARE vehicles_1_task AS
  SELECT id, start_id, end_id, capacity, skills, tw_open, tw_close, speed_factor, 1 AS max_tasks FROM vehicles;

  PREPARE problem_negative_tasks AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles_negative_task',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY
  SELECT throws_ok(
    'problem_negative_tasks',
    'XX000',
    'Invalid max_tasks value -1',
    'Problem with max_tasks as -1'
  );

  PREPARE problem_0_tasks AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles_0_task',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY
  SELECT set_eq('problem_0_tasks',
    $$
      VALUES
      (1, 1, -1, '{}'::JSONB, 1, 4, 5, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[]::INTEGER[]),
      (2, 1, -1, '{}'::JSONB, 2, 3, 5, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[]::INTEGER[]),
      (3, 1, -1, '{}'::JSONB, 3, 2, 1, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[]::INTEGER[]),
      (4, 1, -1, '{}'::JSONB, 4, 2, 2, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[]::INTEGER[]),
      (5, 1, -1, '{}'::JSONB, 5, 2, 3, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[]::INTEGER[]),
      (6, 1, -1, '{}'::JSONB, 6, 2, 4, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[]::INTEGER[]),
      (7, 1, -1, '{}'::JSONB, 7, 2, 5, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[]::INTEGER[]),
      (8, 1, -1, '{}'::JSONB, 8, 3, 1, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[]::INTEGER[]),
      (9, 1, -1, '{}'::JSONB, 9, 4, 1, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[]::INTEGER[]),
      (10, 1, -1, '{}'::JSONB, 10, 3, 2, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[]::INTEGER[]),
      (11, 1, -1, '{}'::JSONB, 11, 4, 2, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[]::INTEGER[]),
      (12, 1, -1, '{}'::JSONB, 12, 3, 3, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[]::INTEGER[]),
      (13, 1, -1, '{}'::JSONB, 13, 4, 3, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[]::INTEGER[]),
      (14, 1, -1, '{}'::JSONB, 14, 3, 4, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[]::INTEGER[]),
      (15, 1, -1, '{}'::JSONB, 15, 4, 4, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[]::INTEGER[]),
      (16, 0, 0, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[]::INTEGER[])
    $$,
    'Problem with max_tasks as 0'
  );

  PREPARE problem_1_task AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'vehicles_1_task',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY
  SELECT set_eq('problem_1_task',
    $$
      VALUES
      (1, 1, 1, '{}'::JSONB, 1, 1, -1, '{}'::JSONB, 300, 0, 0, 0, 300, ARRAY[30]),
      (2, 1, 1, '{}'::JSONB, 2, 5, 1, '{}'::JSONB, 300, 0, 0, 0, 300, ARRAY[30]),
      (3, 1, 1, '{}'::JSONB, 3, 2, 2, '{}'::JSONB, 350, 50, 250, 900, 1500, ARRAY[30]),
      (4, 1, 1, '{}'::JSONB, 4, 6, -1, '{}'::JSONB, 1550, 50, 0, 0, 1550, ARRAY[30]),
      (5, 1, 1, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 100, 250, 900, 0, ARRAY[]::INTEGER[]),
      (6, 2, 2, '{}'::JSONB, 1, 1, -1, '{}'::JSONB, 275, 0, 0, 0, 275, ARRAY[10]),
      (7, 2, 2, '{}'::JSONB, 2, 5, 2, '{}'::JSONB, 275, 0, 10, 0, 285, ARRAY[10]),
      (8, 2, 2, '{}'::JSONB, 3, 2, 3, '{}'::JSONB, 375, 90, 250, 2350, 2975, ARRAY[10]),
      (9, 2, 2, '{}'::JSONB, 4, 6, -1, '{}'::JSONB, 2975, 0, 0, 0, 2975, ARRAY[10]),
      (10, 2, 2, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 90, 260, 2350, 0, ARRAY[]::INTEGER[]),
      (11, 3, 3, '{}'::JSONB, 1, 1, -1, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[20]),
      (12, 3, 3, '{}'::JSONB, 2, 5, 3, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[20]),
      (13, 3, 3, '{}'::JSONB, 3, 2, 1, '{}'::JSONB, 0, 0, 250, 3625, 3875, ARRAY[20]),
      (14, 3, 3, '{}'::JSONB, 4, 6, -1, '{}'::JSONB, 3875, 0, 0, 0, 3875, ARRAY[20]),
      (15, 3, 3, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 0, 250, 3625, 0, ARRAY[]::INTEGER[]),
      (16, 4, 4, '{}'::JSONB, 1, 1, -1, '{}'::JSONB, 250, 0, 0, 0, 250, ARRAY[40]),
      (17, 4, 4, '{}'::JSONB, 2, 5, 4, '{}'::JSONB, 250, 0, 0, 0, 250, ARRAY[40]),
      (18, 4, 4, '{}'::JSONB, 3, 2, 4, '{}'::JSONB, 250, 0, 250, 3275, 3775, ARRAY[40]),
      (19, 4, 4, '{}'::JSONB, 4, 6, -1, '{}'::JSONB, 3775, 0, 0, 0, 3775, ARRAY[40]),
      (20, 4, 4, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 0, 250, 3275, 0, ARRAY[]::INTEGER[]),
      (21, 5, -1, '{}'::JSONB, 1, 4, 5, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[]::INTEGER[]),
      (22, 5, -1, '{}'::JSONB, 2, 3, 5, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[]::INTEGER[]),
      (23, 5, -1, '{}'::JSONB, 3, 2, 5, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[]::INTEGER[]),
      (24, 5, -1, '{}'::JSONB, 4, 3, 1, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[]::INTEGER[]),
      (25, 5, -1, '{}'::JSONB, 5, 4, 1, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[]::INTEGER[]),
      (26, 5, -1, '{}'::JSONB, 6, 3, 2, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[]::INTEGER[]),
      (27, 5, -1, '{}'::JSONB, 7, 4, 2, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[]::INTEGER[]),
      (28, 5, -1, '{}'::JSONB, 8, 3, 3, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[]::INTEGER[]),
      (29, 5, -1, '{}'::JSONB, 9, 4, 3, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[]::INTEGER[]),
      (30, 5, -1, '{}'::JSONB, 10, 3, 4, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[]::INTEGER[]),
      (31, 5, -1, '{}'::JSONB, 11, 4, 4, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[]::INTEGER[]),
      (32, 0, 0, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 190, 1010, 10150, 0, ARRAY[]::INTEGER[])
    $$,
    'Query with max_tasks as 1'
  );

  PREPARE empty_skills_1 AS
  SELECT * FROM vrp_vroomPlain(
    'SELECT id, location_id, service, delivery, pickup, ARRAY[]::INTEGER[]::INTEGER[]::INTEGER[]::INTEGER[] AS skills, priority FROM jobs',
    'jobs_time_windows',
    'SELECT id, p_location_id, p_service, d_location_id, d_service, amount, ARRAY[]::INTEGER[]::INTEGER[]::INTEGER[]::INTEGER[] AS skills, priority FROM shipments',
    'shipments_time_windows',
    'SELECT id, start_id, end_id, capacity, ARRAY[]::INTEGER[]::INTEGER[]::INTEGER[]::INTEGER[] AS skills, tw_open, tw_close, speed_factor, max_tasks FROM vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );

  RETURN QUERY
  SELECT set_eq('empty_skills_1', 'vroom_sql', 'Test for empty skills array - 1');

  PREPARE empty_skills_2 AS
  SELECT * FROM vrp_vroomPlain(
    'SELECT id, location_id, service, delivery, pickup, ARRAY[]::INTEGER[]::INTEGER[]::INTEGER[]::INTEGER[] AS skills, priority FROM jobs',
    'jobs_time_windows',
    'SELECT id, p_location_id, p_service, d_location_id, d_service, amount, ARRAY[]::INTEGER[]::INTEGER[]::INTEGER[]::INTEGER[] AS skills, priority FROM shipments',
    'shipments_time_windows',
    'vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );

  RETURN QUERY
  SELECT set_eq('empty_skills_2', 'vroom_sql', 'Test for empty skills array - 2');

  PREPARE problem_negative_speed_factor AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'SELECT id, start_id, end_id, capacity, skills, tw_open, tw_close, -1 AS speed_factor, max_tasks FROM vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY
  SELECT throws_ok(
    'problem_negative_speed_factor',
    'XX000',
    'Invalid speed_factor -1.000000',
    'Problem with speed_factor as -1'
  );

  PREPARE problem_0_speed_factor AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'SELECT id, start_id, end_id, capacity, skills, tw_open, tw_close, 0 AS speed_factor, max_tasks FROM vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY
  SELECT throws_ok(
    'problem_negative_speed_factor',
    'XX000',
    'Invalid speed_factor -1.000000',
    'Problem with speed_factor as 0'
  );

  UPDATE vehicles SET speed_factor = 5.01 WHERE id = 4;
  PREPARE problem_more_than_5times_speed_factor AS
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
  SELECT throws_ok(
    'problem_more_than_5times_speed_factor',
    'XX000',
    'The speed_factor 5.01 is more than five times the speed factor 1',
    'Problem with one speed factor more than five times the other'
  );
  UPDATE vehicles SET speed_factor = 1.0 WHERE id = 4;

  PREPARE problem_speed_factor_5times_scaled AS
  SELECT * FROM vrp_vroomPlain(
    'jobs',
    'jobs_time_windows',
    'shipments',
    'shipments_time_windows',
    'SELECT id, start_id, end_id, capacity, skills, tw_open, tw_close, speed_factor * 5 AS speed_factor, max_tasks FROM vehicles',
    'breaks',
    'breaks_time_windows',
    'matrix'
  );
  RETURN QUERY
  SELECT set_eq('problem_speed_factor_5times_scaled',
    $$
      VALUES
      (1, 1, 1, '{}'::JSONB, 1, 1, -1, '{}'::JSONB, 300, 0, 0, 0, 300, ARRAY[30]),
      (2, 1, 1, '{}'::JSONB, 2, 5, 1, '{}'::JSONB, 300, 0, 0, 0, 300, ARRAY[30]),
      (3, 1, 1, '{}'::JSONB, 3, 2, 2, '{}'::JSONB, 310, 10, 250, 940, 1500, ARRAY[30]),
      (4, 1, 1, '{}'::JSONB, 4, 3, 5, '{}'::JSONB, 1500, 0, 2250, 11850, 15600, ARRAY[40]),
      (5, 1, 1, '{}'::JSONB, 5, 3, 3, '{}'::JSONB, 15610, 10, 2250, 0, 17860, ARRAY[60]),
      (6, 1, 1, '{}'::JSONB, 6, 4, 5, '{}'::JSONB, 17870, 10, 2250, 305, 20425, ARRAY[50]),
      (7, 1, 1, '{}'::JSONB, 7, 4, 3, '{}'::JSONB, 20425, 0, 2250, 200, 22875, ARRAY[30]),
      (8, 1, 1, '{}'::JSONB, 8, 6, -1, '{}'::JSONB, 22885, 10, 0, 0, 22885, ARRAY[30]),
      (9, 1, 1, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 40, 9250, 13295, 0, ARRAY[]::INTEGER[]),
      (10, 2, 2, '{}'::JSONB, 1, 1, -1, '{}'::JSONB, 275, 0, 0, 0, 275, ARRAY[70]),
      (11, 2, 2, '{}'::JSONB, 2, 5, 2, '{}'::JSONB, 275, 0, 10, 0, 285, ARRAY[70]),
      (12, 2, 2, '{}'::JSONB, 3, 2, 5, '{}'::JSONB, 300, 15, 250, 725, 1275, ARRAY[70]),
      (13, 2, 2, '{}'::JSONB, 4, 2, 3, '{}'::JSONB, 1285, 10, 250, 1440, 2975, ARRAY[70]),
      (14, 2, 2, '{}'::JSONB, 5, 2, 4, '{}'::JSONB, 2975, 0, 250, 550, 3775, ARRAY[70]),
      (15, 2, 2, '{}'::JSONB, 6, 6, -1, '{}'::JSONB, 3775, 0, 0, 0, 3775, ARRAY[70]),
      (16, 2, 2, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 25, 760, 2715, 0, ARRAY[]::INTEGER[]),
      (17, 3, 3, '{}'::JSONB, 1, 1, -1, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[20]),
      (18, 3, 3, '{}'::JSONB, 2, 5, 3, '{}'::JSONB, 0, 0, 0, 0, 0, ARRAY[20]),
      (19, 3, 3, '{}'::JSONB, 3, 2, 1, '{}'::JSONB, 0, 0, 250, 3625, 3875, ARRAY[20]),
      (20, 3, 3, '{}'::JSONB, 4, 3, 4, '{}'::JSONB, 3875, 0, 2250, 2500, 8625, ARRAY[40]),
      (21, 3, 3, '{}'::JSONB, 5, 4, 4, '{}'::JSONB, 8640, 15, 2250, 285, 11175, ARRAY[20]),
      (22, 3, 3, '{}'::JSONB, 6, 6, -1, '{}'::JSONB, 11190, 15, 0, 0, 11190, ARRAY[20]),
      (23, 3, 3, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 30, 4750, 6410, 0, ARRAY[]::INTEGER[]),
      (24, 4, 4, '{}'::JSONB, 1, 1, -1, '{}'::JSONB, 250, 0, 0, 0, 250, ARRAY[0]),
      (25, 4, 4, '{}'::JSONB, 2, 5, 4, '{}'::JSONB, 250, 0, 0, 0, 250, ARRAY[0]),
      (26, 4, 4, '{}'::JSONB, 3, 3, 2, '{}'::JSONB, 255, 5, 2250, 120, 2625, ARRAY[10]),
      (27, 4, 4, '{}'::JSONB, 4, 3, 1, '{}'::JSONB, 2630, 5, 2250, 0, 4880, ARRAY[20]),
      (28, 4, 4, '{}'::JSONB, 5, 4, 2, '{}'::JSONB, 4898, 18, 2250, 0, 7148, ARRAY[10]),
      (29, 4, 4, '{}'::JSONB, 6, 4, 1, '{}'::JSONB, 7170, 22, 2250, 17755, 27175, ARRAY[0]),
      (30, 4, 4, '{}'::JSONB, 7, 6, -1, '{}'::JSONB, 27180, 5, 0, 0, 27180, ARRAY[0]),
      (31, 4, 4, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 55, 9000, 17875, 0, ARRAY[]::INTEGER[]),
      (32, 0, 0, '{}'::JSONB, 0, 0, 0, '{}'::JSONB, 0, 150, 23760, 40295, 0, ARRAY[]::INTEGER[])
    $$,
    'Query with speed factor scaled five times'
  );

END;
$BODY$
LANGUAGE plpgsql;


SELECT edge_cases();

SELECT * FROM finish();
ROLLBACK;
