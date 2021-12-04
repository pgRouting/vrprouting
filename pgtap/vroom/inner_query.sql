BEGIN;
SET search_path TO 'vroom', 'public';

SELECT CASE WHEN min_version('0.2.0') THEN plan (3504) ELSE plan(1) END;

/*
SELECT * FROM vrp_vroomPlain(
  $$SELECT id, location_index, service, delivery, pickup, skills, priority FROM jobs$$,
  $$SELECT id, tw_open, tw_close FROM jobs_time_windows$$,
  $$SELECT id, p_location_index, p_service, d_location_index, d_service, amount, skills, priority FROM shipments$$,
  $$SELECT id, kind, tw_open, tw_close FROM shipments_time_windows$$,
  $$SELECT id, start_index, end_index, capacity, skills, tw_open, tw_close FROM vehicles$$,
  $$SELECT id, vehicle_id, service FROM breaks$$,
  $$SELECT id, tw_open, tw_close FROM breaks_time_windows$$,
  $$SELECT start_id, end_id, duration FROM matrix$$
);
*/

CREATE OR REPLACE FUNCTION test_value(fn TEXT, inner_query_table TEXT, start_sql TEXT, rest_sql TEXT, params TEXT[], parameter TEXT, accept TEXT[], reject TEXT[])
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  end_sql TEXT;
  query TEXT;
  p TEXT;
  type_name TEXT;
BEGIN
  start_sql = 'SELECT * FROM ' || fn || '(' || start_sql || '$$ SELECT ';
  FOREACH p IN ARRAY params
  LOOP
    IF p = parameter THEN CONTINUE;
    END IF;
    start_sql = start_sql || p || ', ';
  END LOOP;
  end_sql = ' FROM ' || inner_query_table || '$$' || rest_sql;

  FOREACH type_name IN ARRAY accept
  LOOP
    query := start_sql || parameter || '::' || type_name || end_sql;
    RETURN query SELECT lives_ok(query);
  END LOOP;

  FOREACH type_name IN ARRAY reject
  LOOP
    query := start_sql || parameter || '::' || type_name || end_sql;
    RETURN query SELECT throws_ok(query);
  END LOOP;
END;
$BODY$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION test_anyInteger(fn TEXT, inner_query_table TEXT, start_sql TEXT, rest_sql TEXT, params TEXT[], parameter TEXT)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  accept TEXT[] := ARRAY['SMALLINT', 'INTEGER', 'BIGINT'];
  reject TEXT[] := ARRAY['REAL', 'FLOAT8', 'NUMERIC'];
BEGIN
  RETURN query SELECT test_value(fn, inner_query_table, start_sql, rest_sql, params, parameter, accept, reject);
END;
$BODY$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION test_Integer(fn TEXT, inner_query_table TEXT, start_sql TEXT, rest_sql TEXT, params TEXT[], parameter TEXT)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  accept TEXT[] := ARRAY['SMALLINT', 'INTEGER'];
  reject TEXT[] := ARRAY['BIGINT', 'REAL', 'FLOAT8', 'NUMERIC'];
BEGIN
  RETURN query SELECT test_value(fn, inner_query_table, start_sql, rest_sql, params, parameter, accept, reject);
END;
$BODY$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION test_anyArrayInteger(fn TEXT, inner_query_table TEXT, start_sql TEXT, rest_sql TEXT, params TEXT[], parameter TEXT)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  accept TEXT[] := ARRAY['SMALLINT[]', 'INTEGER[]', 'BIGINT[]'];
  reject TEXT[] := ARRAY['REAL[]', 'FLOAT8[]', 'NUMERIC[]'];
BEGIN
  RETURN query SELECT test_value(fn, inner_query_table, start_sql, rest_sql, params, parameter, accept, reject);
END;
$BODY$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION test_arrayInteger(fn TEXT, inner_query_table TEXT, start_sql TEXT, rest_sql TEXT, params TEXT[], parameter TEXT)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  accept TEXT[] := ARRAY['SMALLINT[]', 'INTEGER[]'];
  reject TEXT[] := ARRAY['BIGINT[]', 'REAL[]', 'FLOAT8[]', 'NUMERIC[]'];
