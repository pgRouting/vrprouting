BEGIN;
SET search_path TO 'vroom', 'public';
SET client_min_messages TO ERROR;

SELECT CASE WHEN min_version('0.2.0') THEN plan (116) ELSE plan(1) END;

CREATE OR REPLACE FUNCTION no_crash(is_plain BOOLEAN)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  params TEXT[];
  subs TEXT[];
  error_messages TEXT[];
  non_empty_args INTEGER[];
BEGIN
  IF NOT min_version('0.2.0') THEN
    RETURN QUERY
    SELECT skip(1, 'Function is new on 0.2.0');
    RETURN;
  END IF;

  PREPARE jobs AS SELECT * FROM jobs;
  PREPARE jobs_time_windows AS SELECT * FROM jobs_time_windows;
  PREPARE shipments AS SELECT * FROM shipments;
  PREPARE shipments_time_windows AS SELECT * FROM shipments_time_windows;
  PREPARE vehicles AS SELECT * FROM vehicles;
  PREPARE breaks AS SELECT * FROM breaks;
  PREPARE breaks_time_windows AS SELECT * FROM breaks_time_windows;
  PREPARE matrix AS SELECT * FROM matrix;

  RETURN QUERY
  SELECT isnt_empty('jobs', 'Should be not empty to tests be meaningful');
  RETURN QUERY
  SELECT isnt_empty('jobs_time_windows', 'Should be not empty to tests be meaningful');
  RETURN QUERY
  SELECT isnt_empty('shipments', 'Should be not empty to tests be meaningful');
  RETURN QUERY
  SELECT isnt_empty('shipments_time_windows', 'Should be not empty to tests be meaningful');
  RETURN QUERY
  SELECT isnt_empty('vehicles', 'Should be not empty to tests be meaningful');
  RETURN QUERY
  SELECT isnt_empty('breaks', 'Should be not empty to tests be meaningful');
  RETURN QUERY
  SELECT isnt_empty('breaks_time_windows', 'Should be not empty to tests be meaningful');
  RETURN QUERY
  SELECT isnt_empty('matrix', 'Should be not empty to tests be meaningful');

  params = ARRAY[
    '$$jobs$$',
    '$$jobs_time_windows$$',
    '$$shipments$$',
    '$$shipments_time_windows$$',
    '$$vehicles$$',
    '$$breaks$$',
    '$$breaks_time_windows$$',
    '$$matrix$$',
    'exploration_level => 5::SMALLINT',
    'timeout => -1'
  ]::TEXT[];
  subs = ARRAY[
    'NULL',
    'NULL',
    'NULL',
    'NULL',
    'NULL',
    'NULL',
    'NULL',
    'NULL',
    'exploration_level => NULL',
    'timeout => NULL'
  ]::TEXT[];
  error_messages = ARRAY[
    '',
    '',
    '',
    '',
    'Vehicles SQL must not be NULL',
    '',
    '',
    'Matrix SQL must not be NULL',
    '',
    ''
  ]::TEXT[];
  non_empty_args = ARRAY[0, 1, 2, 3, 4, 6, 7, 9, 10]::INTEGER[];

  IF is_plain = TRUE THEN
    params[10] = 'timeout => -1';
    RETURN query SELECT * FROM no_crash_test('vrp_vroomPlain', params, subs, error_messages, non_empty_args);
  ELSE
    params[10] = 'timeout => $$-00:00:01$$::INTERVAL';
    RETURN query SELECT * FROM no_crash_test('vrp_vroom', params, subs, error_messages, non_empty_args);
  END IF;

  params = ARRAY[
    '$$jobs$$',
    '$$jobs_time_windows$$',
    '$$vehicles$$',
    '$$breaks$$',
    '$$breaks_time_windows$$',
    '$$matrix$$',
    'exploration_level => 5::SMALLINT',
    'timeout => -1'
  ]::TEXT[];
  subs = ARRAY[
    'NULL',
    'NULL',
    'NULL',
    'NULL',
    'NULL',
    'NULL',
    'exploration_level => NULL',
    'timeout => NULL'
  ]::TEXT[];
  error_messages = ARRAY[
    'Jobs SQL must not be NULL',
    '',
    'Vehicles SQL must not be NULL',
    '',
    '',
    'Matrix SQL must not be NULL',
    '',
    ''
  ]::TEXT[];
  non_empty_args = ARRAY[0, 2, 4, 5, 7, 8]::INTEGER[];

  IF is_plain = TRUE THEN
    params[8] = 'timeout => -1';
    RETURN query SELECT * FROM no_crash_test('vrp_vroomJobsPlain', params, subs, error_messages, non_empty_args);
  ELSE
    params[8] = 'timeout => $$-00:00:01$$::INTERVAL';
    RETURN query SELECT * FROM no_crash_test('vrp_vroomJobs', params, subs, error_messages, non_empty_args);
  END IF;

  params = ARRAY[
    '$$shipments$$',
    '$$shipments_time_windows$$',
    '$$vehicles$$',
    '$$breaks$$',
    '$$breaks_time_windows$$',
    '$$matrix$$',
    'exploration_level => 5::SMALLINT',
    'timeout => -1'
  ]::TEXT[];
  subs = ARRAY[
    'NULL',
    'NULL',
    'NULL',
    'NULL',
    'NULL',
    'NULL',
    'exploration_level => NULL',
    'timeout => NULL'
  ]::TEXT[];
  error_messages = ARRAY[
    'Shipments SQL must not be NULL',
    '',
    'Vehicles SQL must not be NULL',
    '',
    '',
    'Matrix SQL must not be NULL',
    '',
    ''
  ]::TEXT[];
  non_empty_args = ARRAY[0, 2, 4, 5, 7, 8]::INTEGER[];

  IF is_plain = TRUE THEN
    params[8] = 'timeout => -1';
    RETURN query SELECT * FROM no_crash_test('vrp_vroomShipmentsPlain', params, subs, error_messages, non_empty_args);
  ELSE
    params[8] = 'timeout => $$-00:00:01$$::INTERVAL';
    RETURN query SELECT * FROM no_crash_test('vrp_vroomShipments', params, subs, error_messages, non_empty_args);
  END IF;

  DEALLOCATE ALL;

