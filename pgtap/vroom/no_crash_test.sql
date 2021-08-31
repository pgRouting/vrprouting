BEGIN;
SET search_path TO 'vroom', 'public';
SET client_min_messages TO ERROR;

SELECT CASE WHEN min_version('0.2.0') THEN plan (46) ELSE plan(1) END;

PREPARE jobs AS SELECT * FROM jobs;
PREPARE jobs_time_windows AS SELECT * FROM jobs_time_windows;
PREPARE shipments AS SELECT * FROM shipments;
PREPARE shipments_time_windows AS SELECT * FROM shipments_time_windows;
PREPARE vehicles AS SELECT * FROM vehicles;
PREPARE breaks AS SELECT * FROM breaks;
PREPARE breaks_time_windows AS SELECT * FROM breaks_time_windows;
PREPARE matrix AS SELECT * FROM matrix;

CREATE OR REPLACE FUNCTION no_crash()
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
    '$$matrix$$'
  ]::TEXT[];
  subs = ARRAY[
    'NULL',
    'NULL',
    'NULL',
    'NULL',
    'NULL',
    'NULL',
    'NULL',
    'NULL'
  ]::TEXT[];
  error_messages = ARRAY[
    '',
    '',
    '',
    '',
    'Vehicles SQL must not be NULL',
    '',
    '',
    'Matrix SQL must not be NULL'
  ]::TEXT[];
  non_empty_args = ARRAY[0, 1, 2, 3, 4, 6, 7]::INTEGER[];

  RETURN query SELECT * FROM no_crash_test('vrp_vroomPlain', params, subs, error_messages, non_empty_args);

  params = ARRAY[
    '$$jobs$$',
    '$$jobs_time_windows$$',
    '$$vehicles$$',
    '$$breaks$$',
    '$$breaks_time_windows$$',
    '$$matrix$$'
  ]::TEXT[];
  subs = ARRAY[
    'NULL',
    'NULL',
    'NULL',
    'NULL',
    'NULL',
    'NULL'
  ]::TEXT[];
  error_messages = ARRAY[
    'Jobs SQL must not be NULL',
    '',
    'Vehicles SQL must not be NULL',
    '',
    '',
    'Matrix SQL must not be NULL'
  ]::TEXT[];
  non_empty_args = ARRAY[0, 2, 4, 5]::INTEGER[];

  RETURN query SELECT * FROM no_crash_test('vrp_vroomJobsPlain', params, subs, error_messages, non_empty_args);

  params = ARRAY[
    '$$shipments$$',
    '$$shipments_time_windows$$',
    '$$vehicles$$',
    '$$breaks$$',
    '$$breaks_time_windows$$',
    '$$matrix$$'
  ]::TEXT[];
  subs = ARRAY[
    'NULL',
    'NULL',
    'NULL',
    'NULL',
    'NULL',
    'NULL'
  ]::TEXT[];
  error_messages = ARRAY[
    'Shipments SQL must not be NULL',
    '',
    'Vehicles SQL must not be NULL',
    '',
    '',
    'Matrix SQL must not be NULL'
  ]::TEXT[];
  non_empty_args = ARRAY[0, 2, 4, 5]::INTEGER[];

  RETURN query SELECT * FROM no_crash_test('vrp_vroomShipmentsPlain'::TEXT, params, subs, error_messages, non_empty_args);

END
$BODY$
LANGUAGE plpgsql VOLATILE;


SELECT * FROM no_crash();

ROLLBACK;