BEGIN
  RETURN query SELECT test_value(fn, inner_query_table, start_sql, rest_sql, params, parameter, accept, reject);
END;
$BODY$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION test_anyNumerical(fn TEXT, inner_query_table TEXT, start_sql TEXT, rest_sql TEXT, params TEXT[], parameter TEXT)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  accept TEXT[] := ARRAY['SMALLINT', 'INTEGER', 'BIGINT', 'REAL', 'FLOAT8', 'NUMERIC'];
  reject TEXT[] := ARRAY[]::TEXT[];
BEGIN
  RETURN query SELECT test_value(fn, inner_query_table, start_sql, rest_sql, params, parameter, accept, reject);
END;
$BODY$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION test_Char(fn TEXT, inner_query_table TEXT, start_sql TEXT, rest_sql TEXT, params TEXT[], parameter TEXT)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  accept TEXT[] := ARRAY['CHAR'];
  reject TEXT[] := ARRAY['VARCHAR', 'TEXT'];
BEGIN
  RETURN query SELECT test_value(fn, inner_query_table, start_sql, rest_sql, params, parameter, accept, reject);
END;
$BODY$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION test_Interval(fn TEXT, inner_query_table TEXT, start_sql TEXT, rest_sql TEXT, params TEXT[], parameter TEXT)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  accept TEXT[] := ARRAY['INTERVAL'];
  reject TEXT[] := ARRAY['TIMESTAMP', 'DATE', 'TIME', 'INTEGER'];
BEGIN
  RETURN query SELECT test_value(fn, inner_query_table, start_sql, rest_sql, params, parameter, accept, reject);
END;
$BODY$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION test_Timestamp(fn TEXT, inner_query_table TEXT, start_sql TEXT, rest_sql TEXT, params TEXT[], parameter TEXT)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  accept TEXT[] := ARRAY['TIMESTAMP'];
  reject TEXT[] := ARRAY['INTERVAL', 'DATE', 'TIME', 'INTEGER'];
BEGIN
  RETURN query SELECT test_value(fn, inner_query_table, start_sql, rest_sql, params, parameter, accept, reject);
END;
$BODY$ LANGUAGE plpgsql;



CREATE OR REPLACE FUNCTION inner_query_jobs(fn TEXT, start_sql TEXT, rest_sql TEXT, is_plain BOOLEAN)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  inner_query_table TEXT := 'jobs';
  params TEXT[] := ARRAY['id', 'location_index', 'service', 'delivery', 'pickup', 'skills', 'priority'];
BEGIN
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'id');
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'location_index');
  RETURN QUERY SELECT test_anyArrayInteger(fn, inner_query_table, start_sql, rest_sql, params, 'delivery');
  RETURN QUERY SELECT test_anyArrayInteger(fn, inner_query_table, start_sql, rest_sql, params, 'pickup');
  RETURN QUERY SELECT test_arrayInteger(fn, inner_query_table, start_sql, rest_sql, params, 'skills');
  RETURN QUERY SELECT test_Integer(fn, inner_query_table, start_sql, rest_sql, params, 'priority');
  IF is_plain = TRUE THEN
    RETURN QUERY SELECT test_Integer(fn, inner_query_table, start_sql, rest_sql, params, 'service');
  ELSE
    RETURN QUERY SELECT test_Interval(fn, inner_query_table, start_sql, rest_sql, params, 'service');
  END IF;
END;
$BODY$
LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION inner_query_shipments(fn TEXT, start_sql TEXT, rest_sql TEXT, is_plain BOOLEAN)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  inner_query_table TEXT := 'shipments';
  params TEXT[] := ARRAY['id', 'p_location_index', 'p_service', 'd_location_index', 'd_service', 'amount', 'skills', 'priority'];