END
$BODY$
LANGUAGE plpgsql VOLATILE;


SELECT * FROM no_crash(is_plain => TRUE);

-- Adjust the column types to the expected types for vroom functions with timestamps/interval
ALTER TABLE vroom.jobs ALTER COLUMN service TYPE INTERVAL USING make_interval(secs => service);
ALTER TABLE vroom.shipments ALTER COLUMN p_service TYPE INTERVAL USING make_interval(secs => p_service);
ALTER TABLE vroom.shipments ALTER COLUMN d_service TYPE INTERVAL USING make_interval(secs => d_service);
ALTER TABLE vroom.vehicles ALTER COLUMN tw_open TYPE TIMESTAMP USING (to_timestamp(tw_open + 1630573200) at time zone 'UTC')::TIMESTAMP;
ALTER TABLE vroom.vehicles ALTER COLUMN tw_close TYPE TIMESTAMP USING (to_timestamp(tw_close + 1630573200) at time zone 'UTC')::TIMESTAMP;
ALTER TABLE vroom.breaks ALTER COLUMN service TYPE INTERVAL USING make_interval(secs => service);
ALTER TABLE vroom.jobs_time_windows ALTER COLUMN tw_open TYPE TIMESTAMP USING (to_timestamp(tw_open + 1630573200) at time zone 'UTC')::TIMESTAMP;
ALTER TABLE vroom.jobs_time_windows ALTER COLUMN tw_close TYPE TIMESTAMP USING (to_timestamp(tw_close + 1630573200) at time zone 'UTC')::TIMESTAMP;
ALTER TABLE vroom.shipments_time_windows ALTER COLUMN tw_open TYPE TIMESTAMP USING (to_timestamp(tw_open + 1630573200) at time zone 'UTC')::TIMESTAMP;
ALTER TABLE vroom.shipments_time_windows ALTER COLUMN tw_close TYPE TIMESTAMP USING (to_timestamp(tw_close + 1630573200) at time zone 'UTC')::TIMESTAMP;
ALTER TABLE vroom.breaks_time_windows ALTER COLUMN tw_open TYPE TIMESTAMP USING (to_timestamp(tw_open + 1630573200) at time zone 'UTC')::TIMESTAMP;
ALTER TABLE vroom.breaks_time_windows ALTER COLUMN tw_close TYPE TIMESTAMP USING (to_timestamp(tw_close + 1630573200) at time zone 'UTC')::TIMESTAMP;
ALTER TABLE vroom.matrix ALTER COLUMN duration TYPE INTERVAL USING make_interval(secs => duration);

SELECT * FROM no_crash(is_plain => FALSE);

ROLLBACK;
