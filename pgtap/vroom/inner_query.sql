BEGIN;
SET search_path TO 'vroom', 'public';

SELECT CASE WHEN min_version('0.2.0') THEN plan (564) ELSE plan(1) END;

/*
SELECT * FROM vrp_vroomPlain(
  $$SELECT id, location_index, service, delivery, pickup, skills, priority FROM jobs$$,
  $$SELECT id, tw_open, tw_close FROM jobs_time_windows$$,
  $$SELECT id, p_location_index, p_service, d_location_index, d_service, amount, skills, priority FROM shipments$$,
  $$SELECT id, kind, tw_open, tw_close FROM shipments_time_windows$$,
  $$SELECT id, start_index, end_index, capacity, skills, tw_open, tw_close FROM vehicles$$,
  $$SELECT id, vehicle_id, service FROM breaks$$,
  $$SELECT id, tw_open, tw_close FROM breaks_time_windows$$,
  $$SELECT start_vid, end_vid, agg_cost FROM matrix$$
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
  reject TEXT[] := ARRAY['VARCHAR', 'TEXT']::TEXT[];
BEGIN
  RETURN query SELECT test_value(fn, inner_query_table, start_sql, rest_sql, params, parameter, accept, reject);
END;
$BODY$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION inner_query_jobs(fn TEXT, start_sql TEXT, rest_sql TEXT)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  inner_query_table TEXT := 'jobs';
  params TEXT[] := ARRAY['id', 'location_index', 'service', 'delivery', 'pickup', 'skills', 'priority'];
BEGIN
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'id');
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'location_index');
  RETURN QUERY SELECT test_Integer(fn, inner_query_table, start_sql, rest_sql, params, 'service');
  RETURN QUERY SELECT test_anyArrayInteger(fn, inner_query_table, start_sql, rest_sql, params, 'delivery');
  RETURN QUERY SELECT test_anyArrayInteger(fn, inner_query_table, start_sql, rest_sql, params, 'pickup');
  RETURN QUERY SELECT test_arrayInteger(fn, inner_query_table, start_sql, rest_sql, params, 'skills');
  RETURN QUERY SELECT test_Integer(fn, inner_query_table, start_sql, rest_sql, params, 'priority');
END;
$BODY$
LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION inner_query_shipments(fn TEXT, start_sql TEXT, rest_sql TEXT)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  inner_query_table TEXT := 'shipments';
  params TEXT[] := ARRAY['id', 'p_location_index', 'p_service', 'd_location_index', 'd_service', 'amount', 'skills', 'priority'];
BEGIN
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'id');
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'p_location_index');
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'd_location_index');
  RETURN QUERY SELECT test_Integer(fn, inner_query_table, start_sql, rest_sql, params, 'p_service');
  RETURN QUERY SELECT test_Integer(fn, inner_query_table, start_sql, rest_sql, params, 'd_service');
  RETURN QUERY SELECT test_anyArrayInteger(fn, inner_query_table, start_sql, rest_sql, params, 'amount');
  RETURN QUERY SELECT test_arrayInteger(fn, inner_query_table, start_sql, rest_sql, params, 'skills');
  RETURN QUERY SELECT test_Integer(fn, inner_query_table, start_sql, rest_sql, params, 'priority');
END;
$BODY$
LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION inner_query_vehicles(fn TEXT, start_sql TEXT, rest_sql TEXT)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  inner_query_table TEXT := 'vehicles';
  params TEXT[] := ARRAY['id', 'start_index', 'end_index', 'capacity', 'skills', 'tw_open', 'tw_close', 'speed_factor'];
BEGIN
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'id');
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'start_index');
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'end_index');
  RETURN QUERY SELECT test_anyArrayInteger(fn, inner_query_table, start_sql, rest_sql, params, 'capacity');
  RETURN QUERY SELECT test_arrayInteger(fn, inner_query_table, start_sql, rest_sql, params, 'skills');
  RETURN QUERY SELECT test_Integer(fn, inner_query_table, start_sql, rest_sql, params, 'tw_open');
  RETURN QUERY SELECT test_Integer(fn, inner_query_table, start_sql, rest_sql, params, 'tw_close');
  RETURN QUERY SELECT test_anyNumerical(fn, inner_query_table, start_sql, rest_sql, params, 'speed_factor');
END;
$BODY$
LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION inner_query_matrix(fn TEXT, start_sql TEXT, rest_sql TEXT)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  inner_query_table TEXT := 'matrix';
  params TEXT[] := ARRAY['start_vid', 'end_vid', 'agg_cost'];
BEGIN
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'start_vid');
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'end_vid');
  RETURN QUERY SELECT test_Integer(fn, inner_query_table, start_sql, rest_sql, params, 'agg_cost');
END;
$BODY$
LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION inner_query_breaks(fn TEXT, start_sql TEXT, rest_sql TEXT)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  inner_query_table TEXT := 'breaks';
  params TEXT[] := ARRAY['id', 'vehicle_id', 'service'];
BEGIN
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'id');
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'vehicle_id');
  RETURN QUERY SELECT test_Integer(fn, inner_query_table, start_sql, rest_sql, params, 'service');
END;
$BODY$
LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION inner_query_time_windows(fn TEXT, inner_query_table TEXT, start_sql TEXT, rest_sql TEXT)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  params TEXT[] := ARRAY['id', 'tw_open', 'tw_close'];
BEGIN
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'id');
  RETURN QUERY SELECT test_Integer(fn, inner_query_table, start_sql, rest_sql, params, 'tw_open');
  RETURN QUERY SELECT test_Integer(fn, inner_query_table, start_sql, rest_sql, params, 'tw_close');
END;
$BODY$
LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION inner_query_shipments_time_windows(fn TEXT, inner_query_table TEXT, start_sql TEXT, rest_sql TEXT)
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  params TEXT[] := ARRAY['id', 'kind', 'tw_open', 'tw_close'];
BEGIN
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'id');
  RETURN QUERY SELECT test_Char(fn, inner_query_table, start_sql, rest_sql, params, 'kind');
  RETURN QUERY SELECT test_Integer(fn, inner_query_table, start_sql, rest_sql, params, 'tw_open');
  RETURN QUERY SELECT test_Integer(fn, inner_query_table, start_sql, rest_sql, params, 'tw_close');
END;
$BODY$
LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION inner_query()
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

  -- vrp_vroomPlain

  fn := 'vrp_vroomPlain';
  start_sql := '';
  rest_sql := ', $$SELECT * FROM jobs_time_windows$$, $$SELECT * FROM shipments$$, $$SELECT * FROM shipments_time_windows$$, ' ||
              '$$SELECT * FROM vehicles$$, $$SELECT * FROM breaks$$, ' ||
              '$$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$)';
  RETURN QUERY SELECT inner_query_jobs(fn, start_sql, rest_sql);

  start_sql := '$$SELECT * FROM jobs$$, ';
  rest_sql := ', $$SELECT * FROM shipments$$, $$SELECT * FROM shipments_time_windows$$, ' ||
              '$$SELECT * FROM vehicles$$, $$SELECT * FROM breaks$$, ' ||
              '$$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$)';
  inner_query_table := 'jobs_time_windows';
  RETURN QUERY SELECT inner_query_time_windows(fn, inner_query_table, start_sql, rest_sql);

  start_sql := '$$SELECT * FROM jobs$$, $$SELECT * FROM jobs_time_windows$$, ';
  rest_sql := ', $$SELECT * FROM shipments_time_windows$$, ' ||
              '$$SELECT * FROM vehicles$$, $$SELECT * FROM breaks$$, ' ||
              '$$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$)';
  RETURN QUERY SELECT inner_query_shipments(fn, start_sql, rest_sql);

  start_sql := '$$SELECT * FROM jobs$$, $$SELECT * FROM jobs_time_windows$$, $$SELECT * FROM shipments$$, ';
  rest_sql := ', $$SELECT * FROM vehicles$$, $$SELECT * FROM breaks$$, ' ||
              '$$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$)';
  inner_query_table := 'shipments_time_windows';
  RETURN QUERY SELECT inner_query_shipments_time_windows(fn, inner_query_table, start_sql, rest_sql);

  start_sql := '$$SELECT * FROM jobs$$, $$SELECT * FROM jobs_time_windows$$, $$SELECT * FROM shipments$$, ' ||
               '$$SELECT * FROM shipments_time_windows$$, ';
  rest_sql := ', $$SELECT * FROM breaks$$, ' ||
              '$$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$)';
  RETURN QUERY SELECT inner_query_vehicles(fn, start_sql, rest_sql);

  start_sql := '$$SELECT * FROM jobs$$, $$SELECT * FROM jobs_time_windows$$, $$SELECT * FROM shipments$$, ' ||
               '$$SELECT * FROM shipments_time_windows$$, $$SELECT * FROM vehicles$$, ';
  rest_sql := ', $$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$)';
  RETURN QUERY SELECT inner_query_breaks(fn, start_sql, rest_sql);

  start_sql := '$$SELECT * FROM jobs$$, $$SELECT * FROM jobs_time_windows$$, $$SELECT * FROM shipments$$, ' ||
               '$$SELECT * FROM shipments_time_windows$$, $$SELECT * FROM vehicles$$, ' ||
               '$$SELECT * FROM breaks$$, ';
  rest_sql := ', $$SELECT * FROM matrix$$)';
  inner_query_table := 'breaks_time_windows';
  RETURN QUERY SELECT inner_query_time_windows(fn, inner_query_table, start_sql, rest_sql);

  start_sql := '$$SELECT * FROM jobs$$, $$SELECT * FROM jobs_time_windows$$, $$SELECT * FROM shipments$$, ' ||
               '$$SELECT * FROM shipments_time_windows$$, $$SELECT * FROM vehicles$$, ' ||
               '$$SELECT * FROM breaks$$, $$SELECT * FROM breaks_time_windows$$, ';
  rest_sql := ')';
  RETURN QUERY SELECT inner_query_matrix(fn, start_sql, rest_sql);


  -- vrp_vroomJobsPlain

  fn := 'vrp_vroomJobsPlain';
  start_sql := '';
  rest_sql := ', $$SELECT * FROM jobs_time_windows$$, $$SELECT * FROM vehicles$$, $$SELECT * FROM breaks$$, ' ||
              '$$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$)';
  RETURN QUERY SELECT inner_query_jobs(fn, start_sql, rest_sql);

  start_sql := '$$SELECT * FROM jobs$$, ';
  rest_sql := ', $$SELECT * FROM vehicles$$, $$SELECT * FROM breaks$$, ' ||
              '$$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$)';
  inner_query_table := 'jobs_time_windows';
  RETURN QUERY SELECT inner_query_time_windows(fn, inner_query_table, start_sql, rest_sql);

  start_sql := '$$SELECT * FROM jobs$$, $$SELECT * FROM jobs_time_windows$$, ';
  rest_sql := ', $$SELECT * FROM breaks$$, ' ||
              '$$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$)';
  RETURN QUERY SELECT inner_query_vehicles(fn, start_sql, rest_sql);

  start_sql := '$$SELECT * FROM jobs$$, $$SELECT * FROM jobs_time_windows$$, ' ||
               '$$SELECT * FROM vehicles$$, ';
  rest_sql := ', $$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$)';
  RETURN QUERY SELECT inner_query_breaks(fn, start_sql, rest_sql);

  start_sql := '$$SELECT * FROM jobs$$, $$SELECT * FROM jobs_time_windows$$, ' ||
               '$$SELECT * FROM vehicles$$, $$SELECT * FROM breaks$$, ';
  rest_sql := ', $$SELECT * FROM matrix$$)';
  inner_query_table := 'breaks_time_windows';
  RETURN QUERY SELECT inner_query_time_windows(fn, inner_query_table, start_sql, rest_sql);

  start_sql := '$$SELECT * FROM jobs$$, $$SELECT * FROM jobs_time_windows$$, ' ||
               '$$SELECT * FROM vehicles$$, $$SELECT * FROM breaks$$, $$SELECT * FROM breaks_time_windows$$, ';
  rest_sql := ')';
  RETURN QUERY SELECT inner_query_matrix(fn, start_sql, rest_sql);


  -- vrp_vroomShipmentsPlain

  fn := 'vrp_vroomShipmentsPlain';
  start_sql := '';
  rest_sql := ', $$SELECT * FROM shipments_time_windows$$, $$SELECT * FROM vehicles$$, $$SELECT * FROM breaks$$, ' ||
              '$$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$)';
  RETURN QUERY SELECT inner_query_shipments(fn, start_sql, rest_sql);

  start_sql := '$$SELECT * FROM shipments$$, ';
  rest_sql := ', $$SELECT * FROM vehicles$$, $$SELECT * FROM breaks$$, ' ||
              '$$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$)';
  inner_query_table := 'shipments_time_windows';
  RETURN QUERY SELECT inner_query_shipments_time_windows(fn, inner_query_table, start_sql, rest_sql);

  start_sql := '$$SELECT * FROM shipments$$, $$SELECT * FROM shipments_time_windows$$, ';
  rest_sql := ', $$SELECT * FROM breaks$$, $$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$)';
  RETURN QUERY SELECT inner_query_vehicles(fn, start_sql, rest_sql);

  start_sql := '$$SELECT * FROM shipments$$, $$SELECT * FROM shipments_time_windows$$, ' ||
               '$$SELECT * FROM vehicles$$, ';
  rest_sql := ', $$SELECT * FROM breaks_time_windows$$, $$SELECT * FROM matrix$$)';
  RETURN QUERY SELECT inner_query_breaks(fn, start_sql, rest_sql);

  start_sql := '$$SELECT * FROM shipments$$, $$SELECT * FROM shipments_time_windows$$, ' ||
               '$$SELECT * FROM vehicles$$, $$SELECT * FROM breaks$$, ';
  rest_sql := ', $$SELECT * FROM matrix$$)';
  inner_query_table := 'breaks_time_windows';
  RETURN QUERY SELECT inner_query_time_windows(fn, inner_query_table, start_sql, rest_sql);

  start_sql := '$$SELECT * FROM shipments$$, $$SELECT * FROM shipments_time_windows$$, ' ||
               '$$SELECT * FROM vehicles$$, $$SELECT * FROM breaks$$, $$SELECT * FROM breaks_time_windows$$, ';
  rest_sql := ')';
  RETURN QUERY SELECT inner_query_matrix(fn, start_sql, rest_sql);
END;
$BODY$
LANGUAGE plpgsql;


SELECT inner_query();

SELECT * FROM finish();
ROLLBACK;