BEGIN
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'id');
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'p_location_index');
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'd_location_index');
  RETURN QUERY SELECT test_anyArrayInteger(fn, inner_query_table, start_sql, rest_sql, params, 'amount');
  RETURN QUERY SELECT test_arrayInteger(fn, inner_query_table, start_sql, rest_sql, params, 'skills');
  RETURN QUERY SELECT test_Integer(fn, inner_query_table, start_sql, rest_sql, params, 'priority');
  IF is_plain = TRUE THEN
    RETURN QUERY SELECT test_Integer(fn, inner_query_table, start_sql, rest_sql, params, 'p_service');
    RETURN QUERY SELECT test_Integer(fn, inner_query_table, start_sql, rest_sql, params, 'd_service');
  ELSE
    RETURN QUERY SELECT test_Interval(fn, inner_query_table, start_sql, rest_sql, params, 'p_service');
    RETURN QUERY SELECT test_Interval(fn, inner_query_table, start_sql, rest_sql, params, 'd_service');
  END IF;
END;
$BODY$
LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION inner_query_vehicles(fn TEXT, start_sql TEXT, rest_sql TEXT, is_plain BOOLEAN)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  inner_query_table TEXT := 'vehicles';
  params TEXT[] := ARRAY['id', 'start_index', 'end_index', 'capacity', 'skills', 'tw_open', 'tw_close', 'speed_factor', 'max_tasks'];
BEGIN
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'id');
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'start_index');
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'end_index');
  RETURN QUERY SELECT test_anyArrayInteger(fn, inner_query_table, start_sql, rest_sql, params, 'capacity');
  RETURN QUERY SELECT test_arrayInteger(fn, inner_query_table, start_sql, rest_sql, params, 'skills');
  RETURN QUERY SELECT test_anyNumerical(fn, inner_query_table, start_sql, rest_sql, params, 'speed_factor');
  IF is_plain = TRUE THEN
    RETURN QUERY SELECT test_Integer(fn, inner_query_table, start_sql, rest_sql, params, 'tw_open');
    RETURN QUERY SELECT test_Integer(fn, inner_query_table, start_sql, rest_sql, params, 'tw_close');
  ELSE
    RETURN QUERY SELECT test_Timestamp(fn, inner_query_table, start_sql, rest_sql, params, 'tw_open');
    RETURN QUERY SELECT test_Timestamp(fn, inner_query_table, start_sql, rest_sql, params, 'tw_close');
  END IF;
  RETURN QUERY SELECT test_Integer(fn, inner_query_table, start_sql, rest_sql, params, 'max_tasks');
END;
$BODY$
LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION inner_query_matrix(fn TEXT, start_sql TEXT, rest_sql TEXT, is_plain BOOLEAN)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  inner_query_table TEXT := 'matrix';
  params TEXT[] := ARRAY['start_id', 'end_id', 'duration', 'cost'];
BEGIN
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'start_id');
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'end_id');
  IF is_plain = TRUE THEN
    RETURN QUERY SELECT test_Integer(fn, inner_query_table, start_sql, rest_sql, params, 'duration');
  ELSE
    RETURN QUERY SELECT test_Interval(fn, inner_query_table, start_sql, rest_sql, params, 'duration');
  END IF;
  RETURN QUERY SELECT test_Integer(fn, inner_query_table, start_sql, rest_sql, params, 'cost');
END;
$BODY$
LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION inner_query_breaks(fn TEXT, start_sql TEXT, rest_sql TEXT, is_plain BOOLEAN)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  inner_query_table TEXT := 'breaks';
  params TEXT[] := ARRAY['id', 'vehicle_id', 'service'];
BEGIN
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'id');
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'vehicle_id');
  IF is_plain = TRUE THEN
    RETURN QUERY SELECT test_Integer(fn, inner_query_table, start_sql, rest_sql, params, 'service');
  ELSE
    RETURN QUERY SELECT test_Interval(fn, inner_query_table, start_sql, rest_sql, params, 'service');
  END IF;
