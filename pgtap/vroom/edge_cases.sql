BEGIN;
SET search_path TO 'vroom', 'public';
SET client_min_messages TO ERROR;

SELECT CASE WHEN min_version('0.2.0') THEN plan (38) ELSE plan(1) END;

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

END;
$BODY$
LANGUAGE plpgsql;


SELECT edge_cases();

SELECT * FROM finish();
ROLLBACK;
