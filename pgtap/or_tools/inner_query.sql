BEGIN;
SET search_path TO 'ortools', 'public';

SELECT CASE WHEN min_version('0.3.0') THEN plan (48) ELSE plan(1) END;

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
    RETURN query SELECT throws_ok(query, 38000);
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


CREATE OR REPLACE FUNCTION inner_query()
RETURNS SETOF TEXT AS
$BODY$
DECLARE
  fn TEXT;
  start_sql TEXT;
  rest_sql TEXT;
  inner_query_table TEXT;
  params TEXT[];
BEGIN
  IF NOT min_version('0.3.0') THEN
    RETURN QUERY
    SELECT skip(1, 'Function is modified on 0.3.0');
    RETURN;
  END IF;

  -- vrp_knapsack
  fn := 'vrp_knapsack';
  inner_query_table := 'knapsack_data';
  start_sql := '';
  rest_sql := ', 15)';
  params := ARRAY['id', 'weight', 'cost'];
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'id');
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'weight');
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'cost');

  -- vrp_multiple_knapsack
  fn := 'vrp_multiple_knapsack';
  inner_query_table := 'multiple_knapsack_data';
  start_sql := '';
  rest_sql := ', ARRAY[100,100,100,100,100])';
  params := ARRAY['id', 'weight', 'cost'];
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'id');
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'weight');
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'cost');
  
  -- bin_packing
  fn := 'vrp_bin_packing';
  inner_query_table := 'bin_packing_data';
  start_sql := '';
  rest_sql := ', 100)';
  params := ARRAY['id', 'weight'];
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'id');
  RETURN QUERY SELECT test_anyInteger(fn, inner_query_table, start_sql, rest_sql, params, 'weight');

END;
$BODY$
LANGUAGE plpgsql;

SELECT inner_query();



SELECT * FROM finish();
ROLLBACK;