END;
$BODY$
LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION inner_query_time_windows(fn TEXT, inner_query_table TEXT, start_sql TEXT, rest_sql TEXT, is_plain BOOLEAN)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  params TEXT[] := ARRAY['id', 'tw_open', 'tw_close'];
BEGIN
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'id');
  IF is_plain = TRUE THEN
    RETURN QUERY SELECT test_Integer(fn, inner_query_table, start_sql, rest_sql, params, 'tw_open');
    RETURN QUERY SELECT test_Integer(fn, inner_query_table, start_sql, rest_sql, params, 'tw_close');
  ELSE
    RETURN QUERY SELECT test_Timestamp(fn, inner_query_table, start_sql, rest_sql, params, 'tw_open');
    RETURN QUERY SELECT test_Timestamp(fn, inner_query_table, start_sql, rest_sql, params, 'tw_close');
  END IF;
END;
$BODY$
LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION inner_query_shipments_time_windows(fn TEXT, inner_query_table TEXT, start_sql TEXT, rest_sql TEXT, is_plain BOOLEAN)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  params TEXT[] := ARRAY['id', 'kind', 'tw_open', 'tw_close'];
BEGIN
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'id');
  RETURN QUERY SELECT test_Char(fn, inner_query_table, start_sql, rest_sql, params, 'kind');
  IF is_plain = TRUE THEN
    RETURN QUERY SELECT test_Integer(fn, inner_query_table, start_sql, rest_sql, params, 'tw_open');
    RETURN QUERY SELECT test_Integer(fn, inner_query_table, start_sql, rest_sql, params, 'tw_close');
  ELSE
    RETURN QUERY SELECT test_Timestamp(fn, inner_query_table, start_sql, rest_sql, params, 'tw_open');
    RETURN QUERY SELECT test_Timestamp(fn, inner_query_table, start_sql, rest_sql, params, 'tw_close');
  END IF;
END;
$BODY$
LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION inner_query(is_plain BOOLEAN, additional TEXT)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  fn TEXT;
  start_sql TEXT;
  rest_sql TEXT;
  inner_query_table TEXT;
