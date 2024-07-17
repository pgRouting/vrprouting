BEGIN;
SET search_path TO 'ortools', 'public';

SELECT CASE WHEN min_version('0.3.0') THEN plan (15) ELSE plan(1) END;

CREATE OR REPLACE FUNCTION types_check()
RETURNS SETOF TEXT AS
$BODY$
BEGIN

  IF NOT min_version('0.3.0') THEN
    RETURN QUERY
    SELECT skip(1, 'Function is modified on 0.3.0');
    RETURN;
  END IF;

  -- vrp_knapsack
  RETURN QUERY
  SELECT has_function('vrp_knapsack');
  RETURN QUERY
  SELECT has_function('vrp_knapsack', ARRAY['text', 'integer', 'integer']);
  RETURN QUERY
  SELECT function_returns('vrp_knapsack', ARRAY['text', 'integer', 'integer'], 'setof integer');

  -- parameter names
  RETURN QUERY
  SELECT bag_has(
    $$SELECT proargnames from pg_proc where proname = 'vrp_knapsack'$$,
    $$SELECT '{"inner_query","capacity","max_rows","item_id"}'::TEXT[]$$
  );

  -- parameter types
  RETURN QUERY
  SELECT set_eq(
    $$SELECT  proallargtypes from pg_proc where proname = 'vrp_knapsack'$$,
    $$VALUES
      (ARRAY[25,23,23,23]::OID[])
    $$
  );

  -- vrp_multiple_knapsack
  RETURN QUERY
  SELECT has_function('vrp_multiple_knapsack');
  RETURN QUERY
  SELECT has_function('vrp_multiple_knapsack', ARRAY['text', 'integer[]', 'integer']);
  RETURN QUERY
  SELECT function_returns('vrp_multiple_knapsack', ARRAY['text', 'integer[]', 'integer'], 'setof record');

  -- parameter names
  RETURN QUERY
  SELECT bag_has(
    $$SELECT proargnames from pg_proc where proname = 'vrp_multiple_knapsack'$$,
    $$SELECT '{"inner_query","capacities","max_rows","knapsack_number","item_id"}'::TEXT[]$$
  );

  -- parameter types
  RETURN QUERY
  SELECT set_eq(
    $$SELECT  proallargtypes from pg_proc where proname = 'vrp_multiple_knapsack'$$,
    $$VALUES
      (ARRAY[25,1007,23,23,23]::OID[])
    $$
  );

  -- vrp_bin_packing
  RETURN QUERY
  SELECT has_function('vrp_bin_packing');
  RETURN QUERY
  SELECT has_function('vrp_bin_packing', ARRAY['text', 'integer', 'integer']);
  RETURN QUERY
  SELECT function_returns('vrp_bin_packing', ARRAY['text', 'integer', 'integer'], 'setof record');

  -- parameter names
  RETURN QUERY
  SELECT bag_has(
    $$SELECT proargnames from pg_proc where proname = 'vrp_bin_packing'$$,
    $$SELECT '{"inner_query","bin_capacity","max_rows","bin_number","item_id"}'::TEXT[]$$
  );

  -- parameter types
  RETURN QUERY
  SELECT set_eq(
    $$SELECT  proallargtypes from pg_proc where proname = 'vrp_bin_packing'$$,
    $$VALUES
      (ARRAY[25,23,23,23,23]::OID[])
    $$
  );
END;
$BODY$
LANGUAGE plpgsql;

SELECT types_check();

SELECT * FROM finish();
ROLLBACK;