BEGIN

  IF NOT min_version('0.2.0') THEN
    RETURN QUERY
    SELECT skip(1, 'Function is new on 0.2.0');
    RETURN;
  END IF;

  -- vrp_vroom

  IF is_plain = TRUE THEN
    fn := 'vrp_vroomPlain';
  ELSE
    fn := 'vrp_vroom';
  END IF;
  start_sql := '';
  rest_sql := ', $$SELECT * FROM jobs_time_windows$$, $$SELECT * FROM shipments$$, $$SELECT * FROM shipments_time_windows$$, ' ||
              '$$SELECT * FROM vehicles$$, $$SELECT * FROM breaks$$, ' ||
              '$$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$' || additional;
  RETURN QUERY SELECT inner_query_jobs(fn, start_sql, rest_sql, is_plain);

  start_sql := '$$SELECT * FROM jobs$$, ';
  rest_sql := ', $$SELECT * FROM shipments$$, $$SELECT * FROM shipments_time_windows$$, ' ||
              '$$SELECT * FROM vehicles$$, $$SELECT * FROM breaks$$, ' ||
              '$$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$' || additional;
  inner_query_table := 'jobs_time_windows';
  RETURN QUERY SELECT inner_query_time_windows(fn, inner_query_table, start_sql, rest_sql, is_plain);

  start_sql := '$$SELECT * FROM jobs$$, $$SELECT * FROM jobs_time_windows$$, ';
  rest_sql := ', $$SELECT * FROM shipments_time_windows$$, ' ||
              '$$SELECT * FROM vehicles$$, $$SELECT * FROM breaks$$, ' ||
              '$$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$' || additional;
  RETURN QUERY SELECT inner_query_shipments(fn, start_sql, rest_sql, is_plain);

  start_sql := '$$SELECT * FROM jobs$$, $$SELECT * FROM jobs_time_windows$$, $$SELECT * FROM shipments$$, ';
  rest_sql := ', $$SELECT * FROM vehicles$$, $$SELECT * FROM breaks$$, ' ||
              '$$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$' || additional;
  inner_query_table := 'shipments_time_windows';
  RETURN QUERY SELECT inner_query_shipments_time_windows(fn, inner_query_table, start_sql, rest_sql, is_plain);

  start_sql := '$$SELECT * FROM jobs$$, $$SELECT * FROM jobs_time_windows$$, $$SELECT * FROM shipments$$, ' ||
               '$$SELECT * FROM shipments_time_windows$$, ';
  rest_sql := ', $$SELECT * FROM breaks$$, ' ||
              '$$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$' || additional;
  RETURN QUERY SELECT inner_query_vehicles(fn, start_sql, rest_sql, is_plain);

  start_sql := '$$SELECT * FROM jobs$$, $$SELECT * FROM jobs_time_windows$$, $$SELECT * FROM shipments$$, ' ||
               '$$SELECT * FROM shipments_time_windows$$, $$SELECT * FROM vehicles$$, ';
  rest_sql := ', $$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$' || additional;
  RETURN QUERY SELECT inner_query_breaks(fn, start_sql, rest_sql, is_plain);

  start_sql := '$$SELECT * FROM jobs$$, $$SELECT * FROM jobs_time_windows$$, $$SELECT * FROM shipments$$, ' ||
               '$$SELECT * FROM shipments_time_windows$$, $$SELECT * FROM vehicles$$, ' ||
               '$$SELECT * FROM breaks$$, ';
  rest_sql := ', $$SELECT * FROM matrix$$' || additional;
  inner_query_table := 'breaks_time_windows';
  RETURN QUERY SELECT inner_query_time_windows(fn, inner_query_table, start_sql, rest_sql, is_plain);

  start_sql := '$$SELECT * FROM jobs$$, $$SELECT * FROM jobs_time_windows$$, $$SELECT * FROM shipments$$, ' ||
               '$$SELECT * FROM shipments_time_windows$$, $$SELECT * FROM vehicles$$, ' ||
               '$$SELECT * FROM breaks$$, $$SELECT * FROM breaks_time_windows$$, ';
  rest_sql := additional;
  RETURN QUERY SELECT inner_query_matrix(fn, start_sql, rest_sql, is_plain);


  -- vrp_vroomJobs

  IF is_plain = TRUE THEN
    fn := 'vrp_vroomJobsPlain';
  ELSE
    fn := 'vrp_vroomJobs';
  END IF;
  start_sql := '';
  rest_sql := ', $$SELECT * FROM jobs_time_windows$$, $$SELECT * FROM vehicles$$, $$SELECT * FROM breaks$$, ' ||
              '$$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$' || additional;
  RETURN QUERY SELECT inner_query_jobs(fn, start_sql, rest_sql, is_plain);

  start_sql := '$$SELECT * FROM jobs$$, ';
  rest_sql := ', $$SELECT * FROM vehicles$$, $$SELECT * FROM breaks$$, ' ||
              '$$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$' || additional;
  inner_query_table := 'jobs_time_windows';
  RETURN QUERY SELECT inner_query_time_windows(fn, inner_query_table, start_sql, rest_sql, is_plain);

  start_sql := '$$SELECT * FROM jobs$$, $$SELECT * FROM jobs_time_windows$$, ';
  rest_sql := ', $$SELECT * FROM breaks$$, ' ||
              '$$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$' || additional;
  RETURN QUERY SELECT inner_query_vehicles(fn, start_sql, rest_sql, is_plain);

  start_sql := '$$SELECT * FROM jobs$$, $$SELECT * FROM jobs_time_windows$$, ' ||
               '$$SELECT * FROM vehicles$$, ';
  rest_sql := ', $$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$' || additional;
  RETURN QUERY SELECT inner_query_breaks(fn, start_sql, rest_sql, is_plain);

  start_sql := '$$SELECT * FROM jobs$$, $$SELECT * FROM jobs_time_windows$$, ' ||
               '$$SELECT * FROM vehicles$$, $$SELECT * FROM breaks$$, ';
  rest_sql := ', $$SELECT * FROM matrix$$' || additional;
  inner_query_table := 'breaks_time_windows';
  RETURN QUERY SELECT inner_query_time_windows(fn, inner_query_table, start_sql, rest_sql, is_plain);

  start_sql := '$$SELECT * FROM jobs$$, $$SELECT * FROM jobs_time_windows$$, ' ||
               '$$SELECT * FROM vehicles$$, $$SELECT * FROM breaks$$, $$SELECT * FROM breaks_time_windows$$, ';
  rest_sql :=  additional;
  RETURN QUERY SELECT inner_query_matrix(fn, start_sql, rest_sql, is_plain);


  -- vrp_vroomShipments

  IF is_plain = TRUE THEN
    fn := 'vrp_vroomShipmentsPlain';
  ELSE
    fn := 'vrp_vroomShipments';
  END IF;
  start_sql := '';
  rest_sql := ', $$SELECT * FROM shipments_time_windows$$, $$SELECT * FROM vehicles$$, $$SELECT * FROM breaks$$, ' ||
              '$$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$' || additional;
  RETURN QUERY SELECT inner_query_shipments(fn, start_sql, rest_sql, is_plain);

  start_sql := '$$SELECT * FROM shipments$$, ';
  rest_sql := ', $$SELECT * FROM vehicles$$, $$SELECT * FROM breaks$$, ' ||
              '$$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$' || additional;
  inner_query_table := 'shipments_time_windows';
  RETURN QUERY SELECT inner_query_shipments_time_windows(fn, inner_query_table, start_sql, rest_sql, is_plain);

  start_sql := '$$SELECT * FROM shipments$$, $$SELECT * FROM shipments_time_windows$$, ';
  rest_sql := ', $$SELECT * FROM breaks$$, $$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$' || additional;
  RETURN QUERY SELECT inner_query_vehicles(fn, start_sql, rest_sql, is_plain);

  start_sql := '$$SELECT * FROM shipments$$, $$SELECT * FROM shipments_time_windows$$, ' ||
               '$$SELECT * FROM vehicles$$, ';
  rest_sql := ', $$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$' || additional;
  RETURN QUERY SELECT inner_query_breaks(fn, start_sql, rest_sql, is_plain);

  start_sql := '$$SELECT * FROM shipments$$, $$SELECT * FROM shipments_time_windows$$, ' ||
               '$$SELECT * FROM vehicles$$, $$SELECT * FROM breaks$$, ';
  rest_sql := ', $$SELECT * FROM matrix$$' || additional;
  inner_query_table := 'breaks_time_windows';
  RETURN QUERY SELECT inner_query_time_windows(fn, inner_query_table, start_sql, rest_sql, is_plain);

  start_sql := '$$SELECT * FROM shipments$$, $$SELECT * FROM shipments_time_windows$$, ' ||
               '$$SELECT * FROM vehicles$$, $$SELECT * FROM breaks$$, $$SELECT * FROM breaks_time_windows$$, ';
  rest_sql := additional;
  RETURN QUERY SELECT inner_query_matrix(fn, start_sql, rest_sql, is_plain);

END;
$BODY$
LANGUAGE plpgsql;


SELECT inner_query(is_plain => TRUE, additional => ', exploration_level => 5)');
SELECT inner_query(is_plain => TRUE, additional => ', timeout => -1)');
SELECT inner_query(is_plain => TRUE, additional => ', exploration_level => 5, timeout => -1)');

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

SELECT inner_query(is_plain => FALSE, additional => ', exploration_level => 5)');
SELECT inner_query(is_plain => FALSE, additional => ', timeout => $$-00:00:01$$::INTERVAL)');
SELECT inner_query(is_plain => FALSE, additional => ', exploration_level => 5, timeout => $$-00:00:01$$::INTERVAL)');


SELECT * FROM finish();
ROLLBACK;
